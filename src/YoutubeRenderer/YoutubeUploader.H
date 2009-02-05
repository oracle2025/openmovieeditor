/*  YoutubeUploader.H
 *
 *  Copyright (C) 2009 Richard Spindler <richard.spindler AT gmail.com>
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

#ifndef _YOUTUBE_UPLOADER_H_
#define _YOUTUBE_UPLOADER_H_

#include <string>

namespace nle
{
	
class IProgressListener;

class YoutubeUploader
{
	public:
		YoutubeUploader();
		~YoutubeUploader();
		bool login( const char* user, const char* passwd );
		/* Returns true upon successful Login */
		void encode( IProgressListener* progress_listener = 0 );
		void upload( IProgressListener* progress_listener = 0 );
		void setTitle( const char* title );
		void setDescription( const char* description );
		void setKeywords( const char* keywords );
	private:
		bool m_authentication_ok;
		std::string m_auth_token;
		std::string m_temp_video_file_name;
		std::string m_title;
		std::string m_description;
		std::string m_keywords;
		IProgressListener* m_upload_progress_listener;
	public:
		void login_write_callback( void *ptr, size_t size, size_t nmemb );
		void login_header_callback( void *ptr, size_t size, size_t nmemb );
		int upload_progress_callback( double dltotal, double dlnow, double ultotal, double ulnow );
};

} /* namespace nle */

#endif /* _YOUTUBE_UPLOADER_H_ */


