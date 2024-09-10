#include <algorithm>

#include "graphics/shapes.hpp"

namespace tetragon::graphics {

    Triangle::Triangle(const Vertex a, const Vertex b, const Vertex c):
            a(a), b(b), c(c) {
    }

    const Vertex* Triangle::vertecies() const {
        return &a;
    }

    unsigned int Triangle::vertex_count() const {
        return 3;
    }

    void Triangle::buffer_to(VertexBuffer &buffer) const {
        a.buffer_to(buffer);
        b.buffer_to(buffer);
        c.buffer_to(buffer);
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
        const Vertex thirdCorner{ minX, maxY };
        const Vertex fourthCorner{ maxX, minY };
        a = Triangle(firstCorner, thirdCorner, fourthCorner);
        b = Triangle(secondCorner, fourthCorner, thirdCorner);
    }

    const Vertex* Square::vertecies() const {
        return a.vertecies();
    }

    unsigned int Square::vertex_count() const {
        return a.vertex_count() + b.vertex_count();
    }

    void Square::buffer_to(VertexBuffer& buffer) const {
        a.buffer_to(buffer);
        b.buffer_to(buffer);
    }

} // tetragon::graphics
