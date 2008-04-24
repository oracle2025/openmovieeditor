#ifndef _FLOAT0R_GRAPH_EDITOR_H_
#define _FLOAT0R_GRAPH_EDITOR_H_

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include "FloatNodeFilter.H"
#include <string>

class IFloatNodeFilterFactoryPlugin;
class IFloatNode;

filters* filters_create( int x, int y, int w, int h, IFloatNode* node, std::string name = "", int id = -1 );

class Float0rGraphEditor : public Fl_Widget
{
        public:
                Float0rGraphEditor( int x, int y, int w, int h, const char *label = 0 );
		~Float0rGraphEditor();
		void draw();
		int handle( int event );
		void addNode( IFloatNodeFilterFactoryPlugin* ffp );
		Fl_Widget* m_trash;
		void setFilter( nle::FloatNodeFilter* filter );
	private:
		nle::FloatNodeFilter* m_filter;
		filters* m_current;
		int m_x_drag_offset;
		int m_y_drag_offset;
		int m_input_drag;
		int m_output_drag;
		int m_connection_x;
		int m_connection_y;
};

#endif /* _FLOAT0R_GRAPH_EDITOR_H_ */

