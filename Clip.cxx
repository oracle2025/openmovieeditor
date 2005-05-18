#include "Clip.H"

namespace nle
{

Clip::Clip( Track *track, int64_t position )
{
	m_position = position;
	m_track = track;
	m_trimA = 0;
	m_trimB = 0;
}
void Clip::trimA( int64_t trim )
{
	m_position = m_position - m_trimA;
	m_trimA += trim;
	if ( m_trimA < 0 )
		m_trimA = 0;
	m_position = m_position + m_trimA;
}
void Clip::trimB( int64_t trim )
{
	m_trimB += trim;
	if ( m_trimB < 0 )
		m_trimB = 0;
}

} /* namespace nle */
