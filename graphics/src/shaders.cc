#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <fmt/color.h>
#include <stdexcept>

#include "shaders.hpp"

namespace tetragon::graphics {

namespace {
	int convert_shader_type(const ShaderType type) {
		switch (type) {
			case ShaderType::VERTEX: return GL_VERTEX_SHADER;
			case ShaderType::FRAGMENT: return GL_FRAGMENT_SHADER;
		}
		throw std::runtime_error("Unexpected shader type");
	}

	GLObject create_shader(const ShaderType type, const char* source) {
		const GLObject shader = glCreateShader(convert_shader_type(type));
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



} // tetragon::graphics
