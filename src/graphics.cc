#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <fmt/color.h>
#include <cstdio>
#include <map>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include "graphics.hpp"
#include "initializations.hpp"

namespace tetragon {

void callbacks::window_resize_callback(GLFWwindow* glfwWindow, int width, int height) {
   Window* window = WindowManager::INSTANCE->get_window(glfwWindow);
   if (window == nullptr) return;
   int oldWidth = window->m_width, oldHeight = window->m_height;
   window->m_width = width;
   window->m_height = height;
   window->on_resize(oldWidth, oldHeight);
}

Window::Window(const char* title, int width, int height):
	  m_title(title), m_width(width), m_height(height) {
   init_glfw();
   m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
   glfwSetFramebufferSizeCallback(m_window, callbacks::window_resize_callback);
   glfwMakeContextCurrent(m_window);
   init_glad();
   WindowManager::INSTANCE->register_window(this);
}

Window::~Window() {
   WindowManager::INSTANCE->remove_window(this);
   glfwDestroyWindow(m_window);
}

void Window::on_resize(int oldWidth, int oldHeight) {
   glViewport(0, 0, m_width, m_height);
}

void Window::make_context() const {
   glfwMakeContextCurrent(m_window);
}

void Window::swap_buffers() {
   glfwSwapBuffers(m_window);
}

const char* Window::get_title() const {
   return m_title;
}

void Window::set_title(const char* title) {
   m_title = title;
   glfwSetWindowTitle(m_window, title);
}

int Window::get_width() const {
   return m_width;
}

int Window::get_height() const {
   return m_height;
}

void Window::set_size(int width, int height) {
   m_width = width;
   m_height = height;
   glfwSetWindowSize(m_window, width, height);
}

bool Window::should_close() const {
   return glfwWindowShouldClose(m_window);
}

void Window::set_should_close(bool state) {
   glfwSetWindowShouldClose(m_window, state);
}

namespace {
   class DefaultWindowManager : public WindowManager {
	  std::map<GLFWwindow*, Window*> m_windows;
   public:
	  void register_window(Window* window) override {
		 m_windows.insert({get_glfw_window(window), window});
	  }

	  void remove_window(Window* window) override {
		 m_windows.erase(get_glfw_window(window));
	  }

	  Window* get_window(GLFWwindow* window) const override {
		 return m_windows.at(window);
	  }
   } windowManager;
}

WindowManager* const WindowManager::INSTANCE = &windowManager;

GLFWwindow* WindowManager::get_glfw_window(Window* window) {
   return window->m_window;
}

namespace {
   int convert_shader_type(ShaderType type) {
	  switch (type) {
		 case ShaderType::VERTEX: return GL_VERTEX_SHADER;
		 case ShaderType::FRAGMENT: return GL_FRAGMENT_SHADER;
	  }
	  throw std::runtime_error("Unexpected shader type");
   }

   GLuint create_shader(ShaderType type, const char* source) {
	  GLuint shader = glCreateShader(convert_shader_type(type));
	  glShaderSource(shader, 1, &source, nullptr);
	  glCompileShader(shader);
	  int success;
	  char message[512];
	  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	  if (!success) {
		 glGetShaderInfoLog(shader,512, nullptr, message);
		 spdlog::error("Failed to build a shader: {}", message);
		 throw std::runtime_error(message);
	  }
	  return shader;
   }
}

Shader::Shader(ShaderType type, const char* source):
	  m_type(type), m_object(create_shader(type, source)) {
}

Shader::~Shader() {
   glDeleteShader(m_object);
}

ShaderType Shader::get_type() const {
   return m_type;
}

ShaderProgram::ShaderProgram(GLuint program): m_object(program) {
}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(m_object);
	if (is_bound()) {
		boundInstance = nullptr;
	}
}

ShaderProgram* ShaderProgram::boundInstance = nullptr;

ShaderProgram* ShaderProgram::get_bound_instance() {
	return boundInstance;
}

void ShaderProgram::bind() {
   glUseProgram(m_object);
   boundInstance = this;
}

bool ShaderProgram::is_bound() const {
	return boundInstance == this;
}

GLuint ShaderProgram::get_attribute_location(VertexAttribute const& attribute) const {
	return glGetAttribLocation(m_object, attribute.name());
}

ShaderProgram::Builder::Builder() {
   m_object = glCreateProgram();
}

ShaderProgram::Builder& ShaderProgram::Builder::attach_shader(Shader const& shader) {
   glAttachShader(m_object, shader.m_object);
   return *this;
}

ShaderProgram ShaderProgram::Builder::build() const {
   glLinkProgram(m_object);
   int success;
   char message[512];
   glGetProgramiv(m_object, GL_LINK_STATUS, &success);
   if (!success) {
	  glGetProgramInfoLog(m_object, 512, nullptr, message);
	  spdlog::error("Failed to link a shader program: {}", message);
	  throw std::runtime_error(message);
   }
   return ShaderProgram(m_object);
}

namespace {
   Object create_vertex_buffer() {
	  Object buffer;
	  glGenBuffers(1, &buffer);
	  return buffer;
   }

   Object create_vertex_array() {
	  Object array;
	  glGenVertexArrays(1, &array);
	  return array;
   }
}

VertexBuffer::VertexBuffer():
	  m_object(create_vertex_buffer()) {
	m_maxSize = 32;
	m_buffer = new byte[m_maxSize];
	m_ptr = m_buffer;
	m_name = "Buffer";
	bind();
}

VertexBuffer::~VertexBuffer() {
	glDeleteBuffers(1, &m_object);
	delete m_buffer;
}

void VertexBuffer::ensure_capability(uint additionalSize, Usage usage) {
	if (m_size + additionalSize < m_maxSize) return;
	m_maxSize *= 2;
	byte* expandedBuffer = new byte[m_maxSize];
	memcpy(expandedBuffer, m_buffer, m_size);
	delete m_buffer;
	m_buffer = expandedBuffer;
	m_ptr = m_buffer + m_size;
	glBufferData(GL_ARRAY_BUFFER, m_size, m_buffer, (GLenum) usage);
	spdlog::info("Expanded {} size: {} -> {}", m_name, m_maxSize / 2, m_maxSize);
}

void VertexBuffer::bind() {
   glBindBuffer(GL_ARRAY_BUFFER, m_object);
}

void VertexBuffer::add_attribute(VertexAttribute const& attribute) {
	if (ShaderProgram::get_bound_instance() == nullptr) {
		spdlog::error("Failed to add attribute `{}`, as no shader program is bound", attribute.name());
		std::terminate();
	}
	ShaderProgram& shaderProgram = *ShaderProgram::get_bound_instance();
	uint layoutLocation = shaderProgram.get_attribute_location(attribute);

	bind();
	glVertexAttribPointer(layoutLocation, attribute.size(), attribute.type(),
		attribute.normalized(), attribute.stride(), nullptr);
	glEnableVertexAttribArray(layoutLocation);

	m_name = fmt::format("Buffer({})",
		fmt::format(fmt::fg(fmt::color::aqua), "`{}`", attribute.name()));
}

void VertexBuffer::buffer(const void* ptr, unsigned long size) {
   buffer(ptr, size, Usage::DYNAMIC);
}

void VertexBuffer::buffer(const void* ptr, unsigned long size, Usage usage) {
   	std::vector<float> oldBufferVector((float*) m_buffer, (float*) m_buffer + m_size / sizeof(float));
	std::vector<float> valuesVector((float*) ptr, (float*) ptr + size / sizeof(float));

	ensure_capability(size, usage);
	memcpy(m_ptr, ptr, size);
   	m_ptr += size;
   	m_size += size;
   	bind();
   	glBufferData(GL_ARRAY_BUFFER, m_size, m_buffer, (GLenum) usage);

	// spdlog::debug("Buffered {} bytes, size: {}", size, m_size);
	spdlog::debug(" {}: [ {:.1f}, {} ]",
		m_name,
		fmt::join(oldBufferVector, ", "),
		fmt::format(fmt::fg(fmt::color::green_yellow), "{:.1f}", fmt::join(valuesVector, ", "))
	);
}

unsigned long VertexBuffer::size() const {
	return m_size;
}

VertexArray::VertexArray():
	  m_object(create_vertex_array()) {
}

VertexArray::~VertexArray() {
   glDeleteVertexArrays(1, &m_object);
}

void VertexArray::bind() {
   glBindVertexArray(m_object);
}

Vertex::Vertex() {}

Vertex::Vertex(std::initializer_list<float> values) {
  if (values.size() >= 3) {
	z = *(values.begin() + 2);
  }
  if (values.size() >= 2) {
	y = *(values.begin() + 1);
  }
  if (values.size() >= 1) {
	x = *(values.begin());
  }
}

void Vertex::buffer_to(VertexBuffer& buffer, VertexBuffer::Usage usage) const {
	buffer.buffer(&x, 3 * sizeof(float), usage);
}

Triangle::Triangle() {}

Triangle::Triangle(Vertex a, Vertex b, Vertex c):
		a(a), b(b), c(c) {
}

const Vertex* Triangle::vertecies() const {
	return &a;
}

unsigned int Triangle::vertex_count() const {
	return 3;
}

void Triangle::buffer_to(VertexBuffer &buffer,
						 VertexBuffer::Usage usage) const {
	a.buffer_to(buffer, usage);
	b.buffer_to(buffer, usage);
	c.buffer_to(buffer, usage);
}

/*
  3   2
  
  1   4
*/
Square::Square(Vertex firstCorner, Vertex secondCorner) {
	auto [maxX, minX] = std::minmax(firstCorner.x, secondCorner.x);
	auto [maxY, minY] = std::minmax(firstCorner.y, secondCorner.y);
	firstCorner = { minX, minY };
	secondCorner = { maxX, maxY };
	Vertex thirdCorner{ minX, maxY };
	Vertex fourthCorner{ maxX, minY };
	a = Triangle(firstCorner, thirdCorner, fourthCorner);
	b = Triangle(secondCorner, fourthCorner, thirdCorner);
}

const Vertex* Square::vertecies() const {
	return a.vertecies();
}

unsigned int Square::vertex_count() const {
	return a.vertex_count() + b.vertex_count();
}

void Square::buffer_to(VertexBuffer& buffer, VertexBuffer::Usage usage) const {
	a.buffer_to(buffer, usage);
	b.buffer_to(buffer, usage);
}

VertexAttribute::VertexAttribute(const char* name, uint size, GLenum type,
        bool normalized, uint stride):
	m_name(name), m_size(size), m_type(type),
	m_normalized(normalized), m_stride(stride) {
}

const char* VertexAttribute::name() const {
	return m_name;
}

uint VertexAttribute::size() const {
	return m_size;
}

GLenum VertexAttribute::type() const {
	return m_type;
}

bool VertexAttribute::normalized() const {
	return m_normalized;
}

uint VertexAttribute::stride() const {
	return m_stride;
}

VertexAttribute::Builder& VertexAttribute::Builder::set_name(const char* name) {
	m_name = name;
	return *this;
}

VertexAttribute::Builder& VertexAttribute::Builder::set_size(uint size) {
	m_size = size;
	return *this;
}

VertexAttribute::Builder& VertexAttribute::Builder::set_type(GLenum type) {
	m_type = type;
	return *this;
}

VertexAttribute::Builder& VertexAttribute::Builder::set_normalized(bool state) {
	m_normalized = state;
	return *this;
}

VertexAttribute::Builder& VertexAttribute::Builder::set_stride(uint size) {
	m_stride = size;
	return *this;
}

VertexAttribute VertexAttribute::Builder::build() {
	return VertexAttribute (
		m_name, m_size, m_type, m_normalized, m_stride
	);
}

} // namespace tetragon
