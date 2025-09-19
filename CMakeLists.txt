cmake_minimum_required(VERSION 3.16)
project(GiperbolaBook LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Netlib)
add_subdirectory(ServerLib)
add_subdirectory(GiperbolaBook)
add_subdirectory(ClientTest)
add_subdirectory(Server)
add_subdirectory(ServerTest)