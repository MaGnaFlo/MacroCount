cmake_minimum_required(VERSION 3.5)

project(MacroCount VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/itemtable.cpp
        src/entrytable.cpp
        src/foodtable.cpp
        src/addfoodwidget.cpp

        include/mainwindow.h
        include/item.h
        include/itemtable.h
        include/entrytable.h
        include/foodtable.h
        include/addfoodwidget.h

        src/mainwindow.ui
        src/addfoodwidget.ui
)

qt_add_executable(MacroCount
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_link_libraries(MacroCount PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(MacroCount PUBLIC include)

set_target_properties(MacroCount PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS MacroCount
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


qt_finalize_executable(MacroCount)

