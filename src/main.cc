#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <fmt/color.h>
#include <tetragon/initializations.hpp>
#include <tetragon/applications.hpp>
#include <tetragon/graphics/primitives.hpp>
#include <tetragon/graphics/shaders.hpp>
#include <tetragon/graphics/shapes.hpp>

#include "resources.hpp"
#include "tetragon/graphics/textures.hpp"

#define WINDOW_NAME "Tetragon"
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400

using namespace tetragon;
using namespace tetragon::graphics;

void postpone_closing(Window& window, int seconds);
ShaderProgram create_shader_program();
void update_uniforms(Uniform<float> u_green, Uniform<Vector3> u_offset);

int main() {
	init_logs();

	class TetragonWindow : public Window {
	public:
		TetragonWindow(): Window(WINDOW_NAME, WINDOW_WIDTH, WINDOW_HEIGHT) {}

		[[nodiscard]] std::string construct_title() const {
			return fmt::format("{} {}x{}", WINDOW_NAME, width(), height());
		}

		void on_resize(int oldWidth, int oldHeight) override {
			glViewport(0, 0, width(), height());
			std::string title = construct_title();
			set_title(title.c_str());
		}
	} window;
	window.make_context();

	Controls controls(window);
	controls.add_binding(GLFW_KEY_SPACE, [](Window&) {
		spdlog::info("Press {} to exit application",
			format(fg(fmt::color::magenta), "SHIFT + SPACE"));
	});
	controls.add_binding({ GLFW_KEY_LEFT_SHIFT, GLFW_KEY_SPACE }, [](Window& window) {
		spdlog::info("Exit hotkey pressed, closing the application...");
		window.set_should_close(true);
	});

	const Triangle triangle{
		vec( -.5f, -.25f ),
		vec(	.5f, -.25f ),
		vec(	.0f,	.75f )
	};
	const Triangle triangleBravo{
		vec( -.5f, -.25f ),
		vec(	.5f, -.25f ),
		vec(	.0f, -.8f )
	};
	// const Square square{
	// 	{ .5, .5 },
	// 	{ -.5, -.5 }
	// };

	VertexArray VAO;
	VAO.bind(); 

	constexpr auto usage = VertexBuffer::Usage::STATIC;
	VertexBuffer vbo_position(Vector3().vertex_size(), usage),
		vbo_color(Vector3().vertex_size(), usage),
		vbo_texture(Vector2().vertex_size(), usage);

	ShaderProgram shaderProgram = create_shader_program();
	shaderProgram.bind();

	VertexAttribute::Builder vertexAttribBuilder = VertexAttribute::Builder()
			.set_type(GL_FLOAT)
			.set_size(3);

	const VertexAttribute posAttrib = vertexAttribBuilder.set_name("pos").build();
	const VertexAttribute colorAttrib = vertexAttribBuilder.set_name("color").build();
	const VertexAttribute textureAttrib = vertexAttribBuilder.set_size(2).set_name("texCoord").build();

	vbo_position.add_attribute(posAttrib);
	vbo_color.add_attribute(colorAttrib);
	vbo_texture.add_attribute(textureAttrib);

	vbo_position.buffer(vec(-.5, -.5, 1));
	vbo_position.buffer(vec(-.5, .5, 1));
	vbo_position.buffer(vec(.5, -.5, 1));
	vbo_position.buffer(vec(.5, .5, 1));

	vbo_color.buffer(vec(1, 0, 0));
	vbo_color.buffer(vec( 1, 1, 0 ));
	vbo_color.buffer(vec( 1, 1, 1 ));
	vbo_color.buffer(vec( 0, 1, 1 ));

	vbo_texture.buffer(vec(0, 0));
	vbo_texture.buffer(vec(0, 1));
	vbo_texture.buffer(vec(1, 0));
	vbo_texture.buffer(vec(1, 1));

	ElementBuffer ebo;
	ebo.buffer_indices({0, 1, 2});
	ebo.buffer_indices({1, 2, 3});

	auto u_green = shaderProgram.uniform<float>("u_green");
	auto u_offset = shaderProgram.uniform<Vector3>("u_offset");

	auto textureBricks = Texture::from_file("../resources/textures/bricks.png");
	textureBricks.set_wrapping(TextureWrapping::REPEAT);
	textureBricks.set_downscaling(TextureFiltering::NEAREST);
	textureBricks.set_upscaling(TextureFiltering::NEAREST);

	auto texturePaint = Texture::from_file("../resources/textures/paint.png");
	texturePaint.set_wrapping(TextureWrapping::REPEAT);
	texturePaint.set_downscaling(TextureFiltering::NEAREST);
	texturePaint.set_upscaling(TextureFiltering::NEAREST);

	textureBricks.bind(0);
	texturePaint.bind(1);

	shaderProgram.uniform<Texture>("u_textureBricks").set_value(textureBricks);
	shaderProgram.uniform<Texture>("u_texturePaint").set_value(texturePaint);

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// postpone_closing(window, 2);
	while (!window.should_close()) {
		glClearColor(.3f, .3f, .5f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		controls.process();
		update_uniforms(u_green, u_offset);

		shaderProgram.bind();
		VAO.bind();
		// glDrawArrays(GL_TRIANGLES, 0, vbo_position.size() / vbo_position.vertex_size());
		ebo.draw(GL_TRIANGLES);
		VertexArray::unbind_all();

		window.swap_buffers();
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void postpone_closing(Window& window, int seconds) {
	std::thread t([&window, seconds]() {
		spdlog::info("Postponing closing for {} seconds", seconds);
		std::this_thread::sleep_for(std::chrono::seconds(seconds));
		spdlog::info("Closing the application...");
		window.set_should_close(true);
	});
	t.detach();
}

ShaderProgram create_shader_program() {
	using namespace tetragon;
	const char* vertexShaderSource = RESOURCE_VERTEX_VERT;
	const char* fragmentShaderSource = RESOURCE_FRAGMENT_FRAG;
	const Shader vertexShader(ShaderType::VERTEX, vertexShaderSource);
	const Shader fragmentShader(ShaderType::FRAGMENT, fragmentShaderSource);
	return ShaderProgram::Builder()
		.attach_shader(vertexShader)
		.attach_shader(fragmentShader)
		.build();
}

void update_uniforms(Uniform<float> u_green, Uniform<Vector3> u_offset) {
	const float time = glfwGetTime();
	const float timeSin = sin(time);
	const float greenValue = fabs(timeSin);
	u_green.set_value(greenValue);

	constexpr float length = .5f;
	const Vector3 offset{ length * (1.f - timeSin) - length, 0, 0 };
	u_offset.set_value(offset);
}
