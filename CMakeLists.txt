cmake_minimum_required(VERSION 3.16)
project(idler VERSION 1.0 LANGUAGES CXX OBJCXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Pass -DPORTABLE=ON to embed Qt statically (requires a static Qt build).
# Default (OFF) uses dynamic Qt + windeployqt for a portable folder.
option(PORTABLE "Link Qt statically for a single-exe portable build" OFF)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SOURCES
    main.cpp
    MainWindow.cpp
    resources.qrc
)

if(WIN32)
    list(APPEND SOURCES clicker_windows.cpp assets/icon.rc)
elseif(APPLE)
    list(APPEND SOURCES clicker_mac.mm)
    enable_language(OBJCXX)
elseif(UNIX)
    list(APPEND SOURCES clicker_linux.cpp)
endif()

qt_add_executable(idler WIN32 MACOSX_BUNDLE ${SOURCES})

target_link_libraries(idler PRIVATE Qt6::Widgets)

if(WIN32 AND PORTABLE)
    set_property(TARGET idler PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    if(NOT X11_Xtst_FOUND)
        message(FATAL_ERROR "libXtst not found. Install it with: sudo apt install libxtst-dev  (Debian/Ubuntu) or sudo dnf install libXtst-devel  (Fedora)")
    endif()
    target_include_directories(idler PRIVATE ${X11_INCLUDE_DIR})
    target_link_libraries(idler PRIVATE ${X11_LIBRARIES} ${X11_Xtst_LIB})

    # Install desktop icon at standard sizes
    foreach(SIZE 16 32 48 64 128 256 512)
        install(FILES "assets/icons/icon_${SIZE}.png"
            DESTINATION "share/icons/hicolor/${SIZE}x${SIZE}/apps"
            RENAME "idler.png"
        )
    endforeach()
endif()

# windeployqt — bundles Qt DLLs next to the exe on Windows
if(WIN32)
    find_program(WINDEPLOYQT windeployqt HINTS ${Qt6_DIR}/../../../bin)
    if(WINDEPLOYQT)
        add_custom_command(TARGET idler POST_BUILD
            COMMAND "${WINDEPLOYQT}"
                --release --no-translations
                --no-system-d3d-compiler --no-opengl-sw
                "$<TARGET_FILE:idler>"
            COMMENT "Running windeployqt..."
            VERBATIM
        )
    endif()
endif()

if(APPLE)
    set(ICON_FILE "${CMAKE_SOURCE_DIR}/assets/icon.icns")
    set_source_files_properties("${ICON_FILE}" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    target_sources(idler PRIVATE "${ICON_FILE}")

    target_link_libraries(idler PRIVATE
        "-framework ApplicationServices"
        "-framework CoreFoundation"
    )

    set_target_properties(idler PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.idler.autoclicker"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
        MACOSX_BUNDLE_ICON_FILE "icon.icns"
    )
endif()
# Qt framework bundling is handled by build_portable_mac.sh (macdeployqt + codesign).
