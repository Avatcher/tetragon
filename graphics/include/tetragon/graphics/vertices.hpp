#ifndef VERTICES_HPP
#define VERTICES_HPP

#include <initializer_list>
#include <glad/glad.h>

#include "definitions.hpp"

namespace tetragon::graphics {

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

        static constexpr size_t DEFAULT_BUFFER_SIZE = 32;

        const GLObject m_object;
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
        const GLObject m_object;
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

#endif //VERTICES_HPP
