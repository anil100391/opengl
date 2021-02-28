#ifndef _test_terrain_h_
#define _test_terrain_h_

#include "test.h"
#include "../vertexarray.h"
#include "../vertexbufferlayout.h"
#include "../vertexbuffer.h"
#include "../indexbuffer.h"
#include "../shader.h"
#include "../camera.h"
#include "../texture.h"
#include "../utils/mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <vector>
#include <string>
#include <utility>
#include <iostream>

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class TestLorenz : public Test
{
public:

    TestLorenz(Application *app);
    virtual ~TestLorenz();

    virtual void OnUpdate(float deltaTime) override;
    virtual void OnRender() override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Event &evt) override;

private:

    void CreateLorenzGLBuffers();

    std::unique_ptr<VertexArray>    _vao;
    std::unique_ptr<VertexBuffer>   _vbo;
    std::unique_ptr<IndexBuffer>    _ibo;
    std::unique_ptr<Shader>         _shader;

    Camera                          _camera;

    glm::mat4                       _viewMat;
    glm::mat4                       _projMat;

    int                             _iterations = 2000;
};

}

#endif // _test_terrain_h_