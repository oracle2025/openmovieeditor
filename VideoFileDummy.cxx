#include "VideoFileDummy.H"
#include <fstream>
#include <iostream>
#include <stdlib.h>
using namespace std;

namespace nle
{

VideoFileDummy::VideoFileDummy( const char* dir )
{
	ifstream f;
	string tmp;
	string list_filename = dir;
	list_filename += "/list";
	m_directory = dir;
	
	f.open( list_filename.c_str(), ios::in );
	f >> tmp;
	m_length = atoi(tmp.c_str());
	cout << "Length: " << m_length << endl;
	while ( !f.eof() ) {
		f >> tmp;
		m_files.push_back(tmp);
	}
	m_offset = 0;

	f.close();

	tmp = dir;
	tmp += '/';
	tmp += m_files[0];
	ifstream f2;
	f2.open( tmp.c_str(), ios::in|ios::binary );
	char buffer[256];
	f2.getline(buffer, sizeof(buffer));
	f2.getline(buffer, sizeof(buffer));
	f2.close();
	tmp = buffer;
	int i = tmp.find( " " );
	m_width = atoi( tmp.substr( 0, i ).c_str() );
	m_height = atoi( tmp.substr( i + 1, tmp.length() - i - 1 ).c_str() );
	cout << "Width: " << m_width << endl;
	cout << "Height: " << m_height << endl;
	m_buffer = new unsigned char[m_width * m_height * 3];
}
int64_t VideoFileDummy::length()
{
	return m_length;
}
void VideoFileDummy::size( int &width, int &height )
{
	width = m_width;
	height = m_height;
}
int VideoFileDummy::fps()
{
	return 25;
}
unsigned char* VideoFileDummy::read()
{
	char buf[255];
	string tmp = m_directory;
	tmp += '/';
	tmp += m_files[m_offset];
	ifstream f;
	f.open( tmp.c_str(), ios::in|ios::binary );
	f.getline( buf, sizeof(buf) );
	f.getline( buf, sizeof(buf) );
	f.getline( buf, sizeof(buf) );
	f.read( (char*)m_buffer, m_width * m_height * 3 );
	m_offset++;
	if ( m_offset >= m_length )
		m_offset = 0;
	return m_buffer;
}
void VideoFileDummy::seek( int64_t frame )
{
	m_offset = frame;
	if ( m_offset >= m_length )
		m_offset = 0;
}

} /* namespace nle */
