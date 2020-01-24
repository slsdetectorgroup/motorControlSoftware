set(CMAKE_AUTOMOC ON)

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

#Variable to hold the compile time defenitions needed for the build
set(defenitions)

#Append the correct names and defenitions to the list
if (USE_BIG_XRAY)
    list(APPEND bin_names XRayBoxGui)
    list(APPEND defenitions XRAYBOX)
endif (USE_BIG_XRAY)

if (USE_LASER)
    list(APPEND bin_names LaserBoxGui)
    list(APPEND defenitions LASERBOX)
endif(USE_LASER)

if (USE_VACUUM)
    list(APPEND bin_names VacuumBoxGui)
    list(APPEND defenitions VACUUMBOX)
endif(USE_VACUUM)

#Calculate the length for doing the iteration
list(LENGTH bin_names tmp)
math(EXPR len "${tmp} - 1")

qt4_wrap_ui(FORMS_H ${FORMS})

set(RESOURCES
    include/icons.qrc
)
qt4_add_resources(RESOURCES_SRCS ${RESOURCES})

#Iterate over the lists to add targets
foreach(val RANGE ${len})

    list(GET bin_names ${val} exename)
    list(GET defenitions ${val} defname)
    # message("Adding: ${exename} with ${defname} defined")

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
        ${QT_INCLUDE_DIR}
        ${QT_QTCORE_INCLUDE_DIR}
        ${QT_QTGUI_INCLUDE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(${exename} PUBLIC
        ${QT_QTCORE_LIBRARIES}
        ${QT_QTGUI_LIBRARIES}
        pthread
        rt
        z
        Qt4::QtOpenGL
        Qt4::QtSvg
    )  
    set_target_properties(${exename} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    target_compile_definitions(${exename} PRIVATE ${defname})
    install(TARGETS ${exename}  DESTINATION bin)
endforeach()


