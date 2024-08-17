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

target_link_libraries(${MODULE_NAME}
	PRIVATE glad::glad
	PUBLIC glfw
	PUBLIC spdlog::spdlog
)
