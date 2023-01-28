cmake_minimum_required(VERSION 3.14.7)

project(plot VERSION 0.1.0)

add_executable(plot WIN32
  main.cpp)

install(TARGETS plot
  RUNTIME DESTINATION bin)

include(CPack)
