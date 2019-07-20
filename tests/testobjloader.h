#ifndef _testobjloader_h_
#define _testobjloader_h_

#include "test.h"
#include "../vertexarray.h"
#include "../vertexbufferlayout.h"
#include "../vertexbuffer.h"
#include "../indexbuffer.h"
#include "../shader.h"
#include "../texture.h"

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

    TestObjLoader();
    virtual ~TestObjLoader();

    virtual void OnUpdate(float deltaTime) override;
    virtual void OnRender() override;
    virtual void OnImGuiRender() override;

private:

    std::string                     _filename;
    std::unique_ptr<VertexArray>    _vao;
    std::unique_ptr<VertexBuffer>   _vbo;
    std::unique_ptr<IndexBuffer>    _ibo;
    std::unique_ptr<Shader>         _shader;

    glm::mat4                       _viewMat;
    glm::mat4                       _projMat;
    glm::vec3                       _modelLocation;
    float                           _modelRotation = 0.0f;
    float                           _modelScale = 1.0f;
};

}

#endif // _testobjloader_h_
