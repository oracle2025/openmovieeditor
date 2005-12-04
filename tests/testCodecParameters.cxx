
#include <lqt.h>

#include "../Codecs.H"

namespace nle 
{
	lqt_codec_info_t** g_audio_codec_info;
	lqt_codec_info_t** g_video_codec_info;
} /* namespace nle */

using namespace nle;

int main()
{
	g_audio_codec_info = lqt_query_registry( 1, 0, 1, 0 );
	g_video_codec_info = lqt_query_registry( 0, 1, 1, 0 );

	{
		CodecParameters cp( g_audio_codec_info, g_video_codec_info );
		cp.setVideoCodec( g_video_codec_info[11] );
		ParameterValue val("Zero");
		cp.setVideoParameter( "me_method", val );
	}

	
	lqt_destroy_codec_info( g_audio_codec_info );
	lqt_destroy_codec_info( g_video_codec_info );

	return 0;
	
}



