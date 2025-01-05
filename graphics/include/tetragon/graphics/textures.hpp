#ifndef TEXTURES_HPP
#define TEXTURES_HPP
#include <glad/glad.h>
#include <string>
#include <iostream>

#include "definitions.hpp"

namespace tetragon::graphics {

enum class TextureWrapping {
    REPEAT = GL_REPEAT,
    MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
    CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
    CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER
};

enum class TextureFiltering {
    NEAREST = GL_NEAREST,
    LINEAR = GL_LINEAR
};

class Texture {
    const GLObject m_object;
    const int m_width, m_height;
    int m_unit;
public:
    static Texture from_file(std::string const& path);
    explicit Texture(const unsigned char* data, int width, int height);

    [[nodiscard]] int unit() const;

    void bind();
    void bind(int unit);
    void set_wrapping(TextureWrapping wrapping);
    void set_downscaling(TextureFiltering mode);
    void set_upscaling(TextureFiltering mode);
};

} // tetragon::graphics

#endif //TEXTURES_HPP
