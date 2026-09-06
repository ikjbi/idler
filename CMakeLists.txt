cmake_minimum_required(VERSION 3.16)
project(idler VERSION 1.0 LANGUAGES CXX OBJCXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SOURCES
    main.cpp
    MainWindow.cpp
)

if(WIN32)
    list(APPEND SOURCES clicker_windows.cpp)
elseif(APPLE)
    list(APPEND SOURCES clicker_mac.mm)
    enable_language(OBJCXX)
endif()

qt_add_executable(idler WIN32 MACOSX_BUNDLE ${SOURCES})

target_link_libraries(idler PRIVATE Qt6::Widgets)

if(APPLE)
    target_link_libraries(idler PRIVATE
        "-framework ApplicationServices"
        "-framework CoreFoundation"
    )
    # Accessibility permission is required on macOS for CGEventPost to work
    set_target_properties(idler PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.idler.autoclicker"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    )
endif()
