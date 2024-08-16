cmake_minimum_required(VERSION 3.22)
project(tetragon)

if(DEFINED ENV{VCPKG_ROOT})
   set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
   message("Found vcpkg installed, including its Toolchain file...")
   message("Toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
   include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()
include(injecto.cmake)

set(CXX_STANDARD 20)

option(NO_LOG_EMOJIS "" OFF)
if(NO_LOG_EMOJIS)
   add_compile_definitions(TETRAGON_NO_LOG_EMOJIS)
endif()

find_package(glad 0.1.36 REQUIRED)
find_package(OpenGL 3.3 REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(spdlog 1.9.2 REQUIRED)

set(SOURCES
   src/initializations.cc
   src/graphics.cc
)

add_executable(${PROJECT_NAME}
   src/main.cc
   ${SOURCES}
)

target_inject_resources(${PROJECT_NAME} resources.hpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${BUILT_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)
