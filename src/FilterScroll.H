#include <FL/Fl.H>
#include <FL/Fl_Scroll.H>


namespace nle
{

class FilterClip;
class FilterBase;

class FilterScroll : public Fl_Scroll {
    int nchild;
    int nchildheight;
public:
    FilterScroll(int X, int Y, int W, int H, const char* L=0);
    void resize(int X, int Y, int W, int H);
    void AddItem(FilterBase* filter);
    void setClip( FilterClip* clip );
};

} /* namespace nle */
