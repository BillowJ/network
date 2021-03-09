cmake_minimum_required(VERSION 3.0)
PROJECT (HELLO CXX)

set(targets 
    # test_scheduler.cc 
    # test_thread.cc 
    # test_tmp.cc
    test_iomanager.cc
    test_hook.cc
    )

SET(SRC_LIST
    thread.cc
    scheduler.cc
    mutex.cc
    utils.cc
    fiber.cc
    iomanager.cc
    hook.cc
    timer.cc
    fd_manager.cc
    )

SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")


# MESSAGE(STATUS "this is binary dir: " ${PROJECT_BINARY_DIR})
# MESSAGE(STATUS "this is source dir: " ${PROJECT_SOURCE_DIR})
# MESSAGE(STATUS "dir: " ${SRC_LIST})


# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
file(GLOB APP_SOURCE ${targets})
# MESSAGE(STATUS "targets: " ${APP_SOURCE})
foreach(sourcefile ${APP_SOURCE})
    string(REGEX REPLACE ".+/(.+)\\..*" "\\1" filename ${sourcefile})
    set(src ${src} ${sourcefile} ${SRC_LIST})
    # message(STATUS "src: " ${src})
    # message(STATUS "name: " ${filename})
    # message(STATUS "taget name: " ${sourcefile})
    file(GLOB headerfile "*.h")
    
    include_directories(./)
    add_executable(${filename} ${src})
    target_link_libraries(${filename} dl)
    unset(src)
    # message(STATUS "src end: " ${src})

endforeach(sourcefile)

# ADD_EXECUTABLE(test_thread ${SRC_LIST})
