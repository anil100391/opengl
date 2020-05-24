#ifndef _cubemap_h_
#define _cubemap_h_

#include <vector>
#include <string>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class CubeMap
{
public:

    CubeMap( const std::vector<std::string>& textureFiles );
    ~CubeMap();

    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

private:

    unsigned int    _rendererID = 0;
};

#endif // _cubemap_h_
