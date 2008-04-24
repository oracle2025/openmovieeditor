
#ifndef _SINK_FLOAT_NODE_H_
#define _SINK_FLOAT_NODE_H_

#include "IFloatNode.H"

class SinkFloatNode : public IFloatNode
{
	public:
		SinkFloatNode();
		~SinkFloatNode() {}
		int getInputCount() { return 1; }
		int getOutputCount() { return 0; }
		float* getFrame( int output, double position );
		const char* identifier() { return "effect:builtin:SinkFloatNode"; }
};



#endif /* _SINK_FLOAT_NODE_H_ */
