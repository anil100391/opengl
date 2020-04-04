#ifndef _testfragmentshader_h_
#define _testfragmentshader_h_

#include "test.h"
#include "../vertexarray.h"
#include "../vertexbufferlayout.h"
#include "../vertexbuffer.h"
#include "../indexbuffer.h"
#include "../shader.h"
#include "../events/event.h"

#include <memory>

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class TestFragmentShader: public Test
{
public:

    TestFragmentShader();
    ~TestFragmentShader();

    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;
    virtual void OnEvent(Event &evt) override;

private:

    void Draw();

    std::unique_ptr<VertexArray>    _vao;
    std::unique_ptr<VertexBuffer>   _vbo;
    std::unique_ptr<IndexBuffer>    _ibo;
    std::unique_ptr<Shader>         _shader;

    // 2d coordinate params
    float                           _startX;
    float                           _startY;
    float                           _centerX;
    float                           _centerY;
    float                           _size;

    // window dimensions
    int                             _windowWidth  = 960;
    int                             _windowHeight = 540;
};

}

#endif // _testfragmentshader_h_
