set(SOURCES
    src/Server.cpp
    src/Initialize.cpp
    src/Interface.cpp 
    src/Xray.cpp
    src/Pgauge.cpp
    src/Fwheel.cpp
    src/ReferencePoint.cpp
    src/ReferencePoints.cpp
    src/Fluorescence.cpp
    src/TargetHolder.cpp
    src/Slit.cpp
    src/Controller.cpp
    src/Motor.cpp 
    ../include/MySocketTCP.cpp
)

set(HEADERS
    include/Initialize.h
    include/Interface.h 
    include/Xray.h
    include/Pgauge.h
    include/Fwheel.h
    include/ReferencePoint.h
    include/ReferencePoints.h
    include/Fluorescence.h
    include/TargetHolder.h
    include/Slit.h
    include/Controller.h
    include/Motor.h 
    ../include/MySocketTCP.h  
    ../include/commonDefs.h
    ../include/ansi.h
    ../include/logger.h
)

#options
if (USE_BIG_XRAY)
    set(bin_name  "xrayServer")
endif (USE_BIG_XRAY)

if (USE_LASER)
    set(bin_name  "laserServer")
endif (USE_LASER)

if (USE_VACUUM)
    set(bin_name  "vacuumServer")
endif (USE_VACUUM)

add_executable(${bin_name}
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(${bin_name} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

set_target_properties(${bin_name} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

install(TARGETS ${bin_name} DESTINATION bin)
