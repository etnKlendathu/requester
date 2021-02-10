cmake_minimum_required(VERSION 3.13)

##############################################################################################################

project(requester
    VERSION 1.0.0
    DESCRIPTION "IPM Rest request"
)

find_package(Qt5 COMPONENTS Gui QuickControls2 Network Xml Concurrent)

add_executable(${PROJECT_NAME}
    main.cpp
    request.cpp
    request.h
    history.h
    history.cpp
    resources.qrc
)

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::QuickControls2 Qt5::Network Qt5::Xml Qt5::Concurrent fty-pack)
set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC ON)
set_target_properties(${PROJECT_NAME} PROPERTIES AUTORCC ON)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
