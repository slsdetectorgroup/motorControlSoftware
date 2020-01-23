cmake_minimum_required(VERSION 2.8)
project(MotorControl)
option (USE_GUI "GUI" OFF)
option (USE_SERVER "SERVER" OFF)
option (USE_CLIENT "CLIENT" ON)

#definitions
option (USE_BIG_XRAY "Big Xray Box" OFF)
option (USE_LASER "Laser Box" OFF)
option (USE_VACUUM "Vacuum Box" OFF)

if (USE_CLIENT)
    add_subdirectory(Client)
endif(USE_CLIENT)

if (USE_SERVER)
    add_subdirectory(Server)
endif(USE_SERVER)

if (USE_GUI)  
    find_package(Qt4 REQUIRED)
    add_subdirectory(Gui)
endif (USE_GUI)






