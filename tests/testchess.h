#ifndef _testchess_h_
#define _testchess_h_

#include "test.h"
#include <memory>
#include "../vertexarray.h"
#include "../vertexbufferlayout.h"
#include "../vertexbuffer.h"
#include "../indexbuffer.h"
#include "../shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class TestChess : public Test
{
public:

    TestChess(Application *app);
    virtual ~TestChess();

    virtual void OnUpdate(float deltaTime) override {}
    virtual void OnRender() override;
    virtual void OnImGuiRender() override {}
    virtual void OnEvent(Event &evt) override {}

private:

    std::unique_ptr<VertexArray>    _vao;
    std::unique_ptr<VertexBuffer>   _vbo;
    std::unique_ptr<IndexBuffer>    _ibo;
    std::unique_ptr<Shader>         _shader;

    glm::mat4                       _viewMat;
    glm::mat4                       _projMat;
};

}

#endif // _testchess_h_
