cmake_minimum_required(VERSION 3.22)
project(tetragon)

#if(DEFINED ENV{VCPKG_ROOT})
#   set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
#   message("Found vcpkg installed, including its Toolchain file...")
#   message("Toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
#   include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
#endif()
include(injecto.cmake)

add_subdirectory(applications)
add_subdirectory(graphics)

set(CXX_STANDARD 20)

option(NO_LOG_EMOJIS "" OFF)
if(NO_LOG_EMOJIS)
   add_compile_definitions(TETRAGON_NO_LOG_EMOJIS)
endif()

find_package(glad REQUIRED)
find_package(opengl_system REQUIRED)
find_package(glfw3 REQUIRED)
find_package(spdlog REQUIRED)

set(SOURCES

)

add_executable(${PROJECT_NAME}
   src/main.cc
   ${SOURCES}
)

target_inject_resources(${PROJECT_NAME} resources.hpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)

target_link_libraries(${PROJECT_NAME}
   PRIVATE applications
   PRIVATE graphics
)
