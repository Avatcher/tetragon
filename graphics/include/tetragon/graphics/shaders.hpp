#ifndef TETRAGON_GRAPHICS_SHADERS_HPP
#define TETRAGON_GRAPHICS_SHADERS_HPP

#include <GL/gl.h>
#include <bits/types.h>
#include <initializer_list>
#include <concepts>
#include <string>

namespace tetragon::graphics {

using Object = GLuint;

enum class ShaderType {
	VERTEX, FRAGMENT	
};

class Shader {
	const ShaderType m_type;
	const Object m_object;
public:
	Shader(ShaderType type, const char* source);
	Shader(Shader const&) = delete;
	virtual ~Shader();

	ShaderType get_type() const;

	friend class ShaderProgram;
};

class VertexAttribute {
	const char* const m_name;
	const uint m_size;
	const GLenum m_type;
	const bool m_normalized;
	const uint m_stride;

public:
	VertexAttribute(const char* name, uint size, GLenum type,
			bool normalized, uint stride);

	const char* name() const;
	uint size() const;
	GLenum type() const;
	bool normalized() const;
	uint stride() const;

	class Builder {
		const char* m_name;
		uint m_size;
		GLenum m_type;
		bool m_normalized;
		uint m_stride;
	public:
		Builder& set_name(const char* name);
		Builder& set_size(uint size);
		Builder& set_type(GLenum type);
		Builder& set_normalized(bool normalized);
		Builder& set_stride(uint stride);

		VertexAttribute build();
	};
};

class ShaderProgram;
class Vertex;

template<class T>
struct is_uniformable : std::bool_constant<false> {};

template<> struct is_uniformable<double> : std::bool_constant<true> {};
template<> struct is_uniformable<float> : std::bool_constant<true> {};
template<> struct is_uniformable<int> : std::bool_constant<true> {};
template<> struct is_uniformable<uint> : std::bool_constant<true> {};
template<> struct is_uniformable<Vertex> : std::bool_constant<true> {};

template<class T>
concept IsUniformable = is_uniformable<T>::value;

template<IsUniformable T>
class Uniform {
	ShaderProgram& m_program;
	const char* m_name;
	int m_location;
	bool m_blank;

public:
	Uniform(ShaderProgram& program, const char* name, int location):
			m_program(program), m_name(name), m_location(location) {}

	Uniform(Uniform const& other):
			Uniform(other.m_program, other.m_name, other.m_location) {
	}

	static Uniform<T> blank(ShaderProgram& program, const char* name) {
		Uniform<T> uniform(program, name, -1);
		uniform.m_blank = true;
		return uniform;
	}

	void set_value(T const& value) {}

	T value() const {
		return T{};
	}

	ShaderProgram& program() const {
		return m_program;
	}

	void bind_program() const {
		program().bind();
	}

	const char* name() const {
		return m_name;
	}

	int location() const {
		return m_location;
	}

	bool is_blank() const {
		return m_blank;
	}

	Uniform<T> operator=(Uniform<T> const& other) {
		*this = other;
	}
};

class ShaderProgram {
	static ShaderProgram* boundInstance;

	const Object m_object;

	ShaderProgram(Object program);
public:
	ShaderProgram(ShaderProgram const&) = delete;
	~ShaderProgram();

	static ShaderProgram* get_bound_instance();

	void bind();
	bool is_bound() const;
	uint get_attribute_location(VertexAttribute const& attribute) const;

	bool has_uniform(const char* name) const;

	template<IsUniformable T>
	Uniform<T> uniform(const char* name) {
		int location = glGetUniformLocation(m_object, name);
		if (location < 0) {
			spdlog::warn("Could not find uniform with name `{}`", name);
			spdlog::warn("Returning a blank Uniform");
			return Uniform<T>::blank(*this, name);
		}
		return Uniform<T>(*this, name, location);
	}

	class Builder {
		Object m_object;
	public:
		Builder();

		ShaderProgram::Builder& attach_shader(Shader const& shader);
		ShaderProgram build() const;
	};

	template<IsUniformable T>
	friend class Uniform;
};

class VertexBuffer {
	using byte = char;
	
	const Object m_object;
	byte* m_buffer;
	byte* m_ptr;
	uint m_size;
	uint m_maxSize;
	std::string m_name;

public:
	enum class Usage : GLenum {
		STREAM = GL_STREAM_DRAW,
		STATIC = GL_STATIC_DRAW,
		DYNAMIC = GL_DYNAMIC_DRAW
	};

	VertexBuffer();
	virtual ~VertexBuffer();

	void bind();
	void add_attribute(VertexAttribute const& attribute);

	void buffer(const void* ptr, unsigned long size);
	void buffer(const void* ptr, unsigned long size, Usage usage);
	
	template<class T>
	void buffer(T const& data) {
		buffer(data, Usage::DYNAMIC);
	}

	template<class T>
	void buffer(T const& data, Usage usage) {
		data.buffer_to(*this, usage);
	}

	unsigned long size() const;

private:
	void ensure_capability(uint additionalSize, Usage usage);
};

class VertexArray {
	const Object m_object;
public:
	VertexArray();
	virtual ~VertexArray();

	void bind();
};

class Vertex {
public:
	float x, y, z;

	Vertex();
	Vertex(std::initializer_list<float> values);

	void buffer_to(VertexBuffer& buffer, VertexBuffer::Usage usage) const;
	
	// std::string to_string() const override;
};

} // tetragon::graphics

#endif // TETRAGON_GRAPHICS_SHADERS_HPP
