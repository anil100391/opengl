#include "framebuffer.h"
#include <iostream>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
FrameBuffer::FrameBuffer()
{
    glGenFramebuffers(1, &_frameBufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferId);

    // create color attachment. choosing a 2d texture for it... could have been
    // a renderbuffer as well
    int w = 1920;
    int h = 1080;
    glGenTextures(1, &_textureId);
    glBindTexture(GL_TEXTURE_2D, _textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
    // attach this texture to the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textureId, 0);

    // create a depth and stencil attachement. choosing a single renderbuffer for
    // both of it
    glGenRenderbuffers(1, &_renderBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, _renderBufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);

    // attach the render buffer to the framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _renderBufferId);

    // Sanity check
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::INCOMPLE FRAMEBUFFER!\n";
    }

    // unbind the framebuffer so that we don't accidently draw to it
    Unbind();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &_frameBufferId);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void FrameBuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferId);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void FrameBuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void FrameBuffer::BindTexture() const
{
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, _textureId);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void FrameBuffer::UnbindTexture() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
