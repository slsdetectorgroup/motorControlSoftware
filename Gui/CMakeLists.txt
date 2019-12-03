set(CMAKE_AUTOMOC ON)

set(SOURCES
    src/gui_main.cpp
    src/MotorControlGui.cpp
    src/MotorWidget.cpp
    src/OptionsWidget.cpp
    ../Client/src/Client.cpp
    ../include/MySocketTCP.cpp
)

set(FORMS
    forms/form_motorwidget.ui
    forms/form_optionsgui.ui
)


set(HEADERS
    include/MotorControlGui.h
    include/MotorWidget.h
    include/OptionsWidget.h
    ../Client/include/Client.h
    ../include/MySocketTCP.h
    ../include/commonDefs.h
)

#options
if (USE_BIG_XRAY)
    list (APPEND FORMS 
        forms/form_motorcontrolgui.ui
    )
    set(bin_name  "XRayBoxGui64_sl7")
endif (USE_BIG_XRAY)

if (USE_LASER)
    list (APPEND HEADERS 
        include/FwheelWidget.h 
    )
    list (APPEND SOURCES 
        src/FwheelWidget.cpp 
    )
    list (APPEND FORMS 
        forms/form_fwheelwidget.ui 
        forms/form_laserboxgui.ui
    )
    set(bin_name  "LaserBoxGui64_sl7")
endif (USE_LASER)

if (USE_VACUUM)
    list (APPEND HEADERS 
        include/PGaugeWidget.h 
    )
    list (APPEND SOURCES 
        src/PGaugeWidget.cpp 
    )
    list (APPEND FORMS 
        forms/form_pgaugewidget.ui 
        forms/form_vacuumboxgui.ui
    )
    set(bin_name  "VacuumBoxGui64_sl7")
endif (USE_VACUUM)




qt4_wrap_ui(FORMS_H ${FORMS})

set(RESOURCES
    include/icons.qrc
)
qt4_add_resources(RESOURCES_SRCS ${RESOURCES})

add_executable(${bin_name}
    ${SOURCES}
    ${HEADERS}
    ${FORMS_H}
    ${RESOURCES_SRCS}
)
target_include_directories(${bin_name} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../Client/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_INSTALL_INCLUDEDIR}
    ${QT_INCLUDE_DIR}
    ${QT_QTCORE_INCLUDE_DIR}
    ${QT_QTGUI_INCLUDE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(${bin_name} PUBLIC
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    png
    z
    Qt4::QtOpenGL
    Qt4::QtSvg
)   
set_target_properties(${bin_name} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
install(TARGETS ${bin_name} RUNTIME DESTINATION bin)
