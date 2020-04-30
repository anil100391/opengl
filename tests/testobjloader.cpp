#include "testobjloader.h"
#include "../renderer.h"
#include "../light.h"
#include "../events/mouseevent.h"
#include <imgui.h>
#include <fstream>
#include <algorithm>
#include "../app.h"

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestObjLoader::TestObjLoader(Application *app)
    : Test(app),
      _mesh("res/suzanne.obj"),
      _material { glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), // ambient
                  glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), // diffuse
                  glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), // specular
                  32.0f }                            // shininess
{
    SetUpCamera();

    std::vector<float> &positions = _mesh._vertices;
    std::vector<float> &normals = _mesh._normals;
    std::vector<unsigned int> &trias = _mesh._trias;
    std::vector<unsigned int> &quad = _mesh._quads;;

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    std::vector<float> vertices;
    for ( int ii = 0; ii < positions.size() / 3; ++ii )
    {
        // push positions
        vertices.push_back(positions[3*ii]);
        vertices.push_back(positions[3*ii+1]);
        vertices.push_back(positions[3*ii+2]);
        // push normals
        vertices.push_back(normals[3*ii]);
        vertices.push_back(normals[3*ii+1]);
        vertices.push_back(normals[3*ii+2]);
    }

    _vao = std::make_unique<VertexArray>();
    _vbo = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(3);
    _vao->AddBuffer(*_vbo, layout);

    _ibo = std::make_unique<IndexBuffer>(trias.data(), trias.size());

    _shader = std::make_unique<Shader>("res/shaders/obj.shader");
    _selectShader = std::make_unique<Shader>("res/shaders/select.shader");
    _shader->Bind();
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
        _viewMat = _camera.GetViewMatrix();
        int width, height;
        _app->GetWindowSize(width, height);
        _projMat = glm::perspective(glm::radians(45.0f), (float) width / (float)height, 0.1f, 100.0f);

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
        light l = GetLight();

        _shader->SetUniform3f("light.position", l._position);
        _shader->SetUniform4f("light.ambient", l._ambient);
        _shader->SetUniform4f("light.diffuse", l._diffuse);
        _shader->SetUniform4f("light.specular", l._specular);

        _shader->Bind();
        renderer.Draw(*_vao, *_ibo, *_shader);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestObjLoader::OnImGuiRender()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestObjLoader::OnEvent( Event &evt )
{
    auto evtType = evt.GetEventType();

    switch (evtType)
    {
        case EventType::MouseMoved:
        case EventType::MouseButtonPressed:
        case EventType::MouseButtonReleased:
            break;
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
        case EventType::WindowResize:
        default: break;
    }

    if ( evt.GetEventType() == EventType::MouseButtonPressed )
    {
        auto& mouseEvt = static_cast<MouseEvent&>(evt);
    }
    else if( evt.GetEventType() == EventType::MouseScrolled )
    {
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
light TestObjLoader::GetLight() const
{
    const glm::vec3& p = _camera.GetPosition();
    return { glm::vec3(p[0], p[1], p[2]),       // position
             glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), // ambient
             glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), // diffuse
             glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), // specular
           };
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestObjLoader::SetUpCamera() noexcept
{
    _camera.SetLookAt(_mesh.cog());
    const box3& meshBBox = _mesh.bbox();
    const float* min = meshBBox.min();
    _camera.SetPosition(glm::vec3(1 * min[0], 1 * min[1], 1 * min[2]) * 5.0f);
}
}
