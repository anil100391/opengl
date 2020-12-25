#include "testframebuffer.h"
#include "../renderer.h"
#include "../light.h"
#include "../events/mouseevent.h"
#include "../events/keyevent.h"
#include <imgui.h>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include "../app.h"
#include "../utils/meshbufferobjects.h"

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestFrameBuffer::TestFrameBuffer(Application *app)
    : Test(app),
      _mesh("res/suzanne.obj"),
      _material { glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), // ambient
                  glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), // diffuse
                  glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), // specular
                  32.0f }                            // shininess
{
    _fileDialog.SetTitle("Open Mesh");
    _fileDialog.SetTypeFilters({".obj"});

    SetUpCamera();

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    CreateMeshGLBuffers();
    CreateFrameBufferGLBuffers();

    _shader = std::make_unique<Shader>("res/shaders/obj.shader");
    _shader->Bind();

    _shader->SetUniform1i("u_Texture", 0);

    _fbshader = std::make_unique<Shader>("res/shaders/basic.shader");
    _framebuffer = std::make_unique<FrameBuffer>();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestFrameBuffer::CreateMeshGLBuffers()
{
    std::vector<float> vertices = mbos::vbo(_mesh, _flatShading);
    std::vector<unsigned int> conn = mbos::ibo(_mesh, _flatShading);

    VertexBufferLayout layout;
    layout.Push<float>(3); // position
    layout.Push<float>(3); // normal
    layout.Push<float>(2); // texture coordinate

    _glMesh = std::make_unique<MeshGL>( vertices, layout, conn );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestFrameBuffer::CreateFrameBufferGLBuffers()
{
    float w = 1920;
    float h = 1080;
    std::vector<float> vertices = { 0, 0, 0.0f, 0.0f,
                                    w, 0, 1.0f, 0.0f,
                                    w, h, 1.0f, 1.0f,
                                    0, h, 0.0f, 1.0f };

    std::vector<unsigned int> conn = {0, 1, 2, 0, 2, 3};

    VertexBufferLayout layout;
    layout.Push<float>(2); // position
    layout.Push<float>(2); // texture

    _fbglMesh = std::make_unique<MeshGL>( vertices, layout, conn );
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestFrameBuffer::~TestFrameBuffer()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestFrameBuffer::OnUpdate(float deltaTime)
{
     _time = deltaTime;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestFrameBuffer::OnRender()
{
    Renderer renderer;

    // render to frame buffer first
    _framebuffer->Bind();
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    {
        glm::mat4 model(1.0f); // identity
        model = glm::rotate(glm::mat4(1.0), (float)_time, glm::vec3(0.0, 0.0, 1.0));
        _viewMat = _camera.GetViewMatrix();
        int width, height;
        _app->GetWindowSize(width, height);
        _projMat = glm::perspective(glm::radians(45.0f), (float) width / (float)height, 0.1f, 100.0f);

        _shader->Bind();
        // set transformation matrices uniforms
        _shader->SetUniformMat4f("u_M", model);
        _shader->SetUniformMat4f("u_V", _viewMat);
        _shader->SetUniformMat4f("u_P", _projMat);

        // set material uniforms
        const material& m = GetMaterial();

        _shader->SetUniform4f("material.ambient", m._ambient);
        _shader->SetUniform4f("material.diffuse", m._diffuse);
        _shader->SetUniform4f("material.specular", m._specular);
        _shader->SetUniform1f("material.shininess", m._shininess);

        // set light uniforms
        pointlight l = GetLight();

        _shader->SetUniform3f("light.position", l._position);
        _shader->SetUniform4f("light.ambient", l._ambient);
        _shader->SetUniform4f("light.diffuse", l._diffuse);
        _shader->SetUniform4f("light.specular", l._specular);

        renderer.Draw(*_glMesh->vao(), *_glMesh->ibo(), *_shader);
    }

    _framebuffer->Unbind();
    // return;

    // render to the frame buffer quad using framebuffer texture
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    {
        int width, height;
        _app->GetWindowSize(width, height);
        _projMat = glm::ortho(0.0f, 1.0f * width, 0.0f, 1.0f * height, -1.0f, 1.0f);

        _framebuffer->BindTexture();
        // set transformation matrices uniforms
        _fbshader->Bind();
        _fbshader->SetUniform1i("u_Texture", 0);//_framebuffer->TextureID());
        _fbshader->SetUniformMat4f("u_MVP", _projMat);

        renderer.Draw( *_fbglMesh->vao(), *_fbglMesh->ibo(), *_fbshader );
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestFrameBuffer::OnImGuiRender()
{
    if ( ImGui::Button("Open Mesh") )
    {
        _fileDialog.Open();
    }

    _fileDialog.Display();
    if ( _fileDialog.HasSelected() )
    {
        auto file = _fileDialog.GetSelected().string();
        _fileDialog.ClearSelected();
        _mesh = mesh(file.c_str());
        CreateMeshGLBuffers();
    }

    Test::OnImGuiRender();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestFrameBuffer::OnEvent( Event &evt )
{
    auto evtType = evt.GetEventType();

    static bool buttonPressed = false;
    static bool dragging = false;
    static int startDragX = 0;
    static int startDragY = 0;

    switch (evtType)
    {
        case EventType::MouseMoved:
        {
            auto& mouseEvt = static_cast<MouseMoveEvent&>(evt);
            dragging = buttonPressed;
            if ( dragging )
            {
                const glm::vec3& cor = _camera.GetLookAt();
                const glm::vec3& pos = _camera.GetPosition();
                int dx = mouseEvt.X() - startDragX;
                if ( dx != 0 )
                    dx = dx / abs(dx);
                startDragX = mouseEvt.X();
                startDragY = mouseEvt.Y();
                double dtheta = 2.0 * dx * M_PI / 180.0;
                double theta = std::atan2(pos[1], pos[0]);
                double len = std::sqrt(pos[0]*pos[0] + pos[1]*pos[1]);
                theta += dtheta;
                glm::vec3 newpos = pos;
                newpos[0] = len * std::cos(theta);
                newpos[1] = len * std::sin(theta);
                _camera.SetPosition(newpos);
            }
            break;
        }
        case EventType::MouseButtonPressed:
        {
            auto& mouseEvt = static_cast<MousePressedEvent&>(evt);
            if ( mouseEvt.GetButton() == MouseEvent::Button::MIDDLE )
            {
                buttonPressed = true;
                startDragX = mouseEvt.X();
                startDragY = mouseEvt.Y();
            }
            break;
        }
        case EventType::MouseButtonReleased:
        {
            auto& mouseEvt = static_cast<MouseReleasedEvent&>(evt);
            if ( mouseEvt.GetButton() == MouseEvent::Button::MIDDLE )
            {
                buttonPressed = false;
            }
            break;
        }
        case EventType::MouseScrolled:
        {
            auto& mouseEvt = static_cast<MouseScrollEvent&>(evt);
            const auto& lookAt = _camera.GetLookAt();
            const auto& eye    = _camera.GetPosition();
            glm::vec3 dir = eye - lookAt;
            float scale = 0.1f * mouseEvt.YOffset();
            dir = dir +  dir * scale;
            _camera.SetPosition(lookAt + dir);
            break;
        }
        case EventType::KeyPressed:
        {
            auto& keyEvent = static_cast<KeyPressedEvent&>(evt);
            if ( keyEvent.GetKeyCode() == GLFW_KEY_S )
            {
                _flatShading = !_flatShading;
                CreateMeshGLBuffers();
            }
            break;
        }
        case EventType::WindowResize:
        default: break;
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
const material& TestFrameBuffer::GetMaterial() const
{
    return _material;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestFrameBuffer::SetMaterial(const material& m)
{
    _material = m;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
pointlight TestFrameBuffer::GetLight() const
{
    const box3& meshBBox = _mesh.bbox();
    const float* min = meshBBox.max();
    const glm::vec3 p(5 * min[0], 5 * min[1], 5 * min[2]);
    return { glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), // ambient
             glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), // diffuse
             glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), // specular
             glm::vec3(p[0], p[1], p[2])        // position
           };
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestFrameBuffer::SetUpCamera() noexcept
{
    _camera.SetLookAt(_mesh.cog());
    const box3& meshBBox = _mesh.bbox();
    const float* min = meshBBox.max();
    _camera.SetPosition(glm::vec3(1 * min[0], 1 * min[1], 1 * min[2]) * 5.0f);
}
}
