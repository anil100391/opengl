#include "testimageviewer.h"
#include "../app.h"
#include "../renderer.h"
#include <imgui.h>

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestImageViewer::TestImageViewer(Application *app)
    : Test(app),
      _viewMat(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
      _projMat(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f))
{
    _fileDialog.SetTitle("Open Image");
    _fileDialog.SetTypeFilters({".jpg", ".png", ".jpeg"});

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_DEPTH_TEST);

    _shader = std::make_unique<Shader>("res/shaders/imageview.shader");
    _shader->Bind();

    GenerateTexture("res/textures/sample.jpg");

    _shader->SetUniform1i("u_Texture", 0);
    _shader->SetUniform4f("u_Color", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestImageViewer::~TestImageViewer()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestImageViewer::OnUpdate(float deltaTime)
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestImageViewer::OnRender()
{
    Renderer renderer;
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT);

    if ( !_texture || !_quadGL )
        return;

    glm::mat4 mvp = _projMat * _viewMat;

    _shader->SetUniformMat4f("u_MVP", mvp);

    _shader->Bind();
    renderer.Draw(*_quadGL->vao(), *_quadGL->ibo(), *_shader);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestImageViewer::OnImGuiRender()
{
    if ( ImGui::Button("Open Image") )
    {
        _fileDialog.Open();
    }

    _fileDialog.Display();
    if ( _fileDialog.HasSelected() )
    {
        auto file = _fileDialog.GetSelected().string();
        _fileDialog.ClearSelected();
        GenerateTexture(file);
    }

    Test::OnImGuiRender();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestImageViewer::GenerateGLBuffers()
{
    if (!_texture)
        return;

    int w = 512;
    int h = 512;

    _app->GetWindowSize(w, h);

    float windowAspectRatio = 1.0f * w / h;

    int imgWidth = _texture->GetWidth();
    int imgHeight = _texture->GetHeight();

    float imgAspectRatio = 1.0f * imgWidth / imgHeight;

    float scale = 1.0f;

    float dx = 0.0f;
    float dy = 0.0f;
    if ( imgAspectRatio > windowAspectRatio )
    {
        // imgWidth maps to windowWidth
        scale = 1.0f * w / imgWidth;
        dy = (h - scale * imgHeight) / 2.0f;
        dy /= (scale * imgHeight);
    }
    else
    {
        // imgHeight maps to windowHeight
        scale = 1.0f * h / imgHeight;
        dx = (w - scale * imgWidth) / 2.0f;
        dx /= (scale * imgWidth);
    }

    // texture coordinates
    float tc[8] = { 0.0f - dx, 0.0f - dy,
                    1.0f + dx, 0.0f - dy,
                    1.0f + dx, 1.0f + dy,
                    0.0f - dx, 1.0f + dy };

    std::vector<float> positions = { 0.0f,     0.0f,     tc[0], tc[1],
                                     1.0f * w, 0.0f,     tc[2], tc[3],
                                     1.0f * w, 1.0f * h, tc[4], tc[5],
                                     0.0f,     1.0f * h, tc[6], tc[7] };

    std::vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0 };

    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);

    _quadGL = std::make_unique<MeshGL>( positions, layout, indices );

    _projMat = glm::ortho(0.0f, _zoomlevel * w, 0.0f, _zoomlevel * h, -1.0f, 1.0f);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestImageViewer::GenerateTexture(const std::string& image)
{
    _texture = std::make_unique<Texture>(image);
    _texture->Bind(0);

    GenerateGLBuffers();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestImageViewer::OnEvent(Event& evt)
{
    auto evtType = evt.GetEventType();

    switch (evtType)
    {
    case EventType::MouseMoved:
    {
        break;
    }
    case EventType::MouseButtonPressed:
    {
        break;
    }
    case EventType::MouseButtonReleased:
    {
        break;
    }
    case EventType::MouseScrolled:
    {
        int w = 512;
        int h = 512;

        _app->GetWindowSize(w, h);

        auto& mouseEvt = static_cast<MouseScrollEvent&>(evt);

        int x = mouseEvt.X();
        int y = mouseEvt.Y();

        float u = 1.0f * x / w;
        float v = 1.0f * y / h;

        _zoomlevel += static_cast<float>(0.1 * mouseEvt.YOffset());
        float newWidth = _zoomlevel * w;
        float newHeight = _zoomlevel * h;

        int dx = static_cast<int>(newWidth * u - x);
        int dy = static_cast<int>(newHeight * v - y);

        _projMat = glm::ortho(0.0f - dx, newWidth - dx, 0.0f - dy, newHeight - dy, -1.0f, 1.0f);

        break;
    }
    case EventType::KeyPressed:
    {
        auto& keyEvent = static_cast<KeyPressedEvent&>(evt);
        break;
    }
    case EventType::WindowResize:
    {
        GenerateGLBuffers();
        break;
    }
    default: break;
    }
}

}
