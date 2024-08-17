#include <limits>
#include "graphics/primitives.hpp"

namespace {
double CONSTEXPR sqrtNewtonRaphson(double x, double curr, double prev) {
	return curr == prev
		? curr
		: sqrtNewtonRaphson(x, 0.5 * (curr + x / curr), curr);
}

double CONSTEXPR sqrt(double x) {
    return x >= 0 && x < std::numeric_limits<double>::infinity()
        ? sqrtNewtonRaphson(x, x, 0)
        : std::numeric_limits<double>::quiet_NaN();
}

} // <anonymous>

namespace tetragon::graphics {

CONSTEXPR Vector2::Vector2(float x, float y):
	x(x), y(y) {}

CONSTEXPR Vector3::Vector3(float x, float y, float z):
	Vector2(x, y), z(z) {}

CONSTEXPR Vector4::Vector4(float x, float y, float z, float w):
	Vector3(x, y, z), w(w) {}

#pragma region Vector2
CONSTEXPR float Vector2::length() const {
	return sqrt(length_squared());
}
CONSTEXPR float Vector2::length_squared() const {
	return x * x + y * y;
}

CONSTEXPR Vector2 Vector2::operator+(Scalar const& other) const {
	return Vector2 { x + other, y };
}
CONSTEXPR Vector2 Vector2::operator+(Vector2 const& other) const {
	return Vector2 { x + other.x, y + other.y };
}
CONSTEXPR Vector3 Vector2::operator+(Vector3 const& other) const {
	return Vector3 { x + other.x, y + other.y, other.z };
}
CONSTEXPR Vector4 Vector2::operator+(Vector4 const& other) const {
	return Vector4 { x + other.x, y + other.y, other.z, other.w };
}

CONSTEXPR Vector2 Vector2::operator-(Scalar const& other) const {
	return *this + -other;
}
CONSTEXPR Vector2 Vector2::operator-(Vector2 const& other) const {
	return *this + -other;
}
CONSTEXPR Vector3 Vector2::operator-(Vector3 const& other) const {
	return (*this + -other);
}
CONSTEXPR Vector4 Vector2::operator-(Vector4 const& other) const {
	return (*this + -other);
}

CONSTEXPR Vector2 Vector2::operator*(Scalar multiplier) const {
	return Vector2 { multiplier * x, multiplier * y };
}
CONSTEXPR Vector2 Vector2::operator*(Vector2 multiplier) const {
	return Vector2 { multiplier.x * x, multiplier.y * y };
}

CONSTEXPR Vector2 Vector2::operator/(Scalar divider) const {
	return Vector2 { x / divider, y / divider };
}
CONSTEXPR Vector2 Vector2::operator/(Vector2 divider) const {
	return Vector2 { x / divider.x, y / divider.y };
}

CONSTEXPR Vector2 Vector2::operator-() const {
	return Vector2 { -x, -y };
}
#pragma endregion

#pragma region Vector3
CONSTEXPR float Vector3::length() const {
	return sqrt(length_squared());
}
CONSTEXPR float Vector3::length_squared() const {
	return x * x + y * y + z * z;
}

CONSTEXPR Vector3 Vector3::operator+(Scalar const& other) const {
	return Vector3 { x + other, y, z };
}
CONSTEXPR Vector3 Vector3::operator+(Vector2 const& other) const {
	return Vector3 { x + other.x, y + other.y, z };
}
CONSTEXPR Vector3 Vector3::operator+(Vector3 const& other) const {
	return Vector3 { x + other.x, y + other.y, other.z };
}
CONSTEXPR Vector4 Vector3::operator+(Vector4 const& other) const {
	return Vector4 { x + other.x, y + other.y, other.z, other.w };
}

CONSTEXPR Vector3 Vector3::operator-(Scalar const& other) const {
	return *this + -other;
}
CONSTEXPR Vector3 Vector3::operator-(Vector2 const& other) const {
	return *this + -other;
}
CONSTEXPR Vector3 Vector3::operator-(Vector3 const& other) const {
	return (*this + -other);
}
CONSTEXPR Vector4 Vector3::operator-(Vector4 const& other) const {
	return (*this + -other);
}

CONSTEXPR Vector3 Vector3::operator*(Scalar multiplier) const {
	return Vector3 { multiplier * x, multiplier * y, multiplier * z };
}
CONSTEXPR Vector3 Vector3::operator*(Vector3 multiplier) const {
	return Vector3 { multiplier.x * x, multiplier.y * y, multiplier.z * z };
}

CONSTEXPR Vector3 Vector3::operator/(Scalar divider) const {
	return Vector3 { x / divider, y / divider, z / divider };
}
CONSTEXPR Vector3 Vector3::operator/(Vector3 divider) const {
	return Vector3 { x / divider.x, y / divider.y, z / divider.z };
}

CONSTEXPR Vector3 Vector3::operator-() const {
	return Vector3 { -x, -y, -z };
}
#pragma endregion

#pragma region Vector4
CONSTEXPR float Vector4::length() const {
	return sqrt(length_squared());
}
CONSTEXPR float Vector4::length_squared() const {
	return x * x + y * y + z * z + w * w;
}

CONSTEXPR Vector4 Vector4::operator+(Scalar const& other) const {
	return Vector4 { x + other, y, z, w };
}
CONSTEXPR Vector4 Vector4::operator+(Vector2 const& other) const {
	return Vector4 { x + other.x, y + other.y, z, w };
}
CONSTEXPR Vector4 Vector4::operator+(Vector3 const& other) const {
	return Vector4 { x + other.x, y + other.y, other.z, w };
}
CONSTEXPR Vector4 Vector4::operator+(Vector4 const& other) const {
	return Vector4 { x + other.x, y + other.y, other.z, other.w };
}

CONSTEXPR Vector4 Vector4::operator-(Scalar const& other) const {
	return *this + -other;
}
CONSTEXPR Vector4 Vector4::operator-(Vector2 const& other) const {
	return *this + -other;
}
CONSTEXPR Vector4 Vector4::operator-(Vector3 const& other) const {
	return (*this + -other);
}
CONSTEXPR Vector4 Vector4::operator-(Vector4 const& other) const {
	return (*this + -other);
}

CONSTEXPR Vector4 Vector4::operator*(Scalar multiplier) const {
	return Vector4 { multiplier * x, multiplier * y, multiplier * z, multiplier * w };
}
CONSTEXPR Vector4 Vector4::operator*(Vector4 multiplier) const {
	return Vector4 { multiplier.x * x, multiplier.y * y, multiplier.z * z, multiplier.w * w };
}

CONSTEXPR Vector4 Vector4::operator/(Scalar divider) const {
	return Vector4 { x / divider, y / divider, z / divider, w / divider };
}
CONSTEXPR Vector4 Vector4::operator/(Vector4 divider) const {
	return Vector4 { x / divider.x, y / divider.y, z / divider.z, w / divider.w };
}

CONSTEXPR Vector4 Vector4::operator-() const {
	return Vector4 { -x, -y, -z, -w };
}
#pragma endregion

CONSTEXPR Vector2 vec(float x, float y) { return Vector2 { x, y }; }
CONSTEXPR Vector3 vec(float x, float y, float z) { return Vector3 { x, y, z }; }
CONSTEXPR Vector4 vec(float x, float y, float z, float w) { return Vector4 { x, y, z, w }; }

} // tetragon::graphics
