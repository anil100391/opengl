#include "texture.h"
#include "renderer.h"
#include "vendor/stb_image/stb_image.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Texture::Texture(const std::string& filePath)
    : _filePath(filePath)
{
    stbi_set_flip_vertically_on_load(1);
    _localBuffer = stbi_load(filePath.c_str(), &_width, &_height, &_bpp, 4);

    glGenTextures(1, &_rendererID);
    glBindTexture(GL_TEXTURE_2D, _rendererID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _localBuffer);
    glBindTexture(GL_TEXTURE_2D, 0);

    if ( _localBuffer )
        stbi_image_free(_localBuffer);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Texture::~Texture()
{
    glDeleteTextures(1, &_rendererID);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Texture::Bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, _rendererID);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
