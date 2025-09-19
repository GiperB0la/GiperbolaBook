project(Netlib LANGUAGES CXX)

set(SOURCES
    src/NetLib.cpp
)

include_directories(include)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_compile_definitions(${PROJECT_NAME} PRIVATE NETLIB_EXPORTS)

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)