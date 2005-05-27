
#include "FilmStrip.H"
#include "IVideoFile.H"

namespace nle
{

#define PIC_WIDTH 40 
#define PIC_HEIGHT 30

FilmStrip::FilmStrip( IVideoFile* vfile )
{
	unsigned char **rows  = new (unsigned char*)[PIC_HEIGHT];
	m_count = vfile->length() / 100;
	m_pics = new pic_struct[m_count];
	for ( int i = 0; i < m_count; i++ ) {
		vfile->seek( i * 100 );
		m_pics[i].data = new unsigned char[PIC_WIDTH * PIC_HEIGHT * 3];
		m_pics[i].w = PIC_WIDTH;
		m_pics[i].h = PIC_HEIGHT;
		for ( int j = 0; j < PIC_HEIGHT; j++ ) {
			rows[j] = m_pics[i].data + PIC_WIDTH * 3 * j;
		}
		vfile->read( rows, PIC_WIDTH, PIC_HEIGHT );
	}
}
FilmStrip::~FilmStrip()
{
	for ( int i = 0; i < m_count; i++ ) {
		delete [] m_pics[i].data;
	}
	delete [] m_pics;
}


} /* namespace nle */
