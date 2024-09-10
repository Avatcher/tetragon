#ifndef TETRAGON_GRAPHICS_SHADERS_HPP
#define TETRAGON_GRAPHICS_SHADERS_HPP

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include "vertices.hpp"

namespace tetragon::graphics {

struct Vector3;

enum class ShaderType {
	VERTEX, FRAGMENT	
};

class Shader final {
	const ShaderType m_type;
	const GLObject m_object;
public:
	Shader(ShaderType type, const char* source);
	Shader(Shader const&) = delete;
	virtual ~Shader();

	[[nodiscard]] ShaderType get_type() const;

	friend class ShaderProgram;
};

class ShaderProgram;

template<class>
struct is_uniformable : std::bool_constant<false> {};

template<> struct is_uniformable<double> : std::bool_constant<true> {};
template<> struct is_uniformable<float> : std::bool_constant<true> {};
template<> struct is_uniformable<int> : std::bool_constant<true> {};
template<> struct is_uniformable<uint> : std::bool_constant<true> {};

template<> struct is_uniformable<Vector3> : std::bool_constant<true> {};

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

	const GLObject m_object;

	explicit ShaderProgram(GLObject program);
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
		GLObject m_object;
	public:
		Builder();

		Builder& attach_shader(Shader const& shader);
		[[nodiscard]] ShaderProgram build() const;
	};

	template<IsUniformable T>
	friend class Uniform;
};

} // tetragon::graphics

#endif // TETRAGON_GRAPHICS_SHADERS_HPP
