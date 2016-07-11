DESTDIR = bin
PROG= $(DESTDIR)/MotorControlGui

TYPE_FLAG ?=-DXRAYBOX
#-DXRAYBOX -DLASERBOX -DVACUUMBOX

all: $(PROG) Makefile.gui

clean: 
	if test -e Makefile.gui; then $(MAKE) -f Makefile.gui clean; $(MAKE) -f Makefile.gui mocclean; rm Makefile.gui; else 	$(MAKE) Makefile.gui;$(MAKE) -f Makefile.gui clean; $(MAKE) -f Makefile.gui mocclean;  fi


Makefile.gui: mm

mm:
	qmake -set QT_INSTALL_PREFIX $(QTDIR) && qmake -o Makefile.gui DESTDIR=$(DESTDIR)  TYPE_FLAG=$(TYPE_FLAG)


$(PROG): Makefile.gui $(DIR)
	$(MAKE) -f Makefile.gui DESTDIR=$(DESTDIR)   TYPE_FLAG=$(TYPE_FLAG)
