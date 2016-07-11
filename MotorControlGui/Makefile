DESTDIR		?=	../bin
PROG		= $(DESTDIR)/MotorControlGui

TYPE_FLAG 	?=VACUUMBOX
#-DXRAYBOX -DLASERBOX -DVACUUMBOX

all: $(PROG) Makefile.gui

clean: 
	echo 'type flag' + $(TYPE_FLAG)
	if test -e Makefile.gui; then $(MAKE) -f Makefile.gui clean; $(MAKE) -f Makefile.gui mocclean; rm Makefile.gui; else 	$(MAKE) Makefile.gui;$(MAKE) -f Makefile.gui clean; $(MAKE) -f Makefile.gui mocclean;  fi


Makefile.gui: mm

mm:
	echo 'type flag' + $(TYPE_FLAG)
	qmake -set QT_INSTALL_PREFIX $(QTDIR) && qmake -o Makefile.gui DESTDIR=$(DESTDIR)  TYPE_FLAG=$(TYPE_FLAG)


$(PROG): Makefile.gui $(DIR)
	echo 'type flag' + $(TYPE_FLAG)
	echo 'defines' + $(DEFINES)
	$(MAKE) -f Makefile.gui DESTDIR=$(DESTDIR)   TYPE_FLAG=$(TYPE_FLAG)
