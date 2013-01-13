/*  YoutubeUploader.cxx
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

#include "YoutubeUploader.H"
#include "VideoWriterMP4.H"
#include "Renderer.H"
#include "IProgressListener.H"
#include <cassert>
#include <iostream>
#include <curl/curl.h>
#include <curl/easy.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <cstdlib>

#define APPLICATION_KEY "ytapi-RichardSpindler-OpenMovieEditor-f39fht0c-0"
#define DEVELOPER_KEY "AI39si4urjpjq3Lj2VXBptv3Od-0zOUdSF5xS3ZC96OYyCCPsByzwbrnDnJEvXzu1xlT5opS9jDDgpvGcyNXCO49-Wc0J102Pw"

namespace nle
{

const char* xml_metadata = 
"<?xml version=\"1.0\"?>                                                          "
"<entry xmlns=\"http://www.w3.org/2005/Atom\"                                     "
"  xmlns:media=\"http://search.yahoo.com/mrss/\"                                  "
"  xmlns:yt=\"http://gdata.youtube.com/schemas/2007\">                            "
"  <media:group>                                                                  "
"    <media:title type=\"plain\">%s</media:title>                                 "
"    <media:description type=\"plain\">                                           "
"      %s                                                                         "
"    </media:description>                                                         "
"    <media:category                                                              "
"      scheme=\"http://gdata.youtube.com/schemas/2007/categories.cat\">Sports     "
"    </media:category>                                                            "
"    <media:keywords>%s</media:keywords>                                          "
"  </media:group>                                                                 "
"</entry>                                                                         ";
	
YoutubeUploader::YoutubeUploader()
{
	m_authentication_ok = false;
	m_title = "Video uploaded from Open Movie Editor";
	m_description = "Video Description";
	m_keywords = "";
	curl_global_init(CURL_GLOBAL_ALL);
	m_upload_progress_listener = 0;
}
YoutubeUploader::~YoutubeUploader()
{
}
static size_t login_write_function( void *ptr, size_t size, size_t nmemb, void *data )
{
	YoutubeUploader* uploader = (YoutubeUploader*)data;
	uploader->login_write_callback( ptr, size, nmemb );
	return ( size * nmemb );
}
/* Checks whether there is a HTTP/1.1 200 OK header, and then login is ok*/
static size_t login_header_function( void *ptr, size_t size, size_t nmemb, void *data )
{
	YoutubeUploader* uploader = (YoutubeUploader*)data;
	uploader->login_header_callback( ptr, size, nmemb );
	return ( size * nmemb );
}
void YoutubeUploader::login_write_callback( void *ptr, size_t size, size_t nmemb )
{
	char* auth_tmp_p;
	char* auth_newline_p;
	char result_string[CURL_MAX_WRITE_SIZE];
	memcpy( result_string, ptr, size * nmemb );
	m_auth_token = "";
	if ( m_authentication_ok ) {
		auth_tmp_p = strstr( result_string, "Auth=" );
		auth_tmp_p += 5;
		auth_newline_p = strstr( auth_tmp_p, "\n" );
		auth_newline_p[0] = '\0';
		m_auth_token = auth_tmp_p;
	}
}
void YoutubeUploader::login_header_callback( void *ptr, size_t size, size_t nmemb )
{
	char str[CURL_MAX_WRITE_SIZE];
	memcpy( str, ptr, size * nmemb );
	str[size*nmemb-2] = '\0'; // -2 to get rid of newline and linereturn thingis
	if ( strncmp( "HTTP", str, 4 ) == 0) {
		m_authentication_ok = false;
		if ( strcmp( "HTTP/1.1 200 OK", str ) == 0 ) {
			m_authentication_ok = true;
		}
	}
}
bool YoutubeUploader::login( const char* user, const char* passwd )
{
	char error_buffer[CURL_ERROR_SIZE];
	const char* login_uri = "https://www.google.com/youtube/accounts/ClientLogin";
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
//	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
	struct curl_slist *headers=NULL;
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
	char login_postfields[255];
	snprintf( login_postfields, sizeof(login_postfields), "Email=%s&Passwd=%s&service=youtube&source=Test", user, passwd );
	login_postfields[sizeof(login_postfields)-1] = '\0';
	curl_easy_setopt(curl, CURLOPT_URL, login_uri);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, login_postfields);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buffer );

	curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, login_write_function ); /* Here the Auth Token is written */
	curl_easy_setopt (curl, CURLOPT_WRITEDATA, this );
	curl_easy_setopt (curl, CURLOPT_HEADERFUNCTION, login_header_function ); /* Here the HTTP response Code is written */
	curl_easy_setopt (curl, CURLOPT_WRITEHEADER, this );

	res = curl_easy_perform(curl);

	curl_slist_free_all (headers);
	curl_easy_cleanup( curl );
	if ( m_authentication_ok ) {
		std::cout << "Auth Token: " << m_auth_token << std::endl;
	}
	return m_authentication_ok;
}
void YoutubeUploader::encode( IProgressListener* progress_listener )
{
	std::cout << "YoutubeUploader::encode" << std::endl;
	{
		char* temp_filename = tempnam( 0, 0 );
		m_temp_video_file_name = temp_filename;
		free( temp_filename );
	}
	std::cout << m_temp_video_file_name << std::endl;
	VideoWriterMP4 *writer = new VideoWriterMP4( m_temp_video_file_name.c_str() );
	assert( writer->ok() );
	Renderer renderer( writer );
	renderer.go( progress_listener );
}

int upload_progress_function(void *data,
                      double dltotal,
		      double dlnow,
                      double ultotal,
                      double ulnow)
{
	YoutubeUploader* uploader = (YoutubeUploader*)data;
	return uploader->upload_progress_callback( dltotal, dlnow, ultotal, ulnow );
	//Returning a non-zero value from this callback will cause libcurl to abort the transfer and return CURLE_ABORTED_BY_CALLBACK.
}
int YoutubeUploader::upload_progress_callback( double dltotal, double dlnow, double ultotal, double ulnow )
{
	double percent = ulnow / ultotal;
	return m_upload_progress_listener->progress( percent );
}
void YoutubeUploader::upload( IProgressListener* progress_listener )
{
	m_upload_progress_listener = progress_listener;
	std::cout << "YoutubeUploader::upload" << std::endl;
	//Hint delete tmp-file when done.
	// tmp_rw is deleted automagically (see man tmpfile)
	
	if ( !m_authentication_ok ) {
		return;
	}

	CURL *curl;
	CURLcode res;
	struct curl_slist *headerlist=NULL;
	
/*** CODE COPIED FROM PROTOTYPE FOLLOWS ***/

	char buffer[255];

	char xml_text_buffer[16384];

	sprintf( xml_text_buffer, xml_metadata, m_title.c_str(), m_description.c_str(), m_keywords.c_str() );

	curl = curl_easy_init();
	headerlist = curl_slist_append(headerlist, "Expect:");
	sprintf(buffer,"Authorization: GoogleLogin auth=\"%s\"", m_auth_token.c_str() );
	headerlist = curl_slist_append(headerlist, buffer);
	headerlist = curl_slist_append(headerlist, "GData-Version: 2");
	sprintf(buffer,"X-GData-Client: %s", APPLICATION_KEY );
	headerlist = curl_slist_append(headerlist, buffer);
	sprintf(buffer,"X-GData-Key: key=%s", DEVELOPER_KEY );
	headerlist = curl_slist_append(headerlist, buffer );
	headerlist = curl_slist_append(headerlist, "Slug: video-for-youtube-upload.mp4");
	headerlist = curl_slist_append(headerlist, "Content-Type: multipart/related; boundary=\"example-1\"");

	FILE *tmp_rw;
	tmp_rw = tmpfile();

	const char* boundary_string = "example-1";

	int size_of_video_file;
	int length_of_xml_metadata = strlen(xml_text_buffer);

	struct stat video_file_stats;
	stat( m_temp_video_file_name.c_str(), &video_file_stats );
	size_of_video_file = video_file_stats.st_size;

	int length_of_post = size_of_video_file + length_of_xml_metadata + 250;


	sprintf( buffer, "\n--%s\nContent-Type: application/atom+xml; charset=UTF-8\n\n", boundary_string );
	fwrite( buffer, strlen(buffer), 1, tmp_rw );
	fwrite( xml_text_buffer, strlen(xml_text_buffer), 1, tmp_rw );
	sprintf( buffer, "\n--%s\nContent-Type: video/mp4\nContent-Transfer-Encoding: binary\n\n", boundary_string );
	fwrite( buffer, strlen(buffer), 1, tmp_rw );
	char* video_file_buffer = (char*)malloc( size_of_video_file ); //evil, harhar, better use sendfile

	FILE* video_file = fopen(m_temp_video_file_name.c_str(), "r");
	fread( video_file_buffer, size_of_video_file, 1, video_file );
	fclose( video_file );

	fwrite( video_file_buffer, size_of_video_file, 1, tmp_rw);
	free(video_file_buffer);
	
	sprintf( buffer, "\n--%s--\n\n", boundary_string );
	fwrite( buffer, strlen(buffer), 1, tmp_rw );

	rewind( tmp_rw );

	struct stat tmp_file_stats;
	fstat( fileno( tmp_rw ), &tmp_file_stats );

	if(curl) {
		char buffer[CURL_ERROR_SIZE];
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, buffer );
		curl_easy_setopt(curl, CURLOPT_URL, "http://uploads.gdata.youtube.com/feeds/api/users/default/uploads");
		//curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		curl_easy_setopt(curl, CURLOPT_READDATA, tmp_rw );
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, tmp_file_stats.st_size );
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, upload_progress_function );
		curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this );
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0 );
		//CURLOPT_POST
		/*
		 * Optionally, you can provide data to POST using the CURLOPT_READFUNCTION and CURLOPT_READDATA
		 * You can override the default POST Content-Type: header by setting your own with CURLOPT_HTTPHEADER.
		 *
		 * CURLOPT_READDATA
		 * Data pointer to pass to the file read function.
		 * If you don’t specify a read callback but instead rely on the default internal read function, this
		 * data must be a valid readable FILE *.
		 *
		 * CURLOPT_POSTFIELDSIZE
		 * If you want to post data to the server without letting libcurl do a strlen() to measure the data size, this option must be used.
		 *
		 */
		
		m_upload_progress_listener->start();
		res = curl_easy_perform(curl);
		m_upload_progress_listener->end();
		if (res) {
			printf("ERROR: %s\n", buffer);
		}


		//curl_formfree(formpost);
	}
	curl_slist_free_all (headerlist);
	unlink( m_temp_video_file_name.c_str() );
	m_upload_progress_listener = 0;
}
void YoutubeUploader::setTitle( const char* title )
{
	m_title = title;
}
void YoutubeUploader::setDescription( const char* description )
{
	m_description = description;
}
void YoutubeUploader::setKeywords( const char* keywords )
{
	m_keywords = keywords;
}

} /* namespace nle */
