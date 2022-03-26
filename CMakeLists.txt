cmake_minimum_required(VERSION 3.7)
project(plotjuggler_motor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
    )

#-------  Include Qt dependencies -------
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Widgets
    Xml
    Svg )

include_directories(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Xml_INCLUDE_DIRS}
    ${Qt5Svg_INCLUDE_DIRS} )

set(QT_LIBRARIES
    Qt5::Core
    Qt5::Widgets
    Qt5::Xml
    Qt5::Svg )

add_definitions( ${QT_DEFINITIONS} -DQT_PLUGIN -DQT_SHARED)

set( PJ_LIBRARIES ${QT_LIBRARIES} )

find_package(PlotJuggler REQUIRED)
message(STATUS "PlotJuggler FOUND")

include_directories(${PlotJuggler_INCLUDE_DIRS})
list(APPEND ${PJ_LIBRARIES} ${PlotJuggler_LIBRARIES} )
set(PJ_PLUGIN_INSTALL_DIRECTORY bin )


# message(${PlotJuggler_INCLUDE_DIRS})
# find_file(plotjuggler_data_streamer PlotJuggler/datastreamer_base.h ${PlotJuggler_INCLUDE_DIRS})
# message(${plotjuggler_data_streamer})
# #set(pj_headers ${PlotJuggler_INCLUDE_DIR}/datastreamer_base.h)
# #${PlotJuggler_INCLUDE_DIR}/plotdata.h)
SET( SRC plotjuggler_motor.cpp) # ${pj_headers})

add_library(PlotJugglerMotor SHARED ${SRC} ${UI_SRC}  )

target_link_libraries(PlotJugglerMotor
    ${PJ_LIBRARIES}
    ${PlotJuggler_LIBRARIES}
    motor_manager)

install(TARGETS PlotJugglerMotor DESTINATION ${PJ_PLUGIN_INSTALL_DIRECTORY}  )
