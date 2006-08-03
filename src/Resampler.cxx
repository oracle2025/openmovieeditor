/*  Resampler.cxx
 *
 *  Copyright (C) 2006 Richard Spindler <richard.spindler AT gmail.com>
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

#include <gavl.h>

#include "Resampler.H"


namespace nle
{

Resampler::Resampler( IAudioFile* audiofile )
	: m_audiofile( audiofile )
{
	m_filename = audiofile->filename();
	gavl_audio_format_t input_format;
	gavl_audio_format_t output_format;
	input_format.samplerate = 44100; //TODO: Change this
	input_format.num_channels = 2;
	input_format.sample_format = GAVL_SAMPLE_FLOAT;
	input_format.interleave_mode = GAVL_INTERLEAVE_2;
	
	output_format.samplerate = 48000;
	output_format.num_channels = 2;
	output_format.sample_format = GAVL_SAMPLE_FLOAT;
	output_format.interleave_mode = GAVL_INTERLEAVE_2;
	
	
	m_converter = gavl_audio_converter_create();
	gavl_audio_converter_init( m_converter, &input_format, &output_format );

	m_output_frame.samples.f = new float[4096 * 2 + 20];
	m_output_frame.valid_samples.f = 4096 * 2 + 20;
	
	m_input_frame; // TODO: errechnit sich aus der Samplerate
	
	m_ok = true;
}

Resampler::~Resampler()
{
	gavl_audio_converter_destroy( m_converter );
	delete m_audiofile;
	delete m_output_frame.samples.f;
	delete m_input_frame.samples.f;
}

void Resampler::seek( int64_t sample )
{
	//Verhältnis berechnen
	m_audiofile->seek();
}

int Resampler::fillBuffer( float* output, unsigned long frames )
{
	int c = m_audiofile->fillBuffer( m_input_frame.samples.f, 1234 );
	m_input_frame.valid_samples = c * 2;
	
	gavl_audio_convert( m_converter, &m_input_frame, &m_output_frame );
	//Minimum size is input_frame_size * output_samplerate / input_samplerate + 10
}

} /* namespace nle */
