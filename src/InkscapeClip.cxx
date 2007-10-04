/*  InkscapeClip.cxx
 *
 *  Copyright (C) 2007 Richard Spindler <richard.spindler AT gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <FL/Fl_Shared_Image.H>
#include <FL/Fl.H>

#include "InkscapeClip.H"
#include "ErrorDialog/IErrorHandler.H"
#include "ImageClipArtist.H"
#include "helper.H"
#include "globals.H"
#include "timeline/Track.H"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "VideoViewGL.H"

namespace nle
{
static string random_file_name()
{
	char random_end[17];
	string filename = "";
	for ( int i = 0; i < 16; i++ ) {
		random_end[i] = '0' + rand() % 10;
	}
	random_end[16] = '\0';
	
	filename += random_end;
	return filename;
}

static int copy_file( string source, string destination ) { //This sucks
	string command = "cp ";
	command += "\"";
	command += source;
	command += "\" ";
	command += "\"";
	command += destination;
	command += "\"";
	return system( command.c_str() );
}
static void detect_svg_update_callback( void* data );

InkscapeClip::InkscapeClip( Track* track, int64_t position, int64_t length, int id, ClipData* data )
	: FilterClip( track, position, id ), VideoEffectClip( this )
{
	"/usr/share/openmovieeditor/svg-titles/0000.svg";
	"720x576";
	"inkscape --export-png=123.png /usr/share/openmovieeditor/svg-titles/0002.svg";
	/*

	Clip Create: copy svg-template into Projects Folder $HOME/Video\ Projects/random_file_name().svg
	Clip Render: generate png from svg, using inkscape $HOME/Video\ Projects/random_file_name().png
	Clip Edit: spawn inkscape, with copy from Projects Folder, and check
	whether it was saved, when new version is detetected, run Clip Render

	Extend the context Menü of the clip, for editing

	*/
	m_unique_id = random_file_name();
	string svg_filename = string(getenv("HOME")) + "/" + "Video Projects/" + m_unique_id + ".svg";
	string png_filename = string(getenv("HOME")) + "/" + "Video Projects/" + m_unique_id + ".png";
	copy_file( "/usr/share/openmovieeditor/svg-titles/0002.svg", svg_filename );
	string inkscape_command = "inkscape --export-png=\"";
	inkscape_command += png_filename;
	inkscape_command += "\" \"";
	inkscape_command += svg_filename;
	inkscape_command += "\"";
	system( inkscape_command.c_str() );

	m_ok = false;
	m_artist = 0;
	m_image = Fl_Shared_Image::get( png_filename.c_str() );
	if ( !m_image ) {
		ERROR_DETAIL( "This is not an image file" );
		return;
	}
	if ( m_image->w() > 1024 || m_image->h() > 1024 ) {
		ERROR_DETAIL( "This image is to big, maximum is 1024x1024" );
		return;
	}


	if ( length > 0 ) {
		m_length = length;
	} else {
		m_length = NLE_TIME_BASE * 10;
	}
	m_frame.x = m_frame.y = 0;
	m_frame.w = m_image->w();
	m_frame.h = m_image->h();
	m_frame.alpha = 1.0;
	m_frame.dirty = true;
	m_frame.cacheable = true;
	m_frame.interlace_mode = 0;
	m_frame.first_field = true;
	m_frame.scale_x = 0;
	m_frame.scale_y = 0;
	m_frame.crop_left = 0;
	m_frame.crop_right = 0;
	m_frame.crop_top = 0;
	m_frame.crop_bottom = 0;
	m_frame.tilt_x = 0;
	m_frame.tilt_y = 0;
	if ( m_image->d() == 4 ) {
		m_frame.has_alpha_channel = true;
	} else if ( m_image->d() == 3 ) {
		m_frame.has_alpha_channel = false;
	} else {
		ERROR_DETAIL( "This image file has a wrong color depth,\nonly RGB and RGBA images are supported" );
		return;
	}
	//m_frame.RGB = new (unsigned char)[m_frame.w * m_frame.h * 4];
	char** d = (char**)m_image->data();
	m_frame.RGB = (unsigned char *)d[0];
	
	if ( !track->render_mode() ) {
		m_artist = new ImageClipArtist( m_image );
	}
	
	unsigned long gcd_wh = gcd( m_frame.w, m_frame.h );
	m_aspectHeight = m_frame.h / gcd_wh; 
	m_aspectWidth = m_frame.w /gcd_wh;
	m_aspectRatio = (float)m_aspectWidth / (float)m_aspectHeight;
	m_ok = true;
	setEffects( data );
	Fl::add_timeout(1.0, detect_svg_update_callback, this );
}
InkscapeClip::InkscapeClip( Track* track, int64_t position, int64_t length, int id, TiXmlElement* xml_node )
	: FilterClip( track, position, id ), VideoEffectClip( this )
{
	assert( xml_node );
	m_image = 0;
	m_artist = 0;
	m_ok = false;
	const char* textp;
	if ( ( textp = xml_node->Attribute( "unique_id" ) ) ) {
		m_unique_id = textp;
	} else {
		return;
	}
	string png_filename = string(getenv("HOME")) + "/" + "Video Projects/" + m_unique_id + ".png";
	m_image = Fl_Shared_Image::get( png_filename.c_str() );
	if ( !m_image ) {
		cerr << "Inkscape Clip: Not an Image" << endl;
		return;
	}
	if ( length > 0 ) {
		m_length = length;
	} else {
		m_length = NLE_TIME_BASE * 10;
	}
	m_frame.x = m_frame.y = 0;
	m_frame.w = m_image->w();
	m_frame.h = m_image->h();
	m_frame.alpha = 1.0;
	m_frame.cacheable = true;
	m_frame.dirty = true;
	m_frame.interlace_mode = 0;
	m_frame.first_field = true;
	m_frame.scale_x = 0;
	m_frame.scale_y = 0;
	m_frame.crop_left = 0;
	m_frame.crop_right = 0;
	m_frame.crop_top = 0;
	m_frame.crop_bottom = 0;
	m_frame.tilt_x = 0;
	m_frame.tilt_y = 0;

	if ( m_image->d() == 4 ) {
		m_frame.has_alpha_channel = true;
	} else if ( m_image->d() == 3 ) {
		m_frame.has_alpha_channel = false;
	}
	char** d = (char**)m_image->data();
	m_frame.RGB = (unsigned char *)d[0];

	if ( !track->render_mode() ) {
		m_artist = new ImageClipArtist( m_image );
	}

	unsigned long gcd_wh = gcd( m_frame.w, m_frame.h );
	m_aspectHeight = m_frame.h / gcd_wh; 
	m_aspectWidth = m_frame.w /gcd_wh;
	m_aspectRatio = (float)m_aspectWidth / (float)m_aspectHeight;
	detectSvgUpdate();

	Fl::add_timeout(1.0, detect_svg_update_callback, this );
	m_ok = true;
}

InkscapeClip::~InkscapeClip()
{
	Fl::remove_timeout( detect_svg_update_callback, this );
	if ( m_image ) {
		m_image->release();
		m_image = 0;
	}
	if ( m_artist ) {
		delete m_artist;
	}
	
}
int64_t InkscapeClip::length()
{
	return m_length;
}

void InkscapeClip::doAction( int index )
{
	string svg_filename = string(getenv("HOME")) + "/" + "Video Projects/" + m_unique_id + ".svg";
	string inkscape_command = "inkscape \"";
	inkscape_command += svg_filename;
	inkscape_command += "\"";
	//system( inkscape_command.c_str() );

	const char* args[] = { "/bin/sh", "-c", inkscape_command.c_str(), (char *)0 };
	if ( fork() == 0 ) {
		execvp( "/bin/sh", (char* const*)args );
	}
}
static void detect_svg_update_callback( void* data ) {
	InkscapeClip* ic = (InkscapeClip*)data;
	ic->detectSvgUpdate();
	Fl::repeat_timeout(1.0, detect_svg_update_callback, data);
}
void InkscapeClip::detectSvgUpdate()
{
	string svg_filename = string(getenv("HOME")) + "/" + "Video Projects/" + m_unique_id + ".svg";
	string png_filename = string(getenv("HOME")) + "/" + "Video Projects/" + m_unique_id + ".png";
	struct stat statbuf1;
	struct stat statbuf2;
	int r = stat( svg_filename.c_str(), &statbuf1 );
	assert( r != -1 );
	r = stat( png_filename.c_str(), &statbuf2 );
	assert( r != -1 );
	if ( statbuf1.st_mtime > statbuf2.st_mtime ) {
		string inkscape_command = "inkscape --export-png=\"";
		inkscape_command += png_filename;
		inkscape_command += "\" \"";
		inkscape_command += svg_filename;
		inkscape_command += "\"";
		system( inkscape_command.c_str() );
		if ( m_image ) {
			m_image->release();
			m_image = 0;
		}
		if ( m_artist ) {
			delete m_artist;
		}
		m_artist = 0;
		m_image = Fl_Shared_Image::get( png_filename.c_str() );
		if ( !m_image ) {
			cerr << "Inkscape Clip: Not an Image" << endl;
			return;
		}
		m_frame.w = m_image->w();
		m_frame.h = m_image->h();
		if ( m_image->d() == 4 ) {
			m_frame.has_alpha_channel = true;
		} else if ( m_image->d() == 3 ) {
			m_frame.has_alpha_channel = false;
		}
		char** d = (char**)m_image->data();
		m_frame.RGB = (unsigned char *)d[0];

		if ( !track()->render_mode() ) {
			m_artist = new ImageClipArtist( m_image );
		}

		unsigned long gcd_wh = gcd( m_frame.w, m_frame.h );
		m_aspectHeight = m_frame.h / gcd_wh; 
		m_aspectWidth = m_frame.w /gcd_wh;
		m_aspectRatio = (float)m_aspectWidth / (float)m_aspectHeight;
		m_frame.dirty = true;
		g_videoView->redraw();
	}
}

frame_struct* InkscapeClip::getRawFrame( int64_t position, int64_t &position_in_file )
{
	m_frame.alpha = 1.0;
	position_in_file = position - m_position;
	if ( position >= m_position && position <= m_position + m_length ) {
		return &m_frame;
	} else {
		return 0;
	}
}
int InkscapeClip::w()
{
	return m_image->w();
}
int InkscapeClip::h()
{
	return m_image->h();
}
int64_t InkscapeClip::trimA( int64_t trim )
{
	if ( m_length - trim < 0 ) {
		return 0;
	}
	if ( m_position + trim < 0 ) {
		trim = (-1) * m_position;
	}
	m_length -= trim;
	m_position += trim;
	return trim;
}
int64_t InkscapeClip::trimB( int64_t trim )
{
	if ( m_length - trim < 0 ) {
		return 0;
	}
	m_length -= trim;
	return trim;
}
int64_t InkscapeClip::fileLength()
{
	return -1;
}

} /* namespace nle */
