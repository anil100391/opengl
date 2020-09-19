#ifndef _testgame_h_
#define _testgame_h_

#include "test.h"
#include <memory>
#include "../vertexarray.h"
#include "../vertexbufferlayout.h"
#include "../vertexbuffer.h"
#include "../indexbuffer.h"
#include "../shader.h"
#include "../camera.h"
#include "../utils/mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class TestGame : public Test
{
public:

    TestGame(Application *app);
    virtual ~TestGame();

    virtual void OnUpdate(float deltaTime) override {}
    virtual void OnRender() override;
    virtual void OnImGuiRender() override {}
    virtual void OnEvent(Event &evt) override;

private:

    void SetupCamera();
    void SetupTerrain();

    glm::vec4                       _color;
    std::unique_ptr<VertexArray>    _vao;
    std::unique_ptr<VertexBuffer>   _vbo;
    std::unique_ptr<IndexBuffer>    _ibo;
    std::unique_ptr<Shader>         _shader;

    mesh                            _mesh;
    Camera                          _camera;

};

}

#endif // _testgame_h_
