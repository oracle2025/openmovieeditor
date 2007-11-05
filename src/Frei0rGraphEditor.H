#ifndef _FREI0R_GRAPH_EDITOR_H_
#define _FREI0R_GRAPH_EDITOR_H_

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <string>
const int MAX_FILTER_OUT = 20;
const int MAX_FILTER_IN = 20;

class INode;
class NodeFilterFrei0rFactoryPlugin;
class NodeFilterFrei0rFactory;
class Fl_Widget;

typedef struct _filters {
	struct _filters* next;
	int id;
	int x;
	int y;
	int w;
	int h;
	int input_count;
	int output_count;
	struct _filters* inputs[MAX_FILTER_IN];
	struct _filters* outputs[MAX_FILTER_OUT];
	int target_slots[MAX_FILTER_OUT];
	std::string name;
	INode* node;
	Fl_Widget* widgets[20];
} filters;

class Frei0rGraphEditor : public Fl_Widget
{
        public:
                Frei0rGraphEditor( int x, int y, int w, int h, const char *label = 0 );
		~Frei0rGraphEditor();
		void draw();
		int handle( int event );
		INode* getSink() { return m_sink_node; }
		void addNode( NodeFilterFrei0rFactoryPlugin* ffp );
		void writeXml();
		void readXml();
		Fl_Widget* m_trash;
	private:
		filters* m_filters;
		filters* m_current;
		int m_x_drag_offset;
		int m_y_drag_offset;
		int m_input_drag;
		int m_output_drag;
		int m_connection_x;
		int m_connection_y;
		INode* m_sink_node;
		NodeFilterFrei0rFactory* m_factory;

};

#endif /* _FREI0R_GRAPH_EDITOR_H_ */

