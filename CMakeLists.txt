cmake_minimum_required(VERSION 2.8)

option (USE_GUI "GUI" OFF)
option (USE_SERVER "SERVER ONLY" OFF)

#definitions
option (USE_BIG_XRAY "Big Xray Box" OFF)
option (USE_LASER "Laser Box" OFF)
option (USE_VACUUM "Vacuum Box" OFF)

if(NOT USE_BIG_XRAY)
    if(NOT USE_LASER)
        if(NOT USE_VACUUM)
            message ("None")
            SET(USE_BIG_XRAY "" ON) 
        endif(NOT USE_VACUUM)
    endif(NOT USE_LASER)
endif(NOT USE_BIG_XRAY)

if (USE_BIG_XRAY)
    add_definitions(
        -DXRAYBOX
    )  
endif (USE_BIG_XRAY)

if (USE_LASER)  
    add_definitions(
        -DLASERBOX
    )  
endif (USE_LASER)

if (USE_VACUUM)  
     add_definitions(
        -DVACUUMBOX
    )     
endif (USE_VACUUM)

#server
if (USE_SERVER)
    add_subdirectory(Server)
else()
    #client
    add_subdirectory(Client)

    #gui
    if (USE_GUI)  
        find_package(Qt4)
        if (QT4_FOUND)
            add_subdirectory(Gui)
        endif()
    endif (USE_GUI)
endif (USE_SERVER)

