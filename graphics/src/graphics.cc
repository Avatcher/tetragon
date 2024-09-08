#define GL_GLEXT_PROTOTYPES

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/ranges.h>
#include <cstdio>
#include <map>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include "graphics.hpp"

namespace tetragon::graphics {

Triangle::Triangle() {}

Triangle::Triangle(Vertex a, Vertex b, Vertex c):
		a(a), b(b), c(c) {
}

const Vertex* Triangle::vertecies() const {
	return &a;
}

unsigned int Triangle::vertex_count() const {
	return 3;
}

void Triangle::buffer_to(VertexBuffer &buffer,
							VertexBuffer::Usage usage) const {
	a.buffer_to(buffer, usage);
	b.buffer_to(buffer, usage);
	c.buffer_to(buffer, usage);
}

/*
	3	2
	
	1	4
*/
Square::Square(Vertex firstCorner, Vertex secondCorner) {
	auto [maxX, minX] = std::minmax(firstCorner.x, secondCorner.x);
	auto [maxY, minY] = std::minmax(firstCorner.y, secondCorner.y);
	firstCorner = { minX, minY };
	secondCorner = { maxX, maxY };
	Vertex thirdCorner{ minX, maxY };
	Vertex fourthCorner{ maxX, minY };
	a = Triangle(firstCorner, thirdCorner, fourthCorner);
	b = Triangle(secondCorner, fourthCorner, thirdCorner);
}

const Vertex* Square::vertecies() const {
	return a.vertecies();
}

unsigned int Square::vertex_count() const {
	return a.vertex_count() + b.vertex_count();
}

void Square::buffer_to(VertexBuffer& buffer, VertexBuffer::Usage usage) const {
	a.buffer_to(buffer, usage);
	b.buffer_to(buffer, usage);
}

VertexAttribute::VertexAttribute(const char* name, uint size, GLenum type,
			bool normalized, uint stride):
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

VertexAttribute VertexAttribute::Builder::build() {
	return VertexAttribute (
		m_name, m_size, m_type, m_normalized, m_stride
	);
}

} // namespace tetragon
