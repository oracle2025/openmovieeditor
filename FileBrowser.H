#ifndef _FILE_BROWSER_H_
#define _FILE_BROWSER_H_

#include <string>
#include <FL/Fl_File_Browser.H>

namespace nle
{

class FileBrowser : public Fl_File_Browser
{
	public:
		FileBrowser( int x, int y, int w, int h, const char *l = 0 );
		~FileBrowser() {}
		int handle( int event );
		void load_rel();
	private:
		std::string m_path;
};

} /* namespace nle */
#endif /* _FILE_BROWSER_H_ */
