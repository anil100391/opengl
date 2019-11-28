#include "testfragmentshader.h"
#include "../renderer.h"
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestFragmentShader::TestFragmentShader()
    : _startX(0.354),
      _startY(0.361)
{
    float positions[] = { 0.0f, 0.0f, 0.0f, 0.0f,
                          512.0f, 0.0f, 1.0f, 0.0f,
                          512.0f,  512.0f, 1.0f, 1.0f,
                          0.0f,  512.0f, 0.0f, 1.0f };

    unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_DEPTH_TEST);

    _vao = std::make_unique<VertexArray>();
    _vbo = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);
    _vao->AddBuffer(*_vbo, layout);

    _ibo = std::make_unique<IndexBuffer>(indices, 6);

    _shader = std::make_unique<Shader>("res/shaders/julia.shader");
    _shader->Bind();

    _shader->SetUniform2f("u_StartPos", _startX, _startY);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestFragmentShader::~TestFragmentShader()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestFragmentShader::OnUpdate(float deltaTime)
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestFragmentShader::OnRender()
{
    Renderer renderer;
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 viewMat(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)));
    glm::mat4 projMat(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f));
    {
        glm::mat4 mvp = projMat * viewMat;

        _shader->SetUniform2f("u_StartPos", _startX, _startY);
        _shader->SetUniformMat4f("u_MVP", mvp);

        _shader->Bind();
        renderer.Draw(*_vao, *_ibo, *_shader);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestFragmentShader::OnImGuiRender()
{
    ImGui::SliderFloat("startx", &_startX, -1.5f, 1.5f);
    ImGui::SliderFloat("starty", &_startY, -1.5f, 1.5f);
}

}
