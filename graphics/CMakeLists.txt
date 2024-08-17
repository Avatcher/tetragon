set(MODULE_NAME graphics)
set(SOURCES
	src/graphics.cc
)

find_package(OpenGL REQUIRED)
find_package(spdlog REQUIRED)

add_library(${MODULE_NAME} STATIC ${SOURCES})

target_include_directories(${MODULE_NAME} PRIVATE include/tetragon)
target_include_directories(${MODULE_NAME} PUBLIC include)

target_link_libraries(${MODULE_NAME}
	PUBLIC OpenGL::GL
	PUBLIC spdlog::spdlog
)

target_compile_features(${MODULE_NAME} PRIVATE cxx_std_20)
