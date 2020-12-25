#ifndef _testframebuffer_h_
#define _testframebuffer_h_

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
#include "../framebuffer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <vector>
#include <string>
#include <utility>
#include <iostream>

#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imfilebrowser.h"

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class TestFrameBuffer : public Test
{
public:

    TestFrameBuffer(Application *app);
    virtual ~TestFrameBuffer();

    virtual void OnUpdate(float deltaTime) override;
    virtual void OnRender() override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Event &evt) override;

private:

    void         CreateMeshGLBuffers();
    void         CreateFrameBufferGLBuffers();

    void         SetUpCamera() noexcept;

    const material& GetMaterial() const;
    void            SetMaterial(const material& m);
    pointlight      GetLight() const;

    mesh                            _mesh;
    std::unique_ptr<VertexArray>    _vao;
    std::unique_ptr<VertexBuffer>   _vbo;
    std::unique_ptr<IndexBuffer>    _ibo;
    std::unique_ptr<Shader>         _shader;
    std::unique_ptr<Texture>        _texture;

    // frame buffer gl objects
    std::unique_ptr<VertexArray>    _fbvao;
    std::unique_ptr<VertexBuffer>   _fbvbo;
    std::unique_ptr<IndexBuffer>    _fbibo;
    std::unique_ptr<Shader>         _fbshader;

    std::unique_ptr<FrameBuffer>    _framebuffer;

    glm::mat4                       _viewMat;
    glm::mat4                       _projMat;

    material                        _material;
    Camera                          _camera;

    double                          _time;
    bool                            _flatShading = true;

    ImGui::FileBrowser              _fileDialog;
};

}

#endif // _testframebuffer_h_
