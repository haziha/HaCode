cmake_minimum_required(VERSION 3.5)

project(DriveInfo LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

include_directories(../../DeferClass ../../Utf8Utf16)

add_executable(DriveInfo
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        DriveInfo.cpp DriveInfo.h
        ../../DeferClass/DeferClass.cpp ../../DeferClass/DeferClass.h
        ../../Utf8Utf16/Utf8Utf16.cpp ../../Utf8Utf16/Utf8Utf16.h)

target_link_libraries(DriveInfo PRIVATE Qt5::Widgets)
