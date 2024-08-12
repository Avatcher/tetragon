cmake_minimum_required(VERSION 3.22)
project(tetragon)

include(injecto.cmake)

set(CXX_STANDARD 20)

find_package(OpenGL 3.3 REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(spdlog 1.9.2 REQUIRED)

set(SOURCES
   src/glad.c
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

target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)
