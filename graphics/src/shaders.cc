#define GL_GLEXT_PROTOTYPES

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <fmt/color.h>
#include <fmt/ranges.h>
#include <stdexcept>
// #include <GLFW/glfw3.h>
// #include <GL/gl.h>
// #include <GL/gl.h>
// #include <GL/glext.h>

#include "graphics/shaders.hpp"

namespace tetragon::graphics {

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

// template<>
// void Uniform<double>::set_value(double const& value) {
// 	program().bind();
// 	glUniform1d(location(), value);
// }
//
// template<>
// double Uniform<double>::value() const {
// 	double value;
// 	glGetUniformdv(program().m_object, location(), &value);
// 	return value;
// }

template<>
void Uniform<float>::set_value(float const& value) {
	bind_program();
	glUniform1f(location(), value);
}

template<>
float Uniform<float>::value() const {
	float value;
	glGetUniformfv(program().m_object, location(), &value);
	return value;
}

template<>
void Uniform<int>::set_value(int const& value) {
	bind_program();
	glUniform1i(location(), value);
}

template<>
int Uniform<int>::value() const {
	int value;
	glGetUniformiv(program().m_object, location(), &value);
	return value;
}

template<>
void Uniform<uint>::set_value(uint const& value) {
	bind_program();
	glUniform1ui(location(), value);
}

template<>
uint Uniform<uint>::value() const {
	uint value;
	glGetUniformuiv(program().m_object, location(), &value);
	return value;
}

template<>
void Uniform<Vertex>::set_value(Vertex const& value) {
	bind_program();
	glUniform3f(location(), value.x, value.y, value.z);
}

template<>
Vertex Uniform<Vertex>::value() const {
	float x, y, z;
	glGetUniformfv(program().m_object, location(), &x);
	return Vertex { x, y, z }; 
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

bool ShaderProgram::has_uniform(const char* name) const {
	return glGetUniformLocation(m_object, name) != -1;
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
	m_buffer = new byte[m_maxSize] {};
	for (int i = 0; i < m_maxSize; i++) {
		const byte b = m_buffer[i];
		assert(b == 0);
	}
	m_ptr = m_buffer;
	m_name = "Buffer";
	bind();
}

VertexBuffer::~VertexBuffer() {
	glDeleteBuffers(1, &m_object);
	delete[] m_buffer;
}

void VertexBuffer::ensure_capability(uint additionalSize, Usage usage) {
	if (m_size + additionalSize < m_maxSize) return;
	m_maxSize *= 2;

	auto expandedBuffer = new byte[m_maxSize];
	memcpy(expandedBuffer, m_buffer, m_size);
	delete[] m_buffer;
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
	// std::vector<float> oldBufferVector((float*) m_buffer, (float*) m_buffer + m_size / sizeof(float));
	// std::vector<float> valuesVector((float*) ptr, (float*) ptr + size / sizeof(float));

	ensure_capability(size, usage);
	memcpy(m_ptr, ptr, size);
	m_ptr += size;
	m_size += size;
	bind();
	glBufferData(GL_ARRAY_BUFFER, m_size, m_buffer, (GLenum) usage);

	// spdlog::debug("Buffered {} bytes, size: {}", size, m_size);
	// spdlog::debug(" {}: [ {:.1f}, {} ]",
	// 	m_name,
	// 	fmt::join(oldBufferVector, ", "),
	// 	fmt::format(fmt::fg(fmt::color::green_yellow), "{:.1f}", fmt::join(valuesVector, ", "))
	// );
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

} // tetragon::graphics
