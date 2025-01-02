#include "texture.h"
#include "renderer.h"
#include <stb_image.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Texture::Texture(const std::string& filePath)
    : _filePath(filePath)
{
    stbi_set_flip_vertically_on_load(1);
    auto localBuffer = stbi_load(filePath.c_str(), &_width, &_height, &_bpp, 4);

    glGenTextures(1, &_rendererID);
    glBindTexture(GL_TEXTURE_2D, _rendererID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, _format, _width, _height, 0, _format, GL_UNSIGNED_BYTE, localBuffer);
    glBindTexture(GL_TEXTURE_2D, 0);

    if ( localBuffer )
        stbi_image_free(localBuffer);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Texture::Texture( unsigned int width,
                  unsigned int height,
                  const std::vector<uint8_t> &colors )
    : _width( width ),
      _height( height ),
      _localBuffer( colors.data() )
{

    glGenTextures(1, &_rendererID);
    glBindTexture(GL_TEXTURE_2D, _rendererID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    auto num_channels = colors.size() / (width * height);
    switch ( num_channels )
    {
    case 1u:
        _format = GL_R;
        break;
    case 2u:
        _format = GL_RG;
        break;
    case 3u:
        _format = GL_RGB;
        break;
    case 4u:
        _format = GL_RGBA;
        break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, _format, _width, _height, 0, _format, GL_UNSIGNED_BYTE, colors.data());
    glBindTexture(GL_TEXTURE_2D, 0);
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

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Texture::UpdateTextureData( const std::vector<uint8_t> &colors )
{
    glBindTexture( GL_TEXTURE_2D, _rendererID );
    glTexImage2D(GL_TEXTURE_2D, 0, _format, _width, _height, 0, _format, GL_UNSIGNED_BYTE, colors.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}
