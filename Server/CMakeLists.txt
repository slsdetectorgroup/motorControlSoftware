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
    ../include/sls/DataSocket.cpp
    ../include/sls/ServerSocket.cpp
    ../include/sls/network_utils.cpp
    ../include/sls/string_utils.cpp
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
    ../include/sls/DataSocket.h
    ../include/sls/ServerSocket.h
    ../include/sls/network_utils.h
    ../include/sls/string_utils.h
    ../include/sls/container_utils.h
    ../include/sls/TypeTraits.h 
    ../include/commonDefs.h
    ../include/ansi.h
    ../include/logger.h
)


#Variable to hold the names of the binaries we want to build
set(bin_names)

#Variable to hold the compile time definitions needed for the build
set(definitions)

#Append the correct names and definitions to the list
if (USE_BIG_XRAY)
    list(APPEND definitions XRAYBOX)
    if (USE_VIRTUAL)
        list(APPEND definitions VIRTUAL)
        list(APPEND bin_names xrayServer_virtual)
    else()
        list(APPEND bin_names xrayServer)  
    endif(USE_VIRTUAL)
elseif (USE_LASER)
    list(APPEND definitions LASERBOX)
    if (USE_VIRTUAL)
        list(APPEND definitions VIRTUAL)
        list(APPEND bin_names laserServer_virtual)
    else()
        list(APPEND bin_names laserServer)  
    endif(USE_VIRTUAL)
elseif (USE_VACUUM)
    list(APPEND definitions VACUUMBOX)
    if (USE_VIRTUAL)
        list(APPEND definitions VIRTUAL)
        list(APPEND bin_names vacuumServer_virtual)
    else()
        list(APPEND bin_names vacuumServer)  
    endif(USE_VIRTUAL)
#generic
else()
    list(APPEND definitions GENERIC)
    if (USE_VIRTUAL)
        list(APPEND definitions VIRTUAL)
        list(APPEND bin_names motorControlServer_virtual)
    else()
        list(APPEND bin_names motorControlServer)  
    endif(USE_VIRTUAL)
endif()

#Calculate the length for doing the iteration
list(LENGTH bin_names tmp)
math(EXPR len "${tmp} - 1")

#Iterate over the lists to add targets
foreach(val RANGE ${len})
    list(GET bin_names ${val} exename)
    #message("Adding Server: ${exename} with ${definitions} defined")
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


