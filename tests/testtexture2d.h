#ifndef _testtexture2d_h_
#define _testtexture2d_h_

#include "test.h"
#include "../vertexarray.h"
#include "../vertexbufferlayout.h"
#include "../vertexbuffer.h"
#include "../indexbuffer.h"
#include "../shader.h"
#include "../texture.h"
#include "../utils/meshgl.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class TestTexture2D : public Test
{
public:

    TestTexture2D(Application *app);
    ~TestTexture2D();

    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;

private:

    std::unique_ptr<MeshGL>    _quadGL;
    std::unique_ptr<Shader>         _shader;
    std::unique_ptr<Texture>        _texture;

    glm::mat4                       _viewMat;
    glm::mat4                       _projMat;

    glm::vec3                       _translationA;
    glm::vec3                       _translationB;

};

}

#endif // _testtexture2d_h_
