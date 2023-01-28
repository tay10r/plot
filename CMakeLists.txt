cmake_minimum_required(VERSION 3.14.7)

project(plot VERSION 0.1.0)

add_executable(plot WIN32
  main.cpp)

if(COMMIT)
  target_compile_definitions(plot PRIVATE PLOT_COMMIT=${COMMIT})
endif(COMMIT)

if(TIMESTAMP)
  target_compile_definitions(plot PRIVATE PLOT_TIMESTAMP=${TIMESTAMP})
endif(TIMESTAMP)

install(TARGETS plot
  RUNTIME DESTINATION bin)

include(CPack)
