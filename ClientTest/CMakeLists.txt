project(ClientTest LANGUAGES CXX)

add_executable(${PROJECT_NAME} ClientTest.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Netlib)