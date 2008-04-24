#ifndef _I_FLOAT_NODE_H_
#define _I_FLOAT_NODE_H_

#include "Float0rGraphEditor.H"

class IFloatNode
{
	public:
		virtual ~IFloatNode() {}
		virtual int getInputCount() = 0;
		virtual int getOutputCount() = 0;
		virtual void init_widgets() {};
		virtual void delete_widgets() {};
		virtual float* getFrame( int output, double position ) = 0;
		virtual void init() {}
		virtual const char* identifier() = 0;
		virtual void readXML( TiXmlElement* xml_node ) {}
		virtual void writeXML( TiXmlElement* xml_node ) {}
		filters* node;

};


#endif /* _I_FLOAT_NODE_H_ */
