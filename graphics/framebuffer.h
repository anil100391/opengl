#ifndef _framebuffer_h_
#define _framebuffer_h_

#include "renderer.h"

class FrameBuffer
{
public:

    FrameBuffer();
    ~FrameBuffer();

    void Bind() const;
    void Unbind() const;

    void BindTexture() const;
    void UnbindTexture() const;
    unsigned int TextureID() const { return _textureId; }

private:

    unsigned int _frameBufferId     = 0;
    unsigned int _textureId         = 0;
    unsigned int _renderBufferId    = 0;
};

#endif // _framebuffer_h_
