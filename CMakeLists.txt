cmake_minimum_required(VERSION 3.22)
project(tetragon)

set(CXX_STANDARD 20)

set(RESOURCES_DIR ${CMAKE_SOURCE_DIR}/resources)
set(SYNTHETIC_DIR ${CMAKE_BINARY_DIR}/synthetic)

function(get_resource_macro Variable File)
   set(MacroName "RESOURCE_")
   string(APPEND MacroName ${File})
   string(TOUPPER ${MacroName} MacroName)
   string(REGEX REPLACE "[/\\.]" "_" MacroName ${MacroName})
   set(${Variable} ${MacroName} PARENT_SCOPE)
endfunction()

function(generate_resource_macro MacroVariable File)
   file(READ ${RESOURCES_DIR}/${File} FILE_TEXT)
   string(APPEND INJECTION "${FILE_TEXT}")
   string(REPLACE "\"" "\\\"" INJECTION "${INJECTION}")
   string(REPLACE "\n" "\\n" INJECTION "${INJECTION}")
   string(REPLACE "\t" "\\t" INJECTION "${INJECTION}")
   string(PREPEND INJECTION "\"")
   string(APPEND INJECTION "\"")
   get_resource_macro(MacroName ${File})
   set(${MacroName} ${INJECTION} PARENT_SCOPE)
   set(${MacroVariable} ${MacroName} PARENT_SCOPE)
endfunction()

function(target_inject_resources Target GenerateFile)
   file(GLOB_RECURSE Files "${RESOURCES_DIR}/**")
   foreach(File ${Files})
      string(REPLACE "${RESOURCES_DIR}/" "" File "${File}")
      generate_resource_macro(Macro ${File})
      string(APPEND Definitions "#define ${Macro} ${${Macro}}\n")
   endforeach()
   set(BuiltFile "${SYNTHETIC_DIR}/${GenerateFile}")

   string(TOUPPER "${Target}_${GenerateFile}" HeaderName)
   string(REGEX REPLACE "[/\\.]" "_" HeaderName "${HeaderName}")

   file(MAKE_DIRECTORY ${SYNTHETIC_DIR})
   file(REMOVE ${BuiltFile})
   file(TOUCH ${BuiltFile})
   file(APPEND ${BuiltFile} "#ifndef ${HeaderName}\n")
   file(APPEND ${BuiltFile} "#define ${HeaderName}\n\n")
   file(APPEND ${BuiltFile} "${Definitions}\n")
   file(APPEND ${BuiltFile} "#endif // ${HeaderName}\n")

   target_include_directories(${Target} PRIVATE ${SYNTHETIC_DIR})
endfunction()

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
