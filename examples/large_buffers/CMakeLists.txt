cmake_minimum_required(VERSION 3.0.0)
project(large)

find_package(rpclib REQUIRED)

include_directories(${RPCLIB_INCLUDE_DIR})

add_executable(large_server large_server.cc)
target_link_libraries(large_server ${RPCLIB_LIBS})
set_target_properties(
        large_server
        PROPERTIES
        CXX_STANDARD 17
        COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${RPCLIB_EXTRA_FLAGS}")
target_compile_definitions(large_server PUBLIC ${RPCLIB_COMPILE_DEFINITIONS})

add_executable(large_client large_clinet.cc)
target_link_libraries(large_client ${RPCLIB_LIBS})
set_target_properties(
        large_client
        PROPERTIES
        CXX_STANDARD 17
        COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${RPCLIB_EXTRA_FLAGS}")
target_compile_definitions(large_client PUBLIC ${RPCLIB_COMPILE_DEFINITIONS})
