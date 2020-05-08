#ifndef _testobjloader_h_
#define _testobjloader_h_

#include "test.h"
#include "../vertexarray.h"
#include "../vertexbufferlayout.h"
#include "../vertexbuffer.h"
#include "../indexbuffer.h"
#include "../shader.h"
#include "../texture.h"
#include "../utils/mesh.h"
#include "../light.h"
#include "../camera.h"

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
class TestObjLoader : public Test
{
public:

    TestObjLoader(Application *app);
    virtual ~TestObjLoader();

    virtual void OnUpdate(float deltaTime) override;
    virtual void OnRender() override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Event &evt) override;

private:

    void         CreateMeshGLBuffers();

    void         Select(int x, int y);

    void         SetUpCamera() noexcept;

    const material& GetMaterial() const;
    void            SetMaterial(const material& m);
    light           GetLight() const;

    mesh                            _mesh;
    std::unique_ptr<VertexArray>    _vao;
    std::unique_ptr<VertexBuffer>   _vbo;
    std::unique_ptr<IndexBuffer>    _ibo;
    std::unique_ptr<Shader>         _shader;
    std::unique_ptr<Shader>         _selectShader;
    std::unique_ptr<Texture>        _texture;

    glm::mat4                       _viewMat;
    glm::mat4                       _projMat;

    material                        _material;
    Camera                          _camera;

    double                          _time;
    bool                            _flatShading = true;
};

}

#endif // _testobjloader_h_
