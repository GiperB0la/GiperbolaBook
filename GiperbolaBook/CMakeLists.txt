project(GiperbolaBook LANGUAGES CXX)

set(SOURCES
    Main.cpp
    src/ChatEntity.cpp
    src/GiperbolaBookApp.cpp
    src/MainWindow.cpp
    src/WelcomeWindow.cpp
    src/Widgets.cpp
)

include_directories(include)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(SFML 2.6 COMPONENTS system window graphics network audio REQUIRED)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Netlib
        sfml-system sfml-window sfml-graphics sfml-audio sfml-network
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/GiperbolaBook/Graphics
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/Graphics
)