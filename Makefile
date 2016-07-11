WD=$(shell pwd)
GUIDIR = $(WD)/MotorControlGui

#-DXRAYBOX -DLASERBOX -DVACUUMBOX
TYPEFL = -DLASERBOX
TYPE_FLAG = LASERBOX

INCLUDES= -I MotorControlServer -I MotorControlClient -I MySocketTCP -I .
SERVER_FLAGS= -DLOCAL #-DVERY_VERBOSE #-DVERBOSE_MOTOR
CLIENT_FLAGS=-DREMOTE
LOCAL_FLAGS=-DLOCAL


all: clean socketServer socketClient localClient gui#doc

doc: $(SRC_CLNT)
	doxygen doxy.config


socketServer: MotorControlServer/MotorControlServer.cpp MotorControlServer/INITIALIZE.cpp MotorControlServer/INTERFACE.cpp MotorControlServer/MOTOR.cpp MotorControlServer/FWHEEL.cpp MotorControlServer/XRAY.cpp MotorControlServer/SLIT.cpp MySocketTCP/MySocketTCP.cpp
	echo '*** compiling server ***'
	gcc -o bin/socketServer MotorControlServer/MotorControlServer.cpp MotorControlServer/INITIALIZE.cpp MotorControlServer/INTERFACE.cpp MotorControlServer/MOTOR.cpp MotorControlServer/FWHEEL.cpp MotorControlServer/XRAY.cpp MotorControlServer/SLIT.cpp MySocketTCP/MySocketTCP.cpp $(SERVER_FLAGS) $(INCLUDES) $(TYPEFL) -lm -lstdc++


socketClient: MotorControlClient/main.cpp MotorControlClient/MotorControlClient.cpp MySocketTCP/MySocketTCP.cpp
	echo '*** compiling client ***'
	gcc -o bin/socketClient MotorControlClient/main.cpp MotorControlClient/MotorControlClient.cpp MySocketTCP/MySocketTCP.cpp $(CLIENT_FLAGS) $(INCLUDES) $(TYPEFL) -lm -lstdc++


localClient: MotorControlClient/main.cpp MotorControlClient/MotorControlClient.cpp MotorControlServer/INITIALIZE.cpp MotorControlServer/INTERFACE.cpp MotorControlServer/MOTOR.cpp MotorControlServer/FWHEEL.cpp MotorControlServer/XRAY.cpp MotorControlServer/SLIT.cpp MySocketTCP/MySocketTCP.cpp
	echo '*** compiling local client ***'
	gcc -o 	bin/localClient MotorControlClient/main.cpp MotorControlClient/MotorControlClient.cpp MotorControlServer/INITIALIZE.cpp MotorControlServer/INTERFACE.cpp MotorControlServer/MOTOR.cpp MotorControlServer/FWHEEL.cpp MotorControlServer/XRAY.cpp MotorControlServer/SLIT.cpp MySocketTCP/MySocketTCP.cpp $(LOCAL_FLAGS) $(INCLUDES) $(TYPEFL) -lm -lstdc++

gui:
	cd  $(GUIDIR) && $(MAKE) TYPE_FLAG=$(TYPE_FLAG)


clean:
	echo '*** compiling gui ***'
	rm -rf bin/socketServer bin/socketClient bin/localClient *.*~ /MotorControlServer/*.*~ /MotorControlClient/*.*~ *~
	cd $(GUIDIR) &&	rm -rf bin/MotorControlGui mocs/* objs/*

