#include "testlorenz.h"
#include "../app.h"
#include "../utils/bbox.h"
#include <imgui.h>

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestLorenz::TestLorenz(Application *app) : Test(app)
{
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glDepthMask(false);

    CreateLorenzGLBuffers();
    _shader = std::make_unique<Shader>("res/shaders/lorenz.shader");
    _shader->Bind();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLorenz::CreateLorenzGLBuffers()
{

    std::vector<float> vertices;
    std::vector<unsigned int> conn;

    float h = 0.01f,
          a = 10.0f,
          b = 28.0f,
          c = 8.0f / 3.0f;

    float x0 = 0.1f;
    float y0 = 0.0f;
    float z0 = 0.0f;
    float t  = 0.0f;

    int iterations = _iterations;

    vertices.reserve(3*(iterations + 1));
    vertices.emplace_back(x0);
    vertices.emplace_back(y0);
    vertices.emplace_back(z0);

    conn.reserve(2 * iterations);

    for (int ii = 0; ii < iterations; ++ii)
    {
        float x1 = x0 + h * a * ( y0 - x0 );
        float y1 = y0 + h * ( x0 * ( b - z0 ) - y0 );
        float z1 = z0 + h * ( x0 * y0 - c * z0 );
        x0 = x1;
        y0 = y1;
        z0 = z1;
        t  = t + h;

        vertices.emplace_back( x0 );
        vertices.emplace_back( y0 );
        vertices.emplace_back( z0 );

        conn.emplace_back(ii);
        conn.emplace_back(ii+1);
    }


    VertexBufferLayout layout;
    layout.Push<float>(3); // position

    _vao = std::make_unique<VertexArray>();
    _vbo = std::make_unique<VertexBuffer>( vertices.data(), static_cast<unsigned int>(vertices.size() * sizeof( float )) );

    _vao->AddBuffer( *_vbo, layout );

    _ibo = std::make_unique<IndexBuffer>( conn.data(), static_cast<unsigned int>(conn.size()) );

    // Update camera to keep everything in frame
    box3 bbox;
    for (unsigned int ii = 0; ii < vertices.size() / 3; ++ii)
    {
        float *v = &vertices[3*ii];
        bbox.expand(v);
    }

    const float *center = bbox.center();
    const float *max    = bbox.max();

    _camera.SetLookAt(glm::vec3(center[0], center[1], center[2]));
    _camera.SetPosition(1.5f * glm::vec3(max[0], max[1], max[2]));
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestLorenz::~TestLorenz()
{
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_BLEND);
    glDepthMask(true);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLorenz::OnUpdate( float deltaTime )
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLorenz::OnRender()
{
    Renderer renderer;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set up mvp matrices
    glm::mat4 model(1.0f);

    _viewMat = _camera.GetViewMatrix();

    int width, height;
    _app->GetWindowSize(width, height);
    _projMat = glm::perspective(glm::radians(45.0f), (float) width / (float)height, 0.1f, 100.0f);

    _shader->Bind();
    _shader->SetUniformMat4f("u_M", model);
    _shader->SetUniformMat4f("u_V", _viewMat);
    _shader->SetUniformMat4f("u_P", _projMat);

    renderer.Draw(*_vao, *_ibo, *_shader, Renderer::LINES);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLorenz::OnImGuiRender()
{
    ImGui::SliderInt("Iterations", &_iterations, 100, 10000);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLorenz::OnEvent( Event &evt )
{
    auto evtType = evt.GetEventType();

    static int currIterations = _iterations;

    switch (evtType)
    {
        case EventType::MouseMoved:
        {
            if ( currIterations != _iterations )
            {
                currIterations = _iterations;
                CreateLorenzGLBuffers();
            }
            break;
        }
        case EventType::MouseButtonPressed:
        case EventType::MouseButtonReleased:
        case EventType::MouseScrolled:
        case EventType::KeyPressed:
        case EventType::WindowResize:
        default: break;
    }

    _camera.OnEvent( evt );
}
}