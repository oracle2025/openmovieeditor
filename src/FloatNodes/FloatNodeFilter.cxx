/* FloatNodeFilter.cxx
 *
 *  Copyright (C) 2008 Richard Spindler <richard.spindler AT gmail.com>
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

#include "FloatNodeFilter.H"
#include "FloatNodeFilterDialog.H"
#include <tinyxml.h>
#include "sl/sl.h"
#include "SinkFloatNode.H"
#include "SrcFloatNode.H"
#include "timebase.H"
#include <iostream>
#include "helper.H"
#include "IFloatNodeFilterFactoryPlugin.H"
#include "Float0rNode.H"
#include "NodeFilterFloat0rFactoryPlugin.H"
/*
#include "BezierCurveFloatNode.H"
#include "FloatNodeFilterPreviewFactoryPlugin.H"
#include "FloatNodeFilterImageFactoryPlugin.H"
#include "PreviewFloatNode.H"
*/
#include "LazyFrame.H"

using namespace std;

namespace nle {

FloatNodeFilter::FloatNodeFilter( int w, int h )
{
	m_w = w;
	m_h = h;
	m_dialog = 0;
	m_filters = 0;
	m_filters = (filters*)sl_push( m_filters, filters_create( 10,200, 50, 50, new SinkFloatNode(), "Output", 0 ) );
	filters* C = m_filters;
	m_sink_node = C->node;
	m_filters = (filters*)sl_push( m_filters, filters_create( 10,100, 50, 50, new SrcFloatNode( this ), "Input", 1 ) );
	C = m_filters;
	m_src_node = C->node;
	m_src_node->node->outputs[0] = m_sink_node->node;
	m_sink_node->node->inputs[0] = m_src_node->node;

	gavl_video_format_t format;
	format.frame_width  = w;
	format.frame_height = h;
	format.image_width  = w;
	format.image_height = h;
	format.pixel_width = 1;
	format.pixel_height = 1;
	format.pixelformat = GAVL_RGBA_FLOAT;
	format.interlace_mode = GAVL_INTERLACE_NONE;

	m_lazy_frame = new LazyFrame( &format );
	m_gavl_frame = gavl_video_frame_create( 0 );
	m_gavl_frame->strides[0] = w * 4 *4; //4 channels with 4 bytes per channel
	m_lazy_frame->put_data( m_gavl_frame );
	m_frame_cache = 0;
}
FloatNodeFilter::~FloatNodeFilter()
{
	if ( m_lazy_frame ) {
		delete m_lazy_frame;
	}
	if ( m_gavl_frame ) {
		gavl_video_frame_null( m_gavl_frame );
		gavl_video_frame_destroy( m_gavl_frame );
	}
	if ( m_dialog ) {
		delete m_dialog;
	}
	filters* n;
	while ( ( n = (filters*)sl_pop( &m_filters ) ) ) {
		delete n->node;
		delete n;
	}

}
void FloatNodeFilter::writeXML( TiXmlElement* xml_node )
{
	TiXmlElement* project = xml_node;

	TiXmlElement* xml_filter;
	for ( filters* filter_node = m_filters; filter_node; filter_node = filter_node->next ) {
		xml_filter = new TiXmlElement( "filter" );
		project->LinkEndChild( xml_filter );
		xml_filter->SetAttribute( "x", filter_node->x );
		xml_filter->SetAttribute( "y", filter_node->y );
		xml_filter->SetAttribute( "w", filter_node->w );
		xml_filter->SetAttribute( "h", filter_node->h );
		xml_filter->SetAttribute( "name", filter_node->name.c_str() );
		xml_filter->SetAttribute( "id", filter_node->id );
		xml_filter->SetAttribute( "identifier", filter_node->node->identifier() );
		filter_node->node->writeXML( xml_filter );
		Float0rNode* frei0r_node = dynamic_cast<Float0rNode*>(filter_node->node);
		if ( frei0r_node ) {
			string identifier( "effect:float0r:" );
			identifier += filter_node->name;
			xml_filter->SetAttribute( "identifier", identifier.c_str() );
		}
		for ( int i = 0; i < filter_node->input_count; i++ ) {
			if ( filter_node->inputs[i] ) {
				TiXmlElement* xml_input = new TiXmlElement( "input" );
				xml_filter->LinkEndChild( xml_input );
				xml_input->SetAttribute( "nr", i );
				xml_input->SetAttribute( "input_id", filter_node->inputs[i]->id );
			}
		}
		for ( int i = 0; i < filter_node->output_count; i++ ) {
			if ( filter_node->outputs[i] ) {
				TiXmlElement* xml_output = new TiXmlElement( "output" );
				xml_filter->LinkEndChild( xml_output );
				xml_output->SetAttribute( "nr", i );
				xml_output->SetAttribute( "output_id", filter_node->outputs[i]->id );
			}
		}
	}

}
void FloatNodeFilter::readXML( TiXmlElement* xml_node )
{
	/*FloatNodeFilterBezierCurveFactoryPlugin bezier_node_factory_plugin;
	FloatNodeFilterPreviewFactoryPlugin preview_factory_plugin;
	FloatNodeFilterImageFactoryPlugin image_factory_plugin;*/
	TiXmlElement* xml_filter = TiXmlHandle( xml_node ).FirstChild( "filter" ).Element();
	for ( ; xml_filter; xml_filter = xml_filter->NextSiblingElement( "filter" ) ) {
		int id;
		if ( xml_filter->Attribute( "id", &id ) ) {
			filters* filter_node = 0;
			for ( filter_node = m_filters; filter_node; filter_node = filter_node->next ) {
				if ( filter_node->id == id ) {
					int x, y, w, h;
					if ( xml_filter->Attribute( "x", &x ) ) {
						filter_node->x = x;
					}
					if ( xml_filter->Attribute( "y", &y ) ) {
						filter_node->y = y;
					}
					if ( xml_filter->Attribute( "w", &w ) ) {
						filter_node->w = w;
					}
					if ( xml_filter->Attribute( "h", &h ) ) {
						filter_node->h = h;
					}
					break;
				}
			}
			if ( !filter_node ) {
				//Create from Float0r
				const char* identifier = xml_filter->Attribute( "identifier" );
				IFloatNodeFilterFactoryPlugin* ffp = 0;
				/*
				if ( identifier && strcmp("effect:builtin:BezierCurveFilter", identifier ) == 0 ) {
					ffp = &bezier_node_factory_plugin;
				} else if ( identifier && strcmp("effect:builtin:Preview", identifier ) == 0 ) {
					ffp = &preview_factory_plugin;
				} else if ( identifier && strcmp("effect:builtin:Image", identifier ) == 0 ) {
					ffp = &image_factory_plugin;
				} else*/ if ( identifier && strncmp("effect:float0r:", identifier, strlen("effect:float0r:") ) == 0 && g_node_filter_float0r_factory ) {
					identifier += strlen("effect:float0r:");
					ffp = g_node_filter_float0r_factory->get( identifier );
				}
				if ( ffp ) {
					int x = 10;
					int y = 10;
					int w = 50;
					int h = 50;
					int id = -1;
					xml_filter->Attribute( "x", &x );
					xml_filter->Attribute( "y", &y );
					xml_filter->Attribute( "w", &w );
					xml_filter->Attribute( "h", &h );
					xml_filter->Attribute( "id", &id );

					m_filters = (filters*)sl_push( m_filters, filters_create( x, y, w, h, ffp->get_i_node( m_w, m_h ), ffp->name(), id ) );
					m_filters->node->readXML( xml_filter );
				}

			}
		}
	}

	xml_filter = TiXmlHandle( xml_node ).FirstChild( "filter" ).Element();
	for ( ; xml_filter; xml_filter = xml_filter->NextSiblingElement( "filter" ) ) {
		int id;
		if ( xml_filter->Attribute( "id", &id ) ) {
			filters* filter_node = 0;
			for ( filter_node = m_filters; filter_node; filter_node = filter_node->next ) {
				if ( filter_node->id == id ) {
					TiXmlElement* xml_input =  TiXmlHandle( xml_filter ).FirstChildElement( "input" ).Element();
					for ( ; xml_input; xml_input = xml_input->NextSiblingElement( "input" ) ) {
						int nr, input_id;
						if ( xml_input->Attribute( "nr", &nr ) && xml_input->Attribute( "input_id", &input_id ) ) {
							filters* src_node = 0;
							for ( src_node = m_filters; src_node; src_node = src_node->next ) {
								if ( src_node->id == input_id ) {
									int output_slot = 0;
									while ( src_node->outputs[output_slot] && output_slot < MAX_FILTER_OUT - 1 ) {
										output_slot++;
									}
									filter_node->inputs[nr] = src_node;
									src_node->outputs[output_slot] = filter_node;
									src_node->target_slots[output_slot] = nr;
									break;
								}
							}
						}
					}
					break;
				}
			}
		}
	}
}
const char* FloatNodeFilter::name()
{
	return "FloatNode Compositor";
}
LazyFrame* FloatNodeFilter::getFrame( LazyFrame* frame, int64_t position )
{
	gavl_video_format_t format;
	gavl_video_format_copy( &format, frame->format() );
	format.pixelformat = GAVL_RGBA_FLOAT;
	frame->set_target( &format );
	m_frame_cache = (float*)frame->target()->planes[0];
	m_gavl_frame->planes[0] = (uint8_t*)m_sink_node->getFrame( 0, position / (double)NLE_TIME_BASE );

/*
	for ( filters* filter_node = m_filters; filter_node; filter_node = filter_node->next ) {
		PreviewFloatNode* preview_node = dynamic_cast<PreviewFloatNode*>(filter_node->node);
		if ( preview_node ) {
			preview_node->getFrame( 0, position / (double)NLE_TIME_BASE );
		}
	}
	*/
	if ( m_gavl_frame->planes[0] ) {
		return m_lazy_frame;
	} else {
		return frame;
	}
}
IEffectDialog* FloatNodeFilter::dialog()
{
	if ( !m_dialog ) {
		m_dialog = new FloatNodeFilterDialog( this );
	}
	return m_dialog;
}

} /* namespace nle */

