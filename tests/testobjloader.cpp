#include "testobjloader.h"
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
TestObjLoader::TestObjLoader(Application *app)
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

    CreateEnvironmentGLBuffers();
    std::vector<std::string> skyboxFiles { "res/textures/skybox/right.jpg",
                                           "res/textures/skybox/left.jpg",
                                           "res/textures/skybox/top.jpg",
                                           "res/textures/skybox/bottom.jpg",
                                            "res/textures/skybox/front.jpg",
                                            "res/textures/skybox/back.jpg"
                                           };
    _cubemapTexture = std::make_unique<CubeMap>(skyboxFiles);
    _cubemapShader = std::make_unique<Shader>("res/shaders/cubemap.shader");

    CreateMeshGLBuffers();

    _shader = std::make_unique<Shader>("res/shaders/obj.shader");
    _selectShader = std::make_unique<Shader>("res/shaders/select.shader");
    _shader->Bind();

    // _texture = std::make_unique<Texture>("res/textures/suzanne.png");
    // _texture->Bind(0);
    _shader->SetUniform1i("u_Texture", 0);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestObjLoader::CreateEnvironmentGLBuffers()
{
    std::vector<float> vertices = { -1.0f,  1.0f, -1.0f,
                                    -1.0f, -1.0f, -1.0f,
                                     1.0f, -1.0f, -1.0f,
                                     1.0f, -1.0f, -1.0f,
                                     1.0f,  1.0f, -1.0f,
                                    -1.0f,  1.0f, -1.0f,

                                    -1.0f, -1.0f,  1.0f,
                                    -1.0f, -1.0f, -1.0f,
                                    -1.0f,  1.0f, -1.0f,
                                    -1.0f,  1.0f, -1.0f,
                                    -1.0f,  1.0f,  1.0f,
                                    -1.0f, -1.0f,  1.0f,

                                     1.0f, -1.0f, -1.0f,
                                     1.0f, -1.0f,  1.0f,
                                     1.0f,  1.0f,  1.0f,
                                     1.0f,  1.0f,  1.0f,
                                     1.0f,  1.0f, -1.0f,
                                     1.0f, -1.0f, -1.0f,

                                    -1.0f, -1.0f,  1.0f,
                                    -1.0f,  1.0f,  1.0f,
                                     1.0f,  1.0f,  1.0f,
                                     1.0f,  1.0f,  1.0f,
                                     1.0f, -1.0f,  1.0f,
                                    -1.0f, -1.0f,  1.0f,

                                    -1.0f,  1.0f, -1.0f,
                                     1.0f,  1.0f, -1.0f,
                                     1.0f,  1.0f,  1.0f,
                                     1.0f,  1.0f,  1.0f,
                                    -1.0f,  1.0f,  1.0f,
                                    -1.0f,  1.0f, -1.0f,

                                    -1.0f, -1.0f, -1.0f,
                                    -1.0f, -1.0f,  1.0f,
                                     1.0f, -1.0f, -1.0f,
                                     1.0f, -1.0f, -1.0f,
                                    -1.0f, -1.0f,  1.0f,
                                     1.0f, -1.0f,  1.0f
                                  };

    std::vector<unsigned int> conn = { 0, 1, 2, 3, 4, 5,
                                       6, 7, 8, 9, 10, 11,
                                       12, 13, 14, 15, 15, 17,
                                       18, 19, 20, 21, 22, 23,
                                       24, 25, 26, 27, 28, 29,
                                       30, 31, 32, 33, 34, 35 };

    _envvao = std::make_unique<VertexArray>();
    _envvbo = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(3); // position
    _envvao->AddBuffer(*_envvbo, layout);

    _envibo = std::make_unique<IndexBuffer>(conn.data(), conn.size());
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestObjLoader::CreateMeshGLBuffers()
{
    std::vector<float> vertices = mbos::vbo(_mesh, _flatShading);
    std::vector<unsigned int> conn = mbos::ibo(_mesh, _flatShading);

    _vao = std::make_unique<VertexArray>();
    _vbo = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(3); // position
    layout.Push<float>(3); // normal
    layout.Push<float>(2); // texture coordinate
    _vao->AddBuffer(*_vbo, layout);

    _ibo = std::make_unique<IndexBuffer>(conn.data(), conn.size());
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestObjLoader::~TestObjLoader()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestObjLoader::OnUpdate(float deltaTime)
{
     _time = deltaTime;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestObjLoader::OnRender()
{
    Renderer renderer;
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

        renderer.Draw(*_vao, *_ibo, *_shader);
    }

    {
        glm::mat4 model(1.0f);
        glm::mat4 viewmat = glm::mat4(glm::mat3(_camera.GetViewMatrix()));
        int width, height;
        _app->GetWindowSize(width, height);
        auto proj = glm::perspective(glm::radians(45.0f), (float) width / (float)height, 0.1f, 100.0f);
        auto mvp = model /* viewmat*/ * proj;

        glDepthFunc(GL_LEQUAL);
        _cubemapTexture->Bind();
        _cubemapShader->Bind();
        _cubemapShader->SetUniformMat4f("u_MVP", mvp);
        renderer.Draw(*_envvao, *_envibo, *_cubemapShader);
        glDepthFunc(GL_LESS);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestObjLoader::OnImGuiRender()
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
void TestObjLoader::OnEvent( Event &evt )
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
void TestObjLoader::Select( int x, int y )
{
    /*
    // draw using selection shader
    {
        Renderer renderer;
        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), _modelLocation);
        model = glm::rotate(model, (float)M_PI / 2.0f, glm::vec3(1.0, 0.0, 0.0));
        model = glm::rotate(model, _modelRotation, glm::vec3(0.0, 0.0, 1.0));
        model = glm::scale(model, glm::vec3(_modelScale));

        // set transformation matrices uniforms
        _selectShader->SetUniformMat4f("u_M", model);
        _selectShader->SetUniformMat4f("u_V", _viewMat);
        _selectShader->SetUniformMat4f("u_P", _projMat);

        _selectShader->SetUniform4f("u_Color", glm::vec4(1.0, 1.0, 1.0, 1.0));
        _selectShader->Bind();
        renderer.Draw(*_vao, *_ibo, *_selectShader);
    }

    // force all draw commands to finish before calling glReadPixels
    glFlush();
    glFinish();
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    // process select draw
    unsigned char res[4];
    GLint         viewport[4];

    glGetIntegerv( GL_VIEWPORT, viewport );
    std::cout << "viewport : " << viewport[3] - y << std::endl;
    glReadPixels( x, viewport[3] - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, res );
    printf( "pick data : %d %d %d %d\n", res[0], res[1], res[2], res[3] );
    bool selected = (res[0] != 0);
    if ( selected )
    {
        material m = GetMaterial();
        float r = (1.0f * rand()) / RAND_MAX;
        float g = (1.0f * rand()) / RAND_MAX;
        float b = (1.0f * rand()) / RAND_MAX;
        m._diffuse = glm::vec4(r, g, b, 1.0f);
        SetMaterial(m);
    }
    */
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
const material& TestObjLoader::GetMaterial() const
{
    return _material;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestObjLoader::SetMaterial(const material& m)
{
    _material = m;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
pointlight TestObjLoader::GetLight() const
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
void TestObjLoader::SetUpCamera() noexcept
{
    _camera.SetLookAt(_mesh.cog());
    const box3& meshBBox = _mesh.bbox();
    const float* min = meshBBox.max();
    _camera.SetPosition(glm::vec3(1 * min[0], 1 * min[1], 1 * min[2]) * 5.0f);
}
}
