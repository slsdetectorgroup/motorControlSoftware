
QT_INSTALL_PREFIX	=	$(QTDIR)
QMAKE_UIC 			= 	$(QTDIR)/bin/uic
QMAKE_MOC 			=  	$(QTDIR)/bin/moc
QMAKE_RCC 			=  	$(QTDIR)/bin/rcc
QMAKE_INCDIR_QT 	= 	$(QTDIR)/include/
QMAKE_LIBS_QT 		= 	-L$(QTDIR)/lib 
QMAKE_LIBS 			= 	-L$(QTDIR)/lib 

CXXFLAGS			+=  -g

QMAKE_CXXFLAGS_WARN_ON = 	-w 
QMAKE_CFLAGS_WARN_ON   = 	-w

/*TEMPLATE     		=  	app*/

RESOURCES   		+=  icons.qrc

DESTDIR  			= 	../bin/
MOC_DIR      		=  	mocs
OBJECTS_DIR 		=  	objs
UI_HEADERS_DIR		= 	forms/include
CONFIG				+=	debug no_include_pwd

target.path 		+= 		$(DESTDIR)
documentation.path 	= 		/$(DOCPATH)
documentation.files = 		docs/*
INSTALLS			+= 		target
INSTALLS 			+= 		documentation
QMAKE_CLEAN 		+= 		docs/*/* 


TYPE_FLAG			?=   LASERBOX
DEFINES 			+= 	 $(TYPE_FLAG)


DEPENDPATH  +=  ../MotorControlClient\
                ../MySocketTCP\
                forms/include\
                ../
                
                
INCLUDEPATH +=  ../MotorControlClient\
                ../MotorControlServer\
                ../MySocketTCP\
                /usr/include/qt4\	
				/usr/include/Qt\
				/usr/include/QtCore\
				/usr/include/QtGui\
				forms/include\
                ../      

             

HEADERS      += MotorControlGui.h \
                MotorWidget.h \
                OptionsWidget.h \
                MotorControlClient.h \
                MySocketTCP.h\
                commonDefs.h
                


SOURCES      += gui_main.cpp \
                MotorControlGui.cpp\
                MotorWidget.cpp\
                OptionsWidget.cpp \
                MotorControlClient.cpp \
                MySocketTCP.cpp
                
               

FORMS        += forms/form_motorwidget.ui\
                forms/form_optionsgui.ui


contains(TYPE_FLAG, LASERBOX){
HEADERS		+= FwheelWidget.h
SOURCES		+= FwheelWidget.cpp
FORMS       += forms/form_fwheelwidget.ui  \
			   forms/form_laserboxgui.ui
}
contains(TYPE_FLAG, VACUUMBOX){
HEADERS		+= PGaugeWidget.h
SOURCES		+= PGaugeWidget.cpp
FORMS       += forms/form_pgaugewidget.ui  \
			   forms/form_vacuumboxgui.ui
}
else{
FORMS       += forms/form_motorcontrolgui.ui
}

# install
# target.path = trial
# sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS trial.pro
# sources.path =.
# INSTALLS += target sources

# symbian {
#     TARGET.UID3 = 0xA000A64F
#     include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
# }
