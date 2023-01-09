set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 5.9 COMPONENTS Widgets Core Gui REQUIRED)

set(SOURCES
    src/gui_main.cpp
    src/Gui.cpp
    src/MotorWidget.cpp
    src/ReferencePointsWidget.cpp
    src/FluorWidget.cpp
    src/SlitWidget.cpp
    src/FwheelWidget.cpp 
    src/PGaugeWidget.cpp 
    src/TubeWidget.cpp 
    src/OptionsWidget.cpp
    ../Client/src/Client.cpp
    ../include/MySocketTCP.cpp
)

set(FORMS
    forms/form_gui.ui
    forms/form_motorwidget.ui
    forms/form_referencepointswidget.ui
    forms/form_fluorwidget.ui
    forms/form_slitwidget.ui
    forms/form_fwheelwidget.ui 
    forms/form_pgaugewidget.ui 
    forms/form_tubewidget.ui 
    forms/form_optionswidget.ui
)

qt5_wrap_ui(FORMS_H ${FORMS})


set(HEADERS
    include/GuiDefs.h
    include/Gui.h
    include/MotorWidget.h
    include/ReferencePointsWidget.h
    include/FluorWidget.h
    include/SlitWidget.h
    include/FwheelWidget.h 
    include/PGaugeWidget.h 
    include/TubeWidget.h 
    include/OptionsWidget.h
    ../Client/include/Client.h
    ../include/MySocketTCP.h
    ../include/commonDefs.h
    ../include/logger.h
)

#Variable to hold the names of the binaries we want to build
set(bin_names)

#Variable to hold the compile time definitions needed for the build
set(definitions)
list(APPEND definitions ${Qt5Widgets_DEFINITIONS})

#Append the correct names and definitions to the list
if (USE_BIG_XRAY)
    list(APPEND bin_names XRayBoxGui)
    list(APPEND definitions XRAYBOX)
endif (USE_BIG_XRAY)

if (USE_LASER)
    list(APPEND bin_names LaserBoxGui)
    list(APPEND definitions LASERBOX)
endif(USE_LASER)

if (USE_VACUUM)
    list(APPEND bin_names VacuumBoxGui)
    list(APPEND definitions VACUUMBOX)
endif(USE_VACUUM)

if (USE_GENERIC)
    list(APPEND bin_names MotorControlGui)
    list(APPEND definitions GENERIC)
endif(USE_GENERIC)

#Calculate the length for doing the iteration
list(LENGTH bin_names tmp)
math(EXPR len "${tmp} - 1")


set(RESOURCES
    include/icons.qrc
)
qt5_add_resources(RESOURCES_SRCS ${RESOURCES})

#Iterate over the lists to add targets
foreach(val RANGE ${len})
    list(GET bin_names ${val} exename)
    #message("Adding Gui: ${exename} with ${definitions} defined")

    add_executable(${exename}
        ${SOURCES}
        ${HEADERS}
        ${FORMS_H}
        ${RESOURCES_SRCS}
    )

    target_include_directories(${exename} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../Client/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_INSTALL_INCLUDEDIR}
        ${QT5Widgets_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(${exename} PUBLIC
        pthread
        rt
        Qt5::Widgets
        Qt5::Gui
        Qt5::Core
    )  
    if (${CMAKE_MAJOR_VERSION}$ GREATER "2")
        target_link_libraries(${exename} PRIVATE project_warnings)
    endif()
    set_target_properties(${exename} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        AUTOMOC ON
    )
    target_compile_definitions(${exename} PRIVATE ${definitions})
    install(TARGETS ${exename}  DESTINATION bin)
endforeach()


