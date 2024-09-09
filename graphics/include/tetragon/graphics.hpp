#ifndef TETRAGON_APPLICATION_H
#define TETRAGON_APPLICATION_H

#include "graphics/shaders.hpp"

namespace tetragon::graphics {

class Shape {
public:
	virtual ~Shape() = default;

	[[nodiscard]] virtual const Vertex* vertecies() const = 0;
	[[nodiscard]] virtual unsigned int vertex_count() const = 0;
	virtual void buffer_to(VertexBuffer& buffer) const = 0;
};

class Triangle final : public Shape {
public:
	Vertex a, b, c;

	Triangle();
	Triangle(Vertex, Vertex, Vertex);

	[[nodiscard]] const Vertex* vertecies() const override;
	[[nodiscard]] unsigned int vertex_count() const override;
	void buffer_to(VertexBuffer& buffer) const override;
};

class Square final : public Shape {
	Triangle a, b;
public:
	Square(Vertex, Vertex);

	[[nodiscard]] const Vertex* vertecies() const override;
	[[nodiscard]] unsigned int vertex_count() const override;
	void buffer_to(VertexBuffer& buffer) const override;
};

} // tetragon

#endif // TETRAGON_APPLICATION_H
