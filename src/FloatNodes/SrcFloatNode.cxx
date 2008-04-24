
#include "SrcFloatNode.H"

SrcFloatNode::SrcFloatNode( nle::FloatNodeFilter* node_filter )
{
	m_node_filter = node_filter;
}
float* SrcFloatNode::getFrame( int output, double position )
{
	return m_node_filter->m_frame_cache;
}
