set(SOURCES
    src/Server.cpp
    src/Initialize.cpp
    src/Interface.cpp 
    src/Motor.cpp 
    ../include/MySocketTCP.cpp
)

set(HEADERS
    include/Initialize.h
    include/Interface.h 
    include/Motor.h 
    ../include/MySocketTCP.h  
    ../include/commonDefs.h
)

#options
if (USE_BIG_XRAY)
    list (APPEND HEADERS 
        include/Slit.h 
        include/Xray.h 
    )
    list (APPEND SOURCES 
        src/Slit.cpp 
        src/Xray.cpp 
    )
    set(bin_name  "xrayServer")
endif (USE_BIG_XRAY)

if (USE_LASER)
    list (APPEND HEADERS 
        include/Fwheel.h 
    )
    list (APPEND SOURCES 
        src/Fwheel.cpp 
    )
    set(bin_name  "laserServer")
endif (USE_LASER)

if (USE_VACUUM)
    list (APPEND HEADERS 
        include/Pgauge.h 
        include/Slit.h 
        include/Xray.h 
    )
    list (APPEND SOURCES 
        src/Pgauge.cpp 
        src/Slit.cpp 
        src/Xray.cpp 
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
