#ifndef SHAPES_HPP
#define SHAPES_HPP

#include "primitives.hpp"
#include "shaders.hpp"

namespace tetragon::graphics {

    class Shape {
    public:
        virtual ~Shape() = default;

        virtual void buffer_to(VertexBuffer& buffer) const = 0;
    };

    class Triangle final : public Shape {
    public:
        Vector3 a, b, c;

        Triangle() = default;
        Triangle(Vector3, Vector3, Vector3 );

        void buffer_to(VertexBuffer& buffer) const override;
    };

    // class Square final : public Shape {
    //     Triangle a, b;
    // public:
    //     Square(Vertex, Vertex);
    //
    //     [[nodiscard]] const Vertex* vertecies() const override;
    //     [[nodiscard]] unsigned int vertex_count() const override;
    // };

} // tetragon::graphics

#endif //SHAPES_HPP
