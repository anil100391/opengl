#include "cubemap.h"
#include "renderer.h"
#include <stb_image.h>
#include <cassert>
#include <iostream>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CubeMap::CubeMap(const std::vector<std::string>& textureFiles)
{
    stbi_set_flip_vertically_on_load(false);
    assert(textureFiles.size() == 6);

    glGenTextures(1, &_rendererID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _rendererID);

    unsigned char* buffers[6] = {nullptr};
    int width    = 0;
    int height   = 0;
    int channels = 0;

    for ( int ii = 0; ii < 6; ++ii )
    {
        buffers[ii] = stbi_load(textureFiles[ii].c_str(), &width,
                                &height, &channels, 0);
        if ( buffers[ii] )
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + ii,
                         0, GL_RGB, width, height, 0, GL_RGB,
                         GL_UNSIGNED_BYTE, buffers[ii]);
            stbi_image_free(buffers[ii]);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: "
                      << textureFiles[ii] << "\n";
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CubeMap::~CubeMap()
{
    glDeleteTextures(1, &_rendererID);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void CubeMap::Bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _rendererID);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void CubeMap::Unbind() const
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
