cmake_minimum_required(VERSION 3.0)

project(DIVEYE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -lnvidia-ml -L/lib64")

set(CMAKE_BUILD_TYPE Debug)

include_directories(${CMAKE_SOURCE_DIR}/include /usr/include/ /home/s09003/opt/lib/diveye/libpq/include /home/s09003/opt/lib/diveye/pqxx/include/ /home/s09003/opt/lib/diveye/libhv/include/ /usr/local/cuda-12.1/include/)
LINK_LIBRARIES(pqxx pq hv)
link_directories(main_debug PRIVATE /home/s09003/opt/lib/diveye/pqxx/lib /home/s09003/opt/lib/diveye/libpq/lib /home/s09003/opt/lib/diveye/libhv/lib) 
add_executable(main_debug main.cpp src/DBUtil.cpp src/FrqEye.cpp src/initial.cpp src/test.cpp src/RecordTool.cpp src/Tables.cpp src/Analyze.cpp src/Deliver.cpp src/httpd.cpp)