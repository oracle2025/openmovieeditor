# Copyright (C) 2005 Richard Spindler <richard.spindler AT gmail.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

EXTRA_DIST = style.css tutorial.html
SUBDIRS=images

install-data-local:
	$(mkinstalldirs) $(prefix)/share/doc/openmovieeditor/
	$(INSTALL_DATA) $(srcdir)/tutorial.html $(prefix)/share/doc/openmovieeditor/tutorial.html
	$(INSTALL_DATA) $(srcdir)/style.css $(prefix)/share/doc/openmovieeditor/style.css


