set(SOURCES
    src/main.cpp
    src/Client.cpp
    ../include/MySocketTCP.cpp
)

set(HEADERS
    include/Client.h
    ../include/MySocketTCP.h
    ../include/commonDefs.h
)

#options
if (USE_BIG_XRAY)
    set(bin_name  "xrayClient_sl7")
endif (USE_BIG_XRAY)
if (USE_LASER)
    set(bin_name  "laserClient_sl7")
endif (USE_LASER)
if (USE_VACUUM)
    set(bin_name  "vacuumClient_sl7")
endif (USE_VACUUM)

add_definitions(
    -DREMOTE
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
