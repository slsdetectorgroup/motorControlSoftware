
#Headers and sources are the same for all boxes
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


#Variable to hold the names of the binaries we want to build
set(bin_names)

#Variable to hold the compile time definitions needed for the build
set(definitions)

#Append the correct names and definitions to the list
if (USE_BIG_XRAY)
    list(APPEND bin_names xrayClient)
    list(APPEND definitions XRAYBOX)
endif (USE_BIG_XRAY)

if (USE_LASER)
    list(APPEND bin_names laserClient)
    list(APPEND definitions LASERBOX)
endif(USE_LASER)

if (USE_VACUUM)
    list(APPEND bin_names vacuumClient)
    list(APPEND definitions VACUUMBOX)
endif(USE_VACUUM)

if (USE_GENERIC)
    list(APPEND bin_names motorControlClient)
    list(APPEND definitions GENERIC)
endif(USE_GENERIC)

#Calculate the length for doing the iteration
list(LENGTH bin_names tmp)
math(EXPR len "${tmp} - 1")

#Iterate over the lists to add targets
foreach(val RANGE ${len})
    list(GET bin_names ${val} exename)
    #message("Adding Client: ${exename} with ${definitions} defined")
    add_executable(${exename}
        ${SOURCES}
        ${HEADERS}
    )
    target_include_directories(${exename} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
    )
    set_target_properties(${exename} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    target_link_libraries(${exename} PRIVATE project_warnings)
    target_compile_definitions(${exename} PRIVATE REMOTE ${definitions})
    install(TARGETS ${exename}  DESTINATION bin)
endforeach()

