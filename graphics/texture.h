#ifndef _texture_h_
#define _texture_h_

#include <string>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class Texture
{
public:

    Texture( const std::string& path );
    ~Texture();

    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

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
    unsigned char*  _localBuffer = nullptr;
    int             _width = -1;
    int             _height = -1;
    int             _bpp = -1;
};

#endif // _texture_h_
