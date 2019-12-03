set(SOURCES
    src/MotorControlServer.cpp
    src/INITIALIZE.cpp
    src/INTERFACE.cpp 
    src/MOTOR.cpp 
    ../include/MySocketTCP.cpp
)

set(HEADERS
    include/INITIALIZE.h
    include/INTERFACE.h 
    include/MOTOR.h 
    ../include/MySocketTCP.h  
    ../include/commonDefs.h
)

#options
if (USE_BIG_XRAY)
    list (APPEND HEADERS 
        include/SLIT.h 
        include/XRAY.h 
    )
    list (APPEND SOURCES 
        src/SLIT.cpp 
        src/XRAY.cpp 
    )
    set(bin_name  "xrayServer")
endif (USE_BIG_XRAY)

if (USE_LASER)
    list (APPEND HEADERS 
        include/FWHEEL.h 
    )
    list (APPEND SOURCES 
        src/FWHEEL.cpp 
    )
    set(bin_name  "laserServer")
endif (USE_LASER)

if (USE_VACUUM)
    list (APPEND HEADERS 
        include/PGAUGE.h 
        include/SLIT.h 
        include/XRAY.h 
    )
    list (APPEND SOURCES 
        src/PGAUGE.cpp 
        src/SLIT.cpp 
        src/XRAY.cpp 
    )
    set(bin_name  "vacuumServer")
endif (USE_VACUUM)

add_definitions(
    -DLOCAL
)

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
