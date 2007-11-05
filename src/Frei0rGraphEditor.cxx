#include "Frei0rGraphEditor.H"
#include "NodeFilterFrei0rFactory.H"
#include "NodeFilterFrei0rFactoryPlugin.H"
#include "Frei0rNode.H"
//#include "ImageNode.H"
#include "SinkNode.H"
#include <FL/fl_draw.H>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include "sl.h"
#include <cassert>
#include <iostream>
#include <stdlib.h>
#include <tinyxml.h>
using namespace std;

const int FILTER_IO_SIZE = 16;
const int FILTER_IO_MIDDLE = ( FILTER_IO_SIZE / 2 );
const int FILTER_IO_SPACING = ( FILTER_IO_SIZE + 5 );

static filters* filters_create( int x, int y, int w, int h, INode* node, string name = "", int id = -1 ) {
	static int id_src = 0;
	filters* p;
	p = new filters;
	p->next = 0;
	p->x = x;
	p->y = y;
	p->w = w;
	p->h = h;
	p->input_count = node->getInputCount();
	p->output_count = node->getOutputCount();
	p->name = name;
	p->node = node;
	node->node = p;
	p->widgets[0] = 0;
	node->init();
	if ( id < 0 ) {
		p->id = id_src++;
	} else {
		p->id = id;
		if ( id >= id_src ) {
			id_src = id + 1;
		}
	}
	if ( p->widgets[0] ) {
		p->h = 0;
	}
	for ( int i = 0; p->widgets[i]; i++ ) {
		p->h += 20;
	}
	for ( int i = 0; i < MAX_FILTER_IN; i++ ) {
		p->inputs[i] = 0;
	}
	for ( int i = 0; i < MAX_FILTER_OUT; i++ ) {
		p->outputs[i] = 0;
		p->target_slots[i] = 0;
	}
	return p;
}

extern NodeFilterFrei0rFactory* g_node_filter_frei0r_factory;

Frei0rGraphEditor::Frei0rGraphEditor( int x, int y, int w, int h, const char *label )
	  : Fl_Widget( x, y, w, h, label )
{
	m_factory = g_node_filter_frei0r_factory;
	m_trash = 0;
	m_input_drag = -1;
	m_output_drag = -1;
	m_current = 0;
	m_filters = 0;
/*	m_filters = (filters*)sl_push( m_filters, filters_create( 10,10, 50, 50, new ImageNode("/home/oracle/src/frei0r_compositor/image.png"), "Source" ) );
	m_filters = (filters*)sl_push( m_filters, filters_create( 10,100, 50, 50, new ImageNode("/home/oracle/src/frei0r_compositor/image2.png"), "Source" ) );
	m_filters = (filters*)sl_push( m_filters, filters_create( 100,100, 50, 50, new ImageNode("/home/oracle/src/frei0r_compositor/greenscreen.png"), "Source" ) );
	m_filters = (filters*)sl_push( m_filters, filters_create( 100,100, 50,
	50, new ImageNode("/home/oracle/src/frei0r_compositor/clockwork.png"),
	"Source" ) );*/
	m_filters = (filters*)sl_push( m_filters, filters_create( 10,200, 50, 50, new SinkNode(), "Output" ) );
	filters* C = m_filters;
	m_sink_node = C->node;
}
Frei0rGraphEditor::~Frei0rGraphEditor()
{
}
void Frei0rGraphEditor::draw()
{
	fl_draw_box( FL_DOWN_BOX, x(), y(), w(), h(), FL_BACKGROUND_COLOR );
	fl_push_clip( x() + 2, y() + 2,  w() - 4, h() - 4 );
	fl_line_style( FL_SOLID, 1 );
	for ( filters* i = m_filters; i; i = i->next ) {
		if ( m_current && i == m_current ) {
			if ( m_output_drag >= 0 ) {
				fl_color( FL_FOREGROUND_COLOR );
				fl_line( m_connection_x, m_connection_y, x() + i->x + ( m_output_drag * FILTER_IO_SPACING ) + FILTER_IO_MIDDLE, y() + i->y + i->h + FILTER_IO_MIDDLE );
				fl_draw_box( FL_UP_BOX, x() + i->x, y() + i->y, i->w, i->h, FL_BACKGROUND_COLOR );
			} else if ( m_input_drag >= 0 ) {
				fl_color( FL_FOREGROUND_COLOR );
				fl_line( m_connection_x, m_connection_y, x() + i->x + ( m_input_drag * FILTER_IO_SPACING ) + FILTER_IO_MIDDLE, y() + i->y - FILTER_IO_MIDDLE );
				fl_draw_box( FL_UP_BOX, x() + i->x, y() + i->y, i->w, i->h, FL_BACKGROUND_COLOR );
			} else {
				fl_draw_box( FL_DOWN_BOX, x() + i->x, y() + i->y, i->w, i->h, FL_BACKGROUND_COLOR );
			}
		} else {
			fl_draw_box( FL_UP_BOX, x() + i->x, y() + i->y, i->w, i->h, FL_BACKGROUND_COLOR );
		}
		for ( int j = 0; j < i->input_count; j++ ) {
			fl_draw_box( FL_UP_BOX, x() + i->x + ( j * FILTER_IO_SPACING ), y() + i->y - FILTER_IO_SIZE, FILTER_IO_SIZE, FILTER_IO_SIZE, FL_GREEN );
			fl_color( FL_RED );
			if ( i->inputs[j] ) {
				filters* k = i->inputs[j];
				fl_line( x() + i->x + ( j * FILTER_IO_SPACING ) + FILTER_IO_MIDDLE,
						y() + i->y - FILTER_IO_MIDDLE,
						x() + k->x + FILTER_IO_MIDDLE,
						y() + k->y + k->h + FILTER_IO_MIDDLE );
			}
		}
		for ( int j = 0; j < i->output_count; j++ ) {
			fl_draw_box( FL_UP_BOX, x() + i->x + ( j * FILTER_IO_SPACING ), y() + i->y + i->h, FILTER_IO_SIZE, FILTER_IO_SIZE, FL_RED );
		}
		fl_font( FL_HELVETICA, 12 );
		fl_color( FL_FOREGROUND_COLOR );
		int text_w, text_h;
		fl_measure( i->name.c_str(), text_w, text_h );
		fl_draw( i->name.c_str(), x() + i->x + 3, y() + i->y + 3 + text_h );
		for ( int k = 0; i->widgets[k]; k++ ) {
			i->widgets[k]->resize(x()+i->x+i->w,y()+i->y+k*20,200,20);
		}

	}

	fl_pop_clip();
}
static bool inside_filter( filters* f, int x, int y )
{
	return ( f->x < x && f->x + f->w > x && f->y < y && f->y + f->h > y );
}
static int inside_filter_input( filters* f, int x, int y )
{
	for ( int i = 0; i < f->input_count; i++ ) {
		int px1, py1, px2, py2;
		px1 = f->x + ( i * FILTER_IO_SPACING );
		py1 = f->y - FILTER_IO_SIZE;
		px2 = px1 + FILTER_IO_SIZE;
		py2 = py1 + FILTER_IO_SIZE;
		if ( x > px1 && x < px2 && y > py1 && y < py2 ) {
			return i;
		}
	}
	return -1;
}
static int inside_filter_output( filters* f, int x, int y )
{
	for ( int i = 0; i < f->output_count; i++ ) {
		int px1, py1, px2, py2;
		px1 = f->x + ( i * FILTER_IO_SPACING );
		py1 = f->y + f->h;
		px2 = px1 + FILTER_IO_SIZE;
		py2 = py1 + FILTER_IO_SIZE;
		if ( x > px1 && x < px2 && y > py1 && y < py2 ) {
			return i;
		}
	}
	return -1;
}
int Frei0rGraphEditor::handle( int event )
{
	switch ( event ) {
		case FL_PUSH:
			for ( filters* i = m_filters; i; i = i->next ) {
				if ( inside_filter( i, Fl::event_x() - x(), Fl::event_y() - y() ) ) {
					m_current = i;
					m_x_drag_offset = Fl::event_x() - x() - i->x;
					m_y_drag_offset = Fl::event_y() - y() - i->y;
					return 1;
				} else if ( ( m_output_drag = inside_filter_output( i, Fl::event_x() - x(), Fl::event_y() - y() ) ) >= 0 ) {
					m_current = i;
					m_connection_x = Fl::event_x();
					m_connection_y = Fl::event_y();
					return 1;
				} else if ( ( m_input_drag = inside_filter_input( i, Fl::event_x() - x(), Fl::event_y() - y() ) ) >= 0 ) {
					m_current = i;
					m_connection_x = Fl::event_x();
					m_connection_y = Fl::event_y();
					return 1;
				}
			}
			break;
		case FL_DRAG:
			if ( m_current && m_output_drag < 0 && m_input_drag < 0 ) {
				m_current->x = Fl::event_x() - x() - m_x_drag_offset;
				m_current->y = Fl::event_y() - y() - m_y_drag_offset;
				window()->redraw();
				return 1;
			} else if ( m_current && m_output_drag >= 0 ) {
				m_connection_x = Fl::event_x();
				m_connection_y = Fl::event_y();
				window()->redraw();
				return 1;
			} else if ( m_current && m_input_drag >= 0 ) {
				m_connection_x = Fl::event_x();
				m_connection_y = Fl::event_y();
				window()->redraw();
				return 1;
			}
			break;
		case FL_RELEASE:
			if ( m_current && m_output_drag < 0 && m_input_drag < 0 ) {
				if ( m_trash && Fl::event_x() > m_trash->x() && Fl::event_x() < m_trash->x()+m_trash->w() && Fl::event_y() > m_trash->y() && Fl::event_y() < m_trash->y()+m_trash->h() ) {
					if ( m_current->node == m_sink_node ) {
						m_current->x -= 100;
					} else {

						for ( int j = 0; j < MAX_FILTER_OUT; j++ ) {
							if ( m_current->outputs[j] ) {
								m_current->outputs[j]->inputs[m_current->target_slots[j]] = 0;
								m_current->outputs[j] = 0;
								m_current->target_slots[j] = 0;
							}
						}
						for ( int j = 0; j < m_current->input_count; j++ ) {
							if ( m_current->inputs[j] ) {
								for ( int k = 0; k < MAX_FILTER_OUT; k++ ) {
									if ( m_current->inputs[j]->outputs[k] == m_current ) {
										m_current->inputs[j]->outputs[k] = 0;
										m_current->inputs[j]->target_slots[k] = 0;
									}
								}
								m_current->inputs[j] = 0;
							}
						}

						if ( m_current == m_filters ) {
							m_filters = m_current->next;
						} else {
							filters* filter_node = m_filters;
							while ( filter_node->next !=  m_current ) {
								filter_node = filter_node->next;
							}
							filter_node->next = m_current->next;
						}
						delete m_current->node;
						delete m_current;
					}
					m_current = 0;
				} else {
					m_current->x = Fl::event_x() - x() - m_x_drag_offset;
					m_current->y = Fl::event_y() - y() - m_y_drag_offset;
					m_current = 0;
				}
				window()->redraw();
				do_callback();
				return 1;
			} else if ( m_current && m_output_drag >= 0 ) {
				int target_input = -1;
				for ( filters* i = m_filters; i; i = i->next ) {
					if ( i == m_current ) {
						continue;
					}
					if ( ( target_input = inside_filter_input( i, Fl::event_x() - x(), Fl::event_y() - y() ) ) >= 0 ) {
						int output_slot = 0;
						while ( m_current->outputs[output_slot] && output_slot < MAX_FILTER_OUT - 1 ) {
							output_slot++;
						}
						filters* old_unrelated_src = i->inputs[target_input];
						if ( old_unrelated_src ) {
							for ( int j = 0; j < MAX_FILTER_OUT; j++ ) {
								if ( old_unrelated_src->outputs[j] == i && old_unrelated_src->target_slots[j] == target_input ) {
									old_unrelated_src->outputs[j] = 0;
									old_unrelated_src->target_slots[j] = 0;
									break;
								}
							}
						}
						i->inputs[target_input] = m_current;
						m_current->outputs[output_slot] = i;
						m_current->target_slots[output_slot] = target_input;
						break;
					}
				}
				m_current = 0;
				m_output_drag = -1;
				do_callback();
				window()->redraw();
				return 1;
			} else if ( m_current && m_input_drag >= 0 ) {
				m_current = 0;
				m_input_drag = -1;
				do_callback();
				window()->redraw();
				return 1;
			}
			break;
	}
	return Fl_Widget::handle( event );
}
void Frei0rGraphEditor::addNode( NodeFilterFrei0rFactoryPlugin* ffp )
{
	window()->make_current();
	window()->begin();
	m_filters = (filters*)sl_push( m_filters, filters_create( 10,10, 50, 50, ffp->get( 640, 480 ), ffp->name() ) );
	window()->end();
	redraw();
}

void Frei0rGraphEditor::writeXml()
{
	string filename = getenv( "HOME" );
	filename += "/.frei0r_compositor_layout";
	TiXmlDocument doc( filename.c_str() );
	TiXmlDeclaration* dec = new TiXmlDeclaration( "1.0", "", "no" );
	doc.LinkEndChild( dec );

	TiXmlElement* project = new TiXmlElement( "frei0r_compositor_project" );
	doc.LinkEndChild( project );

	TiXmlElement* xml_filter;
	for ( filters* filter_node = m_filters; filter_node; filter_node = filter_node->next ) {
		xml_filter = new TiXmlElement( "filter" );
		project->LinkEndChild( xml_filter );
		xml_filter->SetAttribute( "x", filter_node->x );
		xml_filter->SetAttribute( "y", filter_node->y );
		xml_filter->SetAttribute( "w", filter_node->w );
		xml_filter->SetAttribute( "h", filter_node->h );
		xml_filter->SetAttribute( "name", filter_node->name.c_str() );
		xml_filter->SetAttribute( "id", filter_node->id );
		Frei0rNode* frei0r_node = dynamic_cast<Frei0rNode*>(filter_node->node);
		if ( frei0r_node ) {
			string identifier( "effect:frei0r:" );
			identifier += filter_node->name;
			xml_filter->SetAttribute( "identifier", identifier.c_str() );
		}
		xml_filter->SetAttribute( "id", filter_node->id );
		for ( int i = 0; i < filter_node->input_count; i++ ) {
			if ( filter_node->inputs[i] ) {
				TiXmlElement* xml_input = new TiXmlElement( "input" );
				xml_filter->LinkEndChild( xml_input );
				xml_input->SetAttribute( "nr", i );
				xml_input->SetAttribute( "input_id", filter_node->inputs[i]->id );
			}
		}
		for ( int i = 0; i < filter_node->output_count; i++ ) {
			if ( filter_node->outputs[i] ) {
				TiXmlElement* xml_output = new TiXmlElement( "output" );
				xml_filter->LinkEndChild( xml_output );
				xml_output->SetAttribute( "nr", i );
				xml_output->SetAttribute( "output_id", filter_node->outputs[i]->id );
			}
		}
	}

	doc.SaveFile();

}
void Frei0rGraphEditor::readXml()
{
	string filename = getenv( "HOME" );
	filename += "/.frei0r_compositor_layout";
	TiXmlDocument doc( filename.c_str() );
	if ( !doc.LoadFile() ) {
		return;
	}
	TiXmlHandle docH( &doc );

	TiXmlElement* xml_filter = docH.FirstChild( "frei0r_compositor_project" ).FirstChild( "filter" ).Element();
	for ( ; xml_filter; xml_filter = xml_filter->NextSiblingElement( "filter" ) ) {
		int id;
		if ( xml_filter->Attribute( "id", &id ) ) {
			filters* filter_node = 0;
			for ( filter_node = m_filters; filter_node; filter_node = filter_node->next ) {
				if ( filter_node->id == id ) {
					int x, y, w, h;
					if ( xml_filter->Attribute( "x", &x ) ) {
						filter_node->x = x;
					}
					if ( xml_filter->Attribute( "y", &y ) ) {
						filter_node->y = y;
					}
					if ( xml_filter->Attribute( "w", &w ) ) {
						filter_node->w = w;
					}
					if ( xml_filter->Attribute( "h", &h ) ) {
						filter_node->h = h;
					}
					break;
				}
			}
			if ( !filter_node ) {
				//Create from Frei0r
				const char* identifier = xml_filter->Attribute( "identifier" );
				if ( identifier && m_factory ) {
					identifier += strlen("effect:frei0r:");
					NodeFilterFrei0rFactoryPlugin* ffp = m_factory->get( identifier );
						if ( ffp ) {
							int x = 10;
							int y = 10;
							int w = 50;
							int h = 50;
							int id = -1;
							xml_filter->Attribute( "x", &x );
							xml_filter->Attribute( "y", &y );
							xml_filter->Attribute( "w", &w );
							xml_filter->Attribute( "h", &h );
							xml_filter->Attribute( "id", &id );

							window()->make_current();
							window()->begin();
							m_filters = (filters*)sl_push( m_filters, filters_create( x, y, w, h, ffp->get( 640, 480 ), ffp->name(), id ) );
							window()->end();
							redraw();

					}
				}
			}
		}
	}

	xml_filter = docH.FirstChild( "frei0r_compositor_project" ).FirstChild( "filter" ).Element();
	for ( ; xml_filter; xml_filter = xml_filter->NextSiblingElement( "filter" ) ) {
		int id;
		if ( xml_filter->Attribute( "id", &id ) ) {
			filters* filter_node = 0;
			for ( filter_node = m_filters; filter_node; filter_node = filter_node->next ) {
				if ( filter_node->id == id ) {
					TiXmlElement* xml_input =  TiXmlHandle( xml_filter ).FirstChildElement( "input" ).Element();
					for ( ; xml_input; xml_input = xml_input->NextSiblingElement( "input" ) ) {
						int nr, input_id;
						if ( xml_input->Attribute( "nr", &nr ) && xml_input->Attribute( "input_id", &input_id ) ) {
							filters* src_node = 0;
							for ( src_node = m_filters; src_node; src_node = src_node->next ) {
								if ( src_node->id == input_id ) {
									int output_slot = 0;
									while ( src_node->outputs[output_slot] && output_slot < MAX_FILTER_OUT - 1 ) {
										output_slot++;
									}
									filter_node->inputs[nr] = src_node;
									src_node->outputs[output_slot] = filter_node;
									src_node->target_slots[output_slot] = nr;
									break;
								}
							}
						}
					}
					break;
				}
			}
		}
	}

}

