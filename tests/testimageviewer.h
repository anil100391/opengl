
#ifndef _testimageviewer_h_
#define _testiamgeviewer_h_

#include "test.h"
#include "../vertexarray.h"
#include "../vertexbufferlayout.h"
#include "../vertexbuffer.h"
#include "../indexbuffer.h"
#include "../shader.h"
#include "../texture.h"
#include "../utils/meshgl.h"
#include "../events/event.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imfilebrowser.h"

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class TestImageViewer : public Test
{
public:

    TestImageViewer(Application *app);
    ~TestImageViewer();

    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;
    void OnEvent(Event& evt) override;

private:

    void GenerateGLBuffers();
    void GenerateTexture(const std::string& image);

    std::unique_ptr<MeshGL>    _quadGL;
    std::unique_ptr<Shader>    _shader;
    std::unique_ptr<Texture>   _texture;

    glm::mat4                  _viewMat;
    glm::mat4                  _projMat;
    float                      _zoomlevel = 1.0f;

    ImGui::FileBrowser         _fileDialog;
};

}

#endif // _testiamgeviewer_h_
