cmake_minimum_required(VERSION 3.10)
project(3decommand)

find_package(KF5TextEditor REQUIRED)
find_package(KF5I18n REQUIRED)
find_package(KF5CoreAddons REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_DEBUG_OUTPUT")

add_library(3decommand MODULE plugin.cpp)
target_link_libraries(3decommand KF5::TextEditor KF5::I18n KF5::CoreAddons Qt5::Core Qt5::Widgets)

install(TARGETS 3decommand DESTINATION /usr/lib/x86_64-linux-gnu/qt5/plugins/ktexteditor/)
