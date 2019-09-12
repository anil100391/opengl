#ifndef _testfragmentshader_h_
#define _testfragmentshader_h_

#include "test.h"
#include "../vertexarray.h"
#include "../vertexbufferlayout.h"
#include "../vertexbuffer.h"
#include "../indexbuffer.h"
#include "../shader.h"

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

private:

    std::unique_ptr<VertexArray>    _vao;
    std::unique_ptr<VertexBuffer>   _vbo;
    std::unique_ptr<IndexBuffer>    _ibo;
    std::unique_ptr<Shader>         _shader;

    float                           _startX;
    float                           _startY;
};

}

#endif // _testfragmentshader_h_
