
#include "SinkFloatNode.H"

SinkFloatNode::SinkFloatNode()
{
}
float* SinkFloatNode::getFrame( int output, double position )
{
	if ( node->inputs[0] && node->inputs[0]->node ) {
		return node->inputs[0]->node->getFrame( 0, position );
	}
	return 0;
}
