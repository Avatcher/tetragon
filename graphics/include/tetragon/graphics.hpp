#ifndef TETRAGON_APPLICATION_H
#define TETRAGON_APPLICATION_H

#include <GLFW/glfw3.h>
#include <exception>
#include <thread>
#include <mutex>
#include <vector>
#include <type_traits>
#include <concepts>

#include "graphics/primitives.hpp"
#include "graphics/shaders.hpp"

namespace tetragon::graphics {

class Shape {
public:
	virtual const Vertex* vertecies() const = 0;
	virtual unsigned int vertex_count() const = 0;
	virtual void buffer_to(VertexBuffer& buffer, VertexBuffer::Usage usage) const = 0;
};

class Triangle : public Shape {
public:
	Vertex a, b, c;

	Triangle();
	Triangle(Vertex, Vertex, Vertex);

	const Vertex* vertecies() const override;
	unsigned int vertex_count() const override;
	void buffer_to(VertexBuffer& buffer, VertexBuffer::Usage usage) const override;
};

class Square : public Shape {
	Triangle a, b;
public:
	Square(Vertex, Vertex);

	const Vertex* vertecies() const override;
	unsigned int vertex_count() const override;
	void buffer_to(VertexBuffer& buffer, VertexBuffer::Usage usage) const override;
};

} // tetragon

#endif // TETRAGON_APPLICATION_H
