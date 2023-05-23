cmake_minimum_required(VERSION 3.0)

project(DIVEYE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -lnvidia-ml -L/lib64")

set(CMAKE_BUILD_TYPE Debug)

include_directories(${CMAKE_SOURCE_DIR}/include /usr/include/ ${DIVEYE_SOURCE_DIR}/lib/libpq/include ${DIVEYE_SOURCE_DIR}/lib/pqxx/include/ ${DIVEYE_SOURCE_DIR}/lib/libhv/include/ ${DIVEYE_SOURCE_DIR}/lib/include/)
LINK_LIBRARIES(pqxx pq hv)
link_directories(main_debug PRIVATE ${CMAKE_SOURCE_DIR}/lib/pqxx/lib ${CMAKE_SOURCE_DIR}/lib/libpq/lib ${CMAKE_SOURCE_DIR}/lib/libhv/lib) 
add_executable(main_debug main.cpp src/DBUtil.cpp src/FrqEye.cpp src/initial.cpp src/test.cpp src/RecordTool.cpp src/Tables.cpp src/Analyze.cpp src/Deliver.cpp src/httpd.cpp)