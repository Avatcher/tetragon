#ifndef VERTICES_HPP
#define VERTICES_HPP

#include <glad/glad.h>
#include <string>
#include <memory>

#include "definitions.hpp"

namespace tetragon::graphics {

    class VertexBuffer;

    struct Vertex {
        [[nodiscard]] const void* vertex_data() const;
        [[nodiscard]] std::size_t vertex_size() const;
    protected:
        Vertex() = default;
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

            [[nodiscard]] VertexAttribute build() const;
        };
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

        static constexpr size_t DEFAULT_BUFFER_SIZE = 8;

        const GLObject m_object;
        const std::size_t m_vertexSize;

        byte* m_buffer;
        byte* m_ptr;
        uint m_size = 0;
        uint m_maxSize = DEFAULT_BUFFER_SIZE * m_vertexSize;

        std::string m_name;
        Usage m_usage;

    public:
        explicit VertexBuffer(std::size_t vertexSize);
        VertexBuffer(std::size_t vertexSize, Usage usage);
        virtual ~VertexBuffer();

        [[nodiscard]] Usage usage() const;
        void set_usage(Usage usage);

        [[nodiscard]] std::size_t size() const;
        [[nodiscard]] std::size_t vertex_size() const;

        void bind() const;
        void add_attribute(VertexAttribute const& attribute);

        template<class T> requires std::is_base_of_v<Vertex, T>
        void buffer(T const& vertex) {
            buffer(vertex.vertex_data(), vertex.vertex_size());
        }

    private:
        void buffer(const void* ptr, unsigned long size);
        void ensure_capacity(uint additionalSize);
    };

    class VertexArray final {
        const GLObject m_object;
    public:
        VertexArray();
        virtual ~VertexArray();

        void bind() const;
    };
} // tetragon::graphics

#endif //VERTICES_HPP
