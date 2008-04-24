
#ifndef _SRC_FLOAT_NODE_H_
#define _SRC_FLOAT_NODE_H_

#include "IFloatNode.H"

class nle::FloatNodeFilter;
class SrcFloatNode : public IFloatNode
{
	public:
		SrcFloatNode( nle::FloatNodeFilter* node_filter );
		~SrcFloatNode() {}
		int getInputCount() { return 0; }
		int getOutputCount() { return 1; }
		float* getFrame( int output, double position );
		const char* identifier() { return "effect:builtin:SrcFloatNode"; }
	private:
		nle::FloatNodeFilter* m_node_filter;
};



#endif /* _SRC_FLOAT_NODE_H_ */
