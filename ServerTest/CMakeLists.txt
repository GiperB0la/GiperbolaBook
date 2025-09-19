project(ServerTest LANGUAGES CXX)

add_executable(${PROJECT_NAME} ServerTest.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE ServerLib)