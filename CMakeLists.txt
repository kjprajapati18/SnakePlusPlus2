cmake_minimum_required(VERSION 3.16.3)
set(CMAKE_CXX_STANDARD 11)

project(RPS VERSION 1.0)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

find_package(Boost 1.71.0 REQUIRED COMPONENTS system)
list(APPEND INC ${Boost_INCLUDE_DIRS})
list(APPEND LIBS ${Boost_LIBRARIES})


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
list(APPEND LIBS Threads::Threads)


add_subdirectory(shared)
list(APPEND LIBS serverGameInfo)

include_directories(${INC})

add_executable(server server.cpp)
target_link_libraries(server PRIVATE ${LIBS})

add_executable(client client.cpp)
target_link_libraries(client PRIVATE ${LIBS})
