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
#include "../utils/meshgl.h"
#include "../light.h"
#include "../camera.h"
#include "../cubemap.h"

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
    void         CreateEnvironmentGLBuffers();

    void         Select(int x, int y);

    void         SetUpCamera() noexcept;

    const material& GetMaterial() const;
    void            SetMaterial(const material& m);
    pointlight      GetLight() const;

    mesh                            _mesh;
    std::unique_ptr<MeshGL>         _glMesh;
    std::unique_ptr<Shader>         _shader;
    std::unique_ptr<Shader>         _selectShader;
    std::unique_ptr<Texture>        _texture;

    glm::mat4                       _viewMat;
    glm::mat4                       _projMat;

    material                        _material;
    Camera                          _camera;

    // enviroment texture
    std::unique_ptr<MeshGL>         _envglMesh;
    std::unique_ptr<CubeMap>        _cubemapTexture;
    std::unique_ptr<Shader>         _cubemapShader;

    double                          _time = 0.0;
    bool                            _flatShading = true;
    bool                            _paused = true;

    ImGui::FileBrowser              _fileDialog;
};

}

#endif // _testobjloader_h_
