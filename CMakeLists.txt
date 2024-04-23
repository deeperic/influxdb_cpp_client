cmake_minimum_required(VERSION 3.25)
project(influx_cpp_client_example)

set(CMAKE_CXX_STANDARD 23)

include_directories("/usr/local/include")

add_executable(influxdb_cpp_client_example example.cpp)
