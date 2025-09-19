project(Server LANGUAGES CXX)

set(SOURCES
    Main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE ServerLib)