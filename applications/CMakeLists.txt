set(MODULE_NAME applications)

set(SOURCES
	src/applications.cc
	src/initializations.cc
)

find_package(glad REQUIRED)
find_package(glfw3 REQUIRED)
find_package(spdlog REQUIRED)

add_library(${MODULE_NAME} STATIC ${SOURCES})

target_include_directories(${MODULE_NAME} PRIVATE include/tetragon)
target_include_directories(${MODULE_NAME} PUBLIC include)

target_compile_definitions(${MODULE_NAME} PRIVATE GLFW_INCLUDE_NONE)

target_link_libraries(${MODULE_NAME}
	glad::glad
	glfw
	spdlog::spdlog
)
