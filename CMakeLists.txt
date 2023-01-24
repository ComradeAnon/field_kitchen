cmake_minimum_required(VERSION 3.5)

project(field_kitchen VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ON)


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Sql)

if(Qt{QT_VERSION_MAJOR}_FOUND AND WIN32 AND TARGET Qt{QT_VERSION_MAJOR}::qmake AND NOT TARGET Qt{QT_VERSION_MAJOR}::windeployqt)
    get_target_property(_qt{QT_VERSION_MAJOR}_qmake_location Qt{QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)

    execute_process(
        COMMAND "${_qt{QT_VERSION_MAJOR}_qmake_location}" -query QT_INSTALL_PREFIX
        RESULT_VARIABLE return_code
        OUTPUT_VARIABLE qt{QT_VERSION_MAJOR}_install_prefix
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(imported_location "${qt{QT_VERSION_MAJOR}_install_prefix}/bin/windeployqt.exe")

    if(EXISTS ${imported_location})
        add_executable(Qt{QT_VERSION_MAJOR}::windeployqt IMPORTED)

        set_target_properties(Qt{QT_VERSION_MAJOR}::windeployqt PROPERTIES
            IMPORTED_LOCATION ${imported_location}
        )
    endif()
endif()

set(PROJECT_SOURCES
        main.cpp
        orderwindow.cpp
        orderwindow.h
        orderwindow.ui
        dish_change_form.cpp
        dish_change_form.h
        dish_change_form.ui
        log_in_accaunt.h
        log_in_accaunt.cpp
        log_in_accaunt.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(field_kitchen
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(field_kitchen
        ${PROJECT_SOURCES}
    )
endif()

if(TARGET Qt6::windeployqt)
    # execute windeployqt in a tmp directory after build
    add_custom_command(TARGET field_kitchen
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/windeployqt"
        COMMAND set PATH=%PATH%$<SEMICOLON>${qt6_install_prefix}/bin
        COMMAND Qt6::windeployqt --dir "${CMAKE_CURRENT_BINARY_DIR}/windeployqt" "$<TARGET_FILE_DIR:field_kitchen>/$<TARGET_FILE_NAME:field_kitchen>"
    )

    # copy deployment directory during installation
    install(
        DIRECTORY
        "${CMAKE_CURRENT_BINARY_DIR}/windeployqt/"
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

target_link_libraries(field_kitchen PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(field_kitchen PRIVATE Qt${QT_VERSION_MAJOR}::Sql)

set_target_properties(field_kitchen PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS field_kitchen
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(field_kitchen)


endif()

add_custom_command(
        TARGET field_kitchen POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/burger.db
                ${CMAKE_CURRENT_BINARY_DIR}/burger.db)
