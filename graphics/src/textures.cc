#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>

#include "definitions.hpp"
#include "textures.hpp"

#include <fmt/format.h>

namespace {
    tetragon::graphics::GLObject genTextureObject() {
        tetragon::graphics::GLObject obj;
        glGenTextures(1, &obj);
        return obj;
    }
}

namespace tetragon::graphics {

Texture Texture::from_file(std::string const& path) {
    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (!data) {
        throw std::runtime_error("Could not load texture from file " + path);
    }
    const Texture texture(data, width, height);
    stbi_image_free(data);
    return texture;
}

Texture::Texture(const unsigned char* data, const int width, const int height):
        m_object(genTextureObject()), m_width(width), m_height(height) {
    bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

int Texture::unit() const {
    return m_unit;
}

void Texture::bind() {
    glBindTexture(GL_TEXTURE_2D, m_object);
}

void Texture::bind(const int unit) {
    int glMaxTextureUnits;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &glMaxTextureUnits);
    if (unit < 0 || unit > glMaxTextureUnits) {
        throw std::invalid_argument(fmt::format("Tried to assign a Texture Unit `{}` outside the limit `0-{}`", unit, glMaxTextureUnits));
    }
    const GLenum glUnit = GL_TEXTURE0 + unit;
    glActiveTexture(glUnit);
    bind();
    m_unit = unit;
}

void Texture::set_wrapping(TextureWrapping wrapping) {
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping));
}

void Texture::set_downscaling(TextureFiltering mode) {
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(mode));
}

void Texture::set_upscaling(TextureFiltering mode) {
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(mode));
}

} // tetragon::graphics
