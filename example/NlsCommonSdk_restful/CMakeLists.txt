cmake_minimum_required(VERSION 2.6)
aux_source_directory(. DIR_LIB_SRCS)

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

link_directories(${CMAKE_SOURCE_DIR}/lib/linux)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(tokenDemo  demo/tokenDemo.cpp)
target_link_libraries(tokenDemo nlsCommonSdk jsoncpp ssl crypto curl uuid)

add_executable(fileTransDemo  demo/fileTransDemo.cpp)
target_link_libraries(fileTransDemo nlsCommonSdk jsoncpp ssl crypto curl uuid)

add_executable(restfulAsrDemo  demo/restfulAsrDemo.cpp)
target_link_libraries(restfulAsrDemo curl)

add_executable(restfulTtsDemo  demo/restfulTtsDemo.cpp)
target_link_libraries(restfulTtsDemo jsoncpp curl)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/demo) 

