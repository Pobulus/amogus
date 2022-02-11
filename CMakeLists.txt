cmake_minimum_required(VERSION 3.0)

project(amogus)


find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(amogus main.cpp amogus.cpp)
target_link_libraries(amogus ${CURSES_LIBRARIES})
add_executable(server amogus-server-main.cpp amogus.cpp)
target_link_libraries(server ${CURSES_LIBRARIES})
add_executable(keybinder keybinder.cpp)
target_link_libraries(keybinder ${CURSES_LIBRARIES})

install(TARGETS amogus RUNTIME DESTINATION bin)
