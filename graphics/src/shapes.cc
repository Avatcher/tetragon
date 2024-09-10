#include <algorithm>
#include <utility>

#include "shapes.hpp"

namespace tetragon::graphics {

    Triangle::Triangle(Vector3 a, Vector3 b, Vector3 c):
            a(std::move(a)), b(std::move(b)), c(std::move(c)) {
    }

    void Triangle::buffer_to(VertexBuffer& buffer) const {
        buffer.buffer(a);
        buffer.buffer(b);
        buffer.buffer(c);
    }

    //     /*
//         3	2
//
//         1	4
//     */
//     Square::Square(Vertex firstCorner, Vertex secondCorner) {
//         auto [maxX, minX] = std::minmax(firstCorner.x, secondCorner.x);
//         auto [maxY, minY] = std::minmax(firstCorner.y, secondCorner.y);
//         firstCorner = { minX, minY };
//         secondCorner = { maxX, maxY };
//         const Vertex thirdCorner{ minX, maxY };
//         const Vertex fourthCorner{ maxX, minY };
//         a = Triangle(firstCorner, thirdCorner, fourthCorner);
//         b = Triangle(secondCorner, fourthCorner, thirdCorner);
//     }
//
//     const Vertex* Square::vertecies() const {
//         return a.vertecies();
//     }
//
//     unsigned int Square::vertex_count() const {
//         return a.vertex_count() + b.vertex_count();
//     }

} // tetragon::graphics
