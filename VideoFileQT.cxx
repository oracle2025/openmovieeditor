#include <quicktime/lqt.h>
#include <quicktime/colormodels.h>
#include "VideoFileQT.h"
#include <string.h>
#include <iostream>
using namespace std;

namespace nle
{

VideoFileQT::VideoFileQT( const char* filename )
{
	m_ok = false;
	m_qt = NULL;
	m_frame = NULL;
	m_rows = NULL;
	char *lqt_sucks_filename = strdup( filename );
	if ( !quicktime_check_sig( lqt_sucks_filename ) )
		return;
	m_qt = quicktime_open( lqt_sucks_filename, true, false );
	free(lqt_sucks_filename);
	if ( quicktime_video_tracks( m_qt ) == 0 )
		return;
	if ( !quicktime_supported_video( m_qt, 0 ) )
		return;
	lqt_set_cmodel( m_qt, 0, BC_RGB888);
	int m_width = quicktime_video_width( m_qt, 0 );
	int m_height = quicktime_video_height( m_qt, 0 );
	m_frame = new unsigned char[m_width * m_height * 4];
	m_rows = new (unsigned char*)[m_height * sizeof(char*)];
	for (int i = 0; i < m_height; i++) {
                m_rows[i] = m_frame + m_width * 3 * i;
	}
	cout << "Video Duration: " << lqt_video_duration( m_qt, 0 ) << endl;
	cout << "Width: " << quicktime_video_width( m_qt, 0 ) << endl;
	cout << "Height: " << quicktime_video_height( m_qt, 0 ) << endl;
	cout << "Video FPS: " << quicktime_frame_rate( m_qt, 0 ) << endl;
	cout << "Video Length: " << quicktime_video_length( m_qt, 0 ) << endl;
	cout << "Video Timescale: " << lqt_video_time_scale( m_qt, 0 ) << endl;
	cout << "Audio Length: " << quicktime_audio_length( m_qt, 0 ) << endl;
	cout << "Audio Samplerate: " << quicktime_sample_rate( m_qt, 0 ) << endl;
	m_ok = true;
}
VideoFileQT::~VideoFileQT()
{
	if ( m_frame )
		delete [] m_frame;
	if ( m_rows )
		delete [] m_rows;
	if ( m_qt )
		quicktime_close( m_qt );
}
bool VideoFileQT::ok() { return m_ok; }
int64_t VideoFileQT::length()
{
	return quicktime_video_length( m_qt, 0 );
}
int VideoFileQT::fps()
{
}
unsigned char* VideoFileQT::read()
{
	quicktime_decode_video( m_qt, m_rows, 0);
	return m_frame;
}
void VideoFileQT::seek( int64_t frame )
{
	quicktime_set_video_position( m_qt, frame, 0 );
}

}; /* namespace nle */
