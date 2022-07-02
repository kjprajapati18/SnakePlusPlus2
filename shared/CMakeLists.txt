add_library(serverGameInfo serverGameInfo.cpp)
target_include_directories(serverGameInfo INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(udpServer udpServer.cpp)
target_include_directories(udpServer INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(udpServer PRIVATE serverGameInfo)