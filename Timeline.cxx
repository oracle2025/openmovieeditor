#include "Timeline.H"
#include "VideoTrack.H"
#include "AudioTrack.H"
#include "VideoClip.H"
#include "Track.H"
#include "SwitchBoard.H"
#include <string>
#include <iostream>

using namespace std;


namespace nle
{

Timeline* g_timeline = 0;

Timeline::Timeline()
{
	VideoTrack *vt;
	AudioTrack *at;
	vt = new VideoTrack( 0 );
	m_allTracks.push_back( vt );
	m_videoTracks.push_back( vt );
	vt = new VideoTrack( 1 );
	m_allTracks.push_back( vt );
	m_videoTracks.push_back( vt );
	at = new AudioTrack( 2 );
	m_allTracks.push_back( at );
	m_audioTracks.push_back( at );
	at = new AudioTrack( 3 );
	m_allTracks.push_back( at );
	m_audioTracks.push_back( at );
	SwitchBoard::i()->timeline( this );
	m_playPosition = 0;
	m_samplePosition = 0;

	g_timeline = this; //Singleton sucks, this is better :)
}
Timeline::~Timeline()
{
	g_timeline = NULL;
	m_videoTracks.clear();
	m_audioTracks.clear();
	Track *track;
	while( m_allTracks.size() ) {
		track = *m_allTracks.end();
		m_allTracks.pop_back();
		delete track;
	}
}

void Timeline::add_video( int track, int64_t position, const char* filename )
{
	std::list< Track* >::iterator i = m_allTracks.begin();
	for ( int j = 0; j < track; j++ )
		i++;
	((VideoTrack*)(*i))->add_video( position, filename );
}
void Timeline::add_audio( int track, int64_t position, const char* filename )
{
	std::list< AudioTrack* >::iterator iter = m_audioTracks.begin();
	while ( iter!= m_audioTracks.end() ) {
		if ( (*iter)->num() == track ) {
			break;
		}
		iter++;
	}
	(*iter)->add_audio( position, filename );
}
void reset_helper( Track* track ) { track->reset(); }
void Timeline::reset()
{
	m_playPosition = 0;
	m_samplePosition = 0;
	for_each( m_allTracks.begin(), m_allTracks.end(), reset_helper );
}
frame_struct* Timeline::nextFrame()
{
	frame_struct* res = NULL;
	for ( std::list< VideoTrack* >::iterator i = m_videoTracks.begin(); i != m_videoTracks.end(); i++ ) {
		VideoTrack* current = *i;
		std::list< Clip* >* clips = current->getClips();
		for ( std::list< Clip* >::iterator j = clips->begin(); j != clips->end(); j++ ) {
			res = ((VideoClip*)(*j))->getFrame(m_playPosition);
			if ( res ) {
				m_playPosition++;
				return res;
			}
		}
	}
	m_playPosition++;
	return res;
}

unsigned int mixChannels( float *A, float *B, float* out, unsigned int frames )
	// Mix function for (-1)-(1) float audio (2 Channels)
{
	unsigned int i;
	float *p_output = out;
	float *p_A = A;
	float *p_B = B;
	for ( i = frames * 2; i > 0; i-- ){
		if ( *p_A < 0 && *p_B < 0 ) {
			*p_output = ( *p_A + 1 ) * ( *p_B + 1 ) - 1;
		} else {
			*p_output = 2 * ( *p_A + *p_B + 2 ) - ( *p_A + 1 ) * ( *p_B + 1) - 3;
		}
		p_output++;
		p_A++;
		p_B++;

	}
	return frames;
}

#define FRAMES_PER_BUFFER 256
int Timeline::fillBuffer( float* output, unsigned long frames )
{
	static float buffer1[FRAMES_PER_BUFFER*2] = {0};
	static float buffer2[FRAMES_PER_BUFFER*2] = {0};
	int rv;
	int max_frames = 0;
	std::list< AudioTrack* >::iterator iter = m_audioTracks.begin();
	if ( iter == m_audioTracks.end() )
		return 0;
	rv = (*iter)->fillBuffer( buffer1, frames, m_samplePosition );
	max_frames = rv;
	iter++;
	if ( iter == m_audioTracks.end() ) { //Only one Track
		for ( unsigned long i = 0; i < frames; i++ ) {
			output[i] = buffer1[i];
			output[i+1] = buffer1[i+1];
		}
		m_samplePosition += max_frames;
		return max_frames;
	}
	rv = (*iter)->fillBuffer( buffer2, frames, m_samplePosition );
	max_frames = rv > max_frames ? rv : max_frames;
	mixChannels( buffer1, buffer2, output, frames);
	iter++;
	while ( iter != m_audioTracks.end() ) {
		rv = (*iter)->fillBuffer( buffer1, frames, m_samplePosition );
		max_frames = rv > max_frames ? rv : max_frames;
		mixChannels( output, buffer1, output, frames );
		iter++;
	}
	m_samplePosition += max_frames;
	return max_frames;
}



} /* namespace nle */
