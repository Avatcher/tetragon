#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <fmt/color.h>
#include <fmt/ranges.h>
#include <stdexcept>

#include "graphics/shaders.hpp"

namespace tetragon::graphics {

namespace {
	int convert_shader_type(const ShaderType type) {
		switch (type) {
			case ShaderType::VERTEX: return GL_VERTEX_SHADER;
			case ShaderType::FRAGMENT: return GL_FRAGMENT_SHADER;
		}
		throw std::runtime_error("Unexpected shader type");
	}

	Object create_shader(const ShaderType type, const char* source) {
		const Object shader = glCreateShader(convert_shader_type(type));
		glShaderSource(shader, 1, &source, nullptr);
		glCompileShader(shader);
		int success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			char message[512];
			glGetShaderInfoLog(shader,512, nullptr, message);
			spdlog::error("Failed to build a shader: {}", message);
			throw std::runtime_error(message);
		}
		return shader;
	}
}

Shader::Shader(const ShaderType type, const char* source):
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
	glGetProgramiv(m_object, GL_LINK_STATUS, &success);
	if (!success) {
		char message[512];
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

VertexBuffer::VertexBuffer(): VertexBuffer(Usage::STATIC) {}

VertexBuffer::VertexBuffer(const Usage usage):
		m_object(create_vertex_buffer()),
		m_usage(usage) {
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

void VertexBuffer::ensure_capacity(const uint additionalSize) {
	if (m_size + additionalSize < m_maxSize) return;
	m_maxSize *= 2;

	auto expandedBuffer = new byte[m_maxSize];
	memcpy(expandedBuffer, m_buffer, m_size);
	delete[] m_buffer;
	m_buffer = expandedBuffer;
	m_ptr = m_buffer + m_size;

	glBufferData(GL_ARRAY_BUFFER, m_size, m_buffer, (GLenum) m_usage);
	spdlog::info("Expanded {} size: {} -> {}", m_name, m_maxSize / 2, m_maxSize);
}

void VertexBuffer::bind() const {
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

VertexBuffer::Usage VertexBuffer::usage() const {
	return m_usage;
}

void VertexBuffer::set_usage(const Usage usage) {
	m_usage = usage;
}

void VertexBuffer::buffer(const void* ptr, const unsigned long size) {
	std::vector oldBufferVector((float*) m_buffer, (float*) m_buffer + m_size / sizeof(float));
	std::vector valuesVector((float*) ptr, (float*) ptr + size / sizeof(float));

	ensure_capacity(size);
	memcpy(m_ptr, ptr, size);
	m_ptr += size;
	m_size += size;
	bind();
	glBufferData(GL_ARRAY_BUFFER, m_size, m_buffer, (GLenum) m_usage);

	spdlog::debug(" {}: [ {:.1f}, {} ]",
		m_name,
		fmt::join(oldBufferVector, ", "),
		fmt::format(fg(fmt::color::green_yellow), "{:.1f}", fmt::join(valuesVector, ", "))
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

void VertexArray::bind() const {
	glBindVertexArray(m_object);
}

	VertexAttribute::VertexAttribute(const char* name, const uint size, const GLenum type,
                                 const bool normalized, const uint stride):
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

VertexAttribute VertexAttribute::Builder::build() const {
	return { m_name, m_size, m_type, m_normalized, m_stride };
}

Vertex::Vertex(const std::initializer_list<float> values) {
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

void Vertex::buffer_to(VertexBuffer& buffer) const {
	buffer.buffer(&x, sizeof(decltype(x)) * 3);
}

} // tetragon::graphics
