#ifndef TETRAGON_GRAPHICS_PRIMITIVES_HPP
#define TETRAGON_GRAPHICS_PRIMITIVES_HPP

#ifdef CXX_CONSTEXPR
#define CONSTEXPR constexpr
#else
#define CONSTEXPR
#endif
#include "vertices.hpp"

namespace tetragon::graphics {

struct Vector2;
struct Vector3;
struct Vector4;

using Scalar = float;

struct Vector2 {
	float x = 0, y = 0;

	CONSTEXPR Vector2() = default;
	CONSTEXPR Vector2(float x, float y);

	CONSTEXPR [[nodiscard]] float length() const;
	CONSTEXPR [[nodiscard]] float length_squared() const;

	CONSTEXPR Vector2 operator+(Scalar const& other) const;
	CONSTEXPR Vector2 operator+(Vector2 const& other) const;
	CONSTEXPR Vector3 operator+(Vector3 const& other) const;
	CONSTEXPR Vector4 operator+(Vector4 const& other) const;

	CONSTEXPR Vector2 operator-(Scalar const& other) const;
	CONSTEXPR Vector2 operator-(Vector2 const& other) const;
	CONSTEXPR Vector3 operator-(Vector3 const& other) const;
	CONSTEXPR Vector4 operator-(Vector4 const& other) const;

	CONSTEXPR Vector2 operator*(Scalar multiplier) const;
	CONSTEXPR Vector2 operator*(Vector2 multiplier) const;

	CONSTEXPR Vector2 operator/(Scalar divider) const;
	CONSTEXPR Vector2 operator/(Vector2 divider) const;

	CONSTEXPR Vector2 operator-() const;

	CONSTEXPR operator Vector3() const;
};

struct Vector3 : Vector2, Vertex {
	float z = 0;

	CONSTEXPR Vector3() = default;
	CONSTEXPR Vector3(float x, float y, float z);

	[[nodiscard]] const void* data() const override;
	[[nodiscard]] std::size_t size() const override;

	CONSTEXPR [[nodiscard]] float length() const;
	CONSTEXPR [[nodiscard]] float length_squared() const;

	CONSTEXPR Vector3 operator+(Scalar const& other) const;
	CONSTEXPR Vector3 operator+(Vector2 const& other) const;
	CONSTEXPR Vector3 operator+(Vector3 const& other) const;
	CONSTEXPR Vector4 operator+(Vector4 const& other) const;

	CONSTEXPR Vector3 operator-(Scalar const& other) const;
	CONSTEXPR Vector3 operator-(Vector2 const& other) const;
	CONSTEXPR Vector3 operator-(Vector3 const& other) const;
	CONSTEXPR Vector4 operator-(Vector4 const& other) const;

	CONSTEXPR Vector3 operator*(Scalar multiplier) const;
	CONSTEXPR Vector3 operator*(Vector3 multiplier) const;

	CONSTEXPR Vector3 operator/(Scalar divider) const;
	CONSTEXPR Vector3 operator/(Vector3 divider) const;

	CONSTEXPR Vector3 operator-() const;
};

struct Vector4 : Vector3 {
	float w = 0;

	CONSTEXPR Vector4() = default;
	CONSTEXPR Vector4(float x, float y, float z, float w);

	CONSTEXPR [[nodiscard]] float length() const;
	CONSTEXPR [[nodiscard]] float length_squared() const;

	CONSTEXPR Vector4 operator+(Scalar const& other) const;
	CONSTEXPR Vector4 operator+(Vector2 const& other) const;
	CONSTEXPR Vector4 operator+(Vector3 const& other) const;
	CONSTEXPR Vector4 operator+(Vector4 const& other) const;

	CONSTEXPR Vector4 operator-(Scalar const& other) const;
	CONSTEXPR Vector4 operator-(Vector2 const& other) const;
	CONSTEXPR Vector4 operator-(Vector3 const& other) const;
	CONSTEXPR Vector4 operator-(Vector4 const& other) const;

	CONSTEXPR Vector4 operator*(Scalar multiplier) const;
	CONSTEXPR Vector4 operator*(Vector4 multiplier) const;

	CONSTEXPR Vector4 operator/(Scalar divider) const;
	CONSTEXPR Vector4 operator/(Vector4 divider) const;

	CONSTEXPR Vector4 operator-() const;
};

CONSTEXPR Vector2 vec(float x, float y);
CONSTEXPR Vector3 vec(float x, float y, float z);
CONSTEXPR Vector4 vec(float x, float y, float z, float w);

} // tetragon::graphics

#endif // TETRAGON_GRAPHICS_PRIMITIVES_HPP
