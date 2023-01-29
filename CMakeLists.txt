cmake_minimum_required(VERSION 3.5)

project(field_kitchen VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

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

qt_add_executable(field_kitchen MANUAL_FINALIZATION ${PROJECT_SOURCES})

target_link_libraries(field_kitchen PRIVATE Qt6::Widgets Qt6::Sql)

set_target_properties(field_kitchen PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(field_kitchen)


endif()

add_custom_command(
        TARGET field_kitchen POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/burger.db
                ${CMAKE_CURRENT_BINARY_DIR}/burger.db)

install(TARGETS field_kitchen DESTINATION .)
install(FILES ${field_kitchen_SOURCE_DIR}/burger.db DESTINATION .)
