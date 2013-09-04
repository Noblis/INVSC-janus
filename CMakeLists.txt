project(janus)
cmake_minimum_required(VERSION 2.8.9)

add_subdirectory(src)
add_subdirectory(doxygen)

install(FILES LICENSE.txt DESTINATION .)
install(DIRECTORY include DESTINATION .)
