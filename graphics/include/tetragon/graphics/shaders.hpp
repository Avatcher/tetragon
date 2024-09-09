#ifndef TETRAGON_GRAPHICS_SHADERS_HPP
#define TETRAGON_GRAPHICS_SHADERS_HPP

#include <glad/glad.h>
#include <initializer_list>
#include <string>

namespace tetragon::graphics {

using Object = GLuint;
using uint = unsigned int;

enum class ShaderType {
	VERTEX, FRAGMENT	
};

class Shader final {
	const ShaderType m_type;
	const Object m_object;
public:
	Shader(ShaderType type, const char* source);
	Shader(Shader const&) = delete;
	virtual ~Shader();

	[[nodiscard]] ShaderType get_type() const;

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

	[[nodiscard]] const char* name() const;
	[[nodiscard]] uint size() const;
	[[nodiscard]] GLenum type() const;
	[[nodiscard]] bool normalized() const;
	[[nodiscard]] uint stride() const;

	class Builder {
		const char* m_name = nullptr;
		uint m_size = 0;
		GLenum m_type = 0;
		bool m_normalized = false;
		uint m_stride = 0;
	public:
		Builder& set_name(const char* name);
		Builder& set_size(uint size);
		Builder& set_type(GLenum type);
		Builder& set_normalized(bool normalized);
		Builder& set_stride(uint stride);

		VertexAttribute build() const;
	};
};

class ShaderProgram;
class Vertex;

template<class>
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

	Uniform(ShaderProgram& program, const char* name, const int location, const bool blank):
			m_program(program), m_name(name), m_location(location), m_blank(blank) {}
public:
	Uniform(ShaderProgram& program, const char* name, const int location):
			Uniform(program, name, location, false) {}

	Uniform(Uniform const& other):
			Uniform(other.m_program, other.m_name, other.m_location, other.m_blank) {}

	static Uniform<T> blank(ShaderProgram& program, const char* name) {
		Uniform<T> uniform(program, name, -1);
		uniform.m_blank = true;
		return uniform;
	}

	void set_value(T const& value);

	T value() const;

	[[nodiscard]] ShaderProgram& program() const {
		return m_program;
	}

	void bind_program() const {
		program().bind();
	}

	[[nodiscard]] const char* name() const {
		return m_name;
	}

	[[nodiscard]] int location() const {
		return m_location;
	}

	[[nodiscard]] bool is_blank() const {
		return m_blank;
	}

	Uniform<T> operator=(Uniform<T> const& other) {
		*this = other;
	}
};

class ShaderProgram {
	static ShaderProgram* boundInstance;

	const Object m_object;

	explicit ShaderProgram(Object program);
public:
	ShaderProgram(ShaderProgram const&) = delete;
	~ShaderProgram();

	static ShaderProgram* get_bound_instance();

	void bind();
	[[nodiscard]] bool is_bound() const;
	[[nodiscard]] uint get_attribute_location(VertexAttribute const& attribute) const;

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

		Builder& attach_shader(Shader const& shader);
		[[nodiscard]] ShaderProgram build() const;
	};

	template<IsUniformable T>
	friend class Uniform;
};

class VertexBuffer final {
public:
	enum class Usage : GLenum {
		STREAM = GL_STREAM_DRAW,
		STATIC = GL_STATIC_DRAW,
		DYNAMIC = GL_DYNAMIC_DRAW
	};
private:
	using byte = char;

	static constexpr size_t DEFAULT_BUFFER_SIZE = 32;
	
	const Object m_object;
	byte* m_buffer;
	byte* m_ptr;
	uint m_size = 0;
	uint m_maxSize = DEFAULT_BUFFER_SIZE;
	std::string m_name;
	Usage m_usage;

public:
	VertexBuffer();
	explicit VertexBuffer(Usage usage);
	virtual ~VertexBuffer();

	[[nodiscard]] Usage usage() const;
	void set_usage(Usage usage);

	[[nodiscard]] unsigned long size() const;

	void bind() const;
	void add_attribute(VertexAttribute const& attribute);

	void buffer(const void* ptr, unsigned long size);

	template<class T>
	void buffer(T const& data) {
		data.buffer_to(*this);
	}

private:
	void ensure_capacity(uint additionalSize);
};

class VertexArray final {
	const Object m_object;
public:
	VertexArray();
	virtual ~VertexArray();

	void bind() const;
};

class Vertex {
public:
	float x, y, z;

	Vertex() = default;
	Vertex(std::initializer_list<float> values);

	void buffer_to(VertexBuffer& buffer) const;
	
	// std::string to_string() const override;
};

} // tetragon::graphics

#endif // TETRAGON_GRAPHICS_SHADERS_HPP
