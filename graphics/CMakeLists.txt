set(MODULE_NAME graphics)
set(SOURCES
		src/primitives.cc
		src/shaders.cc
		src/shapes.cc
)
set(HEADERS_DIR include/tetragon/graphics)
set(HEADERS
		${HEADERS_DIR}/primitives.hpp
		${HEADERS_DIR}/shaders.hpp
		${HEADERS_DIR}/shapes.hpp
)

find_package(opengl_system REQUIRED)
find_package(glfw3 REQUIRED)
find_package(spdlog REQUIRED)
find_package(glad REQUIRED)

add_library(${MODULE_NAME} STATIC ${SOURCES} ${HEADERS})

target_include_directories(${MODULE_NAME} PRIVATE include/tetragon)
target_include_directories(${MODULE_NAME} PUBLIC include)

target_compile_features(${MODULE_NAME} PRIVATE cxx_std_20)
target_compile_definitions(${MODULE_NAME} PRIVATE GLFW_INCLUDE_NONE)

target_link_libraries(${MODULE_NAME}
		glfw
		opengl::opengl
		spdlog::spdlog
		glad::glad
)
