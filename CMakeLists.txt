cmake_minimum_required(VERSION 3.14.7)

project(plot VERSION 0.1.0)

option(PLOT_NO_COMPILER_WARNINGS OFF "Disables compiler warnings.")

find_package(glfw3 REQUIRED)
find_package(Freetype REQUIRED)
find_package(OpenMP REQUIRED COMPONENTS CXX)
find_package(GTest REQUIRED CONFIG)

if(WIN32)
  find_path(MLPACK_INCLUDE_DIRS "mlpack/mlpack.hpp")

  add_library(plot_mlpack INTERFACE)

  target_include_directories(plot_mlpack INTERFACE "${MLPACK_INCLUDE_DIRS}")

  add_library(mlpack::mlpack ALIAS plot_mlpack)

else(WIN32)
  find_package(mlpack REQUIRED)
endif(WIN32)

#==============#
# Dependencies #
#==============#

include(FetchContent)

FetchContent_Declare(imgui
  URL "https://github.com/ocornut/imgui/archive/refs/tags/v1.89.2.zip"
  URL_HASH "SHA256=ecb213f444da7f8512ad49e28c83c2b9fe952d4ee0fc34505f458e25f1cae53e")
FetchContent_MakeAvailable(imgui)
FetchContent_GetProperties(imgui)

FetchContent_Declare(implot
  URL "https://github.com/epezent/implot/archive/refs/tags/v0.14.zip")
FetchContent_MakeAvailable(implot)
FetchContent_GetProperties(implot)

FetchContent_Declare(pfd
  URL "https://github.com/samhocevar/portable-file-dialogs/archive/5652fbd0df05f001aa2e92d86c22f762a03c1fd9.zip"
  URL_HASH "SHA256=9a0a0bcd9fd5190c1f24dd25b765d0c108ed657ed5ca9f1d5135e08fc5360632")
FetchContent_MakeAvailable(pfd)

#=================#
# Support Library #
#=================#

add_library(plotlib
  source/exception.hpp
  source/exception.cpp
  source/csv.hpp
  source/csv.cpp
  source/data_set.hpp
  source/data_set.cpp
  source/session.hpp
  source/session.cpp)

target_compile_definitions(plotlib PUBLIC _USE_MATH_DEFINES=1)

target_compile_features(plotlib PUBLIC cxx_std_17)

target_link_libraries(plotlib PUBLIC OpenMP::OpenMP_CXX mlpack::mlpack)

if(MSVC)
  target_compile_options(plotlib PUBLIC /Zc:__cplusplus)
endif(MSVC)

#=======#
# Tests #
#=======#

add_executable(plot_tests
  source/csv_tests.cpp
  source/data_set_tests.cpp
  source/session_tests.cpp)

target_link_libraries(plot_tests PRIVATE GTest::gtest GTest::gtest_main plotlib)

target_compile_definitions(plot_tests PRIVATE "PROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\"")

#=================#
# Font Generation #
#=================#

add_executable(bin2comp
  "${imgui_SOURCE_DIR}/misc/fonts/binary_to_compressed_c.cpp")

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/font.hpp"
  COMMAND $<TARGET_FILE:bin2comp> "${imgui_SOURCE_DIR}/misc/fonts/Roboto-Medium.ttf" RobotoMedium ">${CMAKE_CURRENT_BINARY_DIR}/font.hpp")

#=========#
# Program #
#=========#

set(glad_sources
  source/glad/src/glad.c
  source/glad/include/glad/glad.h
  source/glad/include/KHR/khrplatform.h)

set(imgui_sources
  "${imgui_SOURCE_DIR}/imgui.h"
  "${imgui_SOURCE_DIR}/imgui_internal.h"
  "${imgui_SOURCE_DIR}/imstb_rectpack.h"
  "${imgui_SOURCE_DIR}/imstb_textedit.h"
  "${imgui_SOURCE_DIR}/imstb_truetype.h"
  "${imgui_SOURCE_DIR}/imgui.cpp"
  "${imgui_SOURCE_DIR}/imgui_draw.cpp"
  "${imgui_SOURCE_DIR}/imgui_tables.cpp"
  "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
  "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.h"
  "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp"
  "${imgui_SOURCE_DIR}/misc/freetype/imgui_freetype.h"
  "${imgui_SOURCE_DIR}/misc/freetype/imgui_freetype.cpp"
  "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h"
  "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
  "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.h"
  "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp")

set(implot_sources
  "${implot_SOURCE_DIR}/implot.h"
  "${implot_SOURCE_DIR}/implot_internal.h"
  "${implot_SOURCE_DIR}/implot.cpp"
  "${implot_SOURCE_DIR}/implot_items.cpp")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/source/glad" PREFIX glad FILES ${glad_sources})

source_group(TREE ${implot_SOURCE_DIR} PREFIX implot FILES ${implot_sources})

source_group(TREE ${imgui_SOURCE_DIR} PREFIX imgui FILES ${imgui_sources})

add_executable(plot WIN32
  source/main.cpp
  source/application.hpp
  source/application.cpp
  source/menu_bar.hpp
  source/menu_bar.cpp
  source/file_dialog.hpp
  source/file_dialog.cpp
  ${glad_sources}
  ${implot_sources}
  ${imgui_sources}
  "${CMAKE_CURRENT_BINARY_DIR}/font.hpp")

if(CMAKE_COMPILER_IS_GNUCXX AND NOT PLOT_NO_COMPILER_WARNINGS)
  target_compile_options(plot PRIVATE -Wall -Wextra -Werror -Wfatal-errors)
endif(CMAKE_COMPILER_IS_GNUCXX AND NOT PLOT_NO_COMPILER_WARNINGS)

target_compile_features(plot PRIVATE cxx_std_17)

target_link_libraries(plot PRIVATE glfw Freetype::Freetype portable_file_dialogs plotlib)

target_compile_definitions(plot
  PRIVATE
    GLFW_INCLUDE_NONE=1
    IMGUI_IMPL_OPENGL_ES2=1
    IMGUI_ENABLE_FREETYPE=1
    ImDrawIdx=unsigned)

target_include_directories(plot
  PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}"
    source/glad/include
    "${implot_SOURCE_DIR}"
    "${imgui_SOURCE_DIR}"
    "${imgui_SOURCE_DIR}/misc/cpp"
    "${imgui_SOURCE_DIR}/misc/freetype"
    "${imgui_SOURCE_DIR}/backends")

if(COMMIT)
  target_compile_definitions(plot PRIVATE PLOT_COMMIT=${COMMIT})
endif(COMMIT)

if(TIMESTAMP)
  target_compile_definitions(plot PRIVATE PLOT_TIMESTAMP=${TIMESTAMP})
endif(TIMESTAMP)

install(TARGETS plot
  RUNTIME DESTINATION bin)

include(CPack)
