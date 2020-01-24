
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

#Variable to hold the compile time defenitions needed for the build
set(defenitions)

#Append the correct names and defenitions to the list
if (USE_BIG_XRAY)
    list(APPEND bin_names xrayClient)
    list(APPEND defenitions XRAYBOX)
endif (USE_BIG_XRAY)

if (USE_LASER)
    list(APPEND bin_names laserClient)
    list(APPEND defenitions LASERBOX)
endif(USE_LASER)

if (USE_VACUUM)
    list(APPEND bin_names vacuumClient)
    list(APPEND defenitions VACUUMBOX)
endif(USE_VACUUM)


#Calculate the length for doing the iteration
list(LENGTH bin_names tmp)
math(EXPR len "${tmp} - 1")

#Iterate over the lists to add targets
foreach(val RANGE ${len})
    list(GET bin_names ${val} exename)
    list(GET defenitions ${val} defname)
    message("Adding: ${exename} with ${defname} defined")


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
    target_compile_definitions(${exename} PRIVATE REMOTE ${defname})
    install(TARGETS ${exename}  DESTINATION bin)
endforeach()

