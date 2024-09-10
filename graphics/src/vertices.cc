#include <spdlog/spdlog.h>
#include <fmt/color.h>

#include "vertices.hpp"
#include "shaders.hpp"

namespace tetragon::graphics {

    namespace {
	GLObject create_vertex_buffer() {
		GLObject buffer;
		glGenBuffers(1, &buffer);
		return buffer;
	}

	GLObject create_vertex_array() {
		GLObject array;
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
