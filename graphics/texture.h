#ifndef _texture_h_
#define _texture_h_

#include <string>
#include <vector>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class Texture
{
public:

    Texture( const std::string& path );

    Texture( unsigned int width,
             unsigned int height,
             const std::vector<uint8_t> &colors );

    ~Texture();

    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

    void UpdateTextureData( const std::vector<uint8_t> &colors );

    inline int GetWidth() const
    {
        return _width;
    }

    inline int GetHeight() const
    {
        return _height;
    }

private:

    unsigned int    _rendererID = 0;
    std::string     _filePath;
    const uint8_t*  _localBuffer = nullptr;
    int             _width = -1;
    int             _height = -1;
    int             _bpp = -1;
    unsigned int    _format = -1;
};

#endif // _texture_h_
