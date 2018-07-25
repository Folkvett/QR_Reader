cmake_minimum_required(VERSION 3.0)

project(qr_reader)

file(GLOB_RECURSE SRC ${CMAKE_CURRENT_LIST_DIR}/src/*)

add_executable(qr_reader ${SRC})

install(TARGETS qr_reader DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
