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


#Variable to hold the names of the binaries we want to build
set(bin_names)

#Variable to hold the compile time defenitions needed for the build
set(defenitions)

#Append the correct names and defenitions to the list
if (USE_BIG_XRAY)
    list(APPEND bin_names xrayServer)
    list(APPEND defenitions XRAYBOX)
endif (USE_BIG_XRAY)

if (USE_LASER)
    list(APPEND bin_names laserServer)
    list(APPEND defenitions LASERBOX)
endif(USE_LASER)

if (USE_VACUUM)
    list(APPEND bin_names vacuumServer)
    list(APPEND defenitions VACUUMBOX)
endif(USE_VACUUM)

if (USE_GENERIC)
    list(APPEND bin_names motorControlServer)
    list(APPEND defenitions GENERIC)
endif(USE_GENERIC)

if (USE_VIRTUAL)
    list(APPEND bin_names motorControlServer_virtual)
    list(APPEND defenitions VIRTUAL)
endif(USE_VIRTUAL)

#Calculate the length for doing the iteration
list(LENGTH bin_names tmp)
math(EXPR len "${tmp} - 1")

#Iterate over the lists to add targets
foreach(val RANGE ${len})
    list(GET bin_names ${val} exename)
    list(GET defenitions ${val} defname)
    # message("Adding: ${exename} with ${defname} defined")
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
    if (${CMAKE_MAJOR_VERSION}$ GREATER "2")
        target_link_libraries(${exename} PRIVATE project_warnings)
    endif()
    target_compile_definitions(${exename} PRIVATE REMOTE ${defname})
    install(TARGETS ${exename}  DESTINATION bin)
endforeach()


