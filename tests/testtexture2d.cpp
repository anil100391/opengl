#include "testtexture2d.h"
#include "../renderer.h"
#include <imgui.h>

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestTexture2D::TestTexture2D()
    : _viewMat(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
      _projMat(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
      _translationA(glm::vec3(200.0f, 200.0, 0.0f)),
      _translationB(glm::vec3(400.0f, 200.0, 0.0f))
{
    float positions[] = { -50.0f, -50.0f, 0.0f, 0.0f,
                           50.0f, -50.0f, 1.0f, 0.0f,
                           50.0f,  50.0f, 1.0f, 1.0f,
                          -50.0f,  50.0f, 0.0f, 1.0f };

    unsigned int indices[] = { 0, 1, 2,
                               2, 3, 0 };

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

    _shader = std::make_unique<Shader>("res/shaders/basic.shader");
    _shader->Bind();

    _texture = std::make_unique<Texture>("res/textures/sample.jpg");
    _texture->Bind(0);
    _shader->SetUniform1i("u_Texture", 0);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestTexture2D::~TestTexture2D()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestTexture2D::OnUpdate(float deltaTime)
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestTexture2D::OnRender()
{
    Renderer renderer;
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT);

    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), _translationA);
        glm::mat4 mvp = _projMat * _viewMat * model;

        _shader->SetUniformMat4f("u_MVP", mvp);

        _shader->Bind();
        renderer.Draw(*_vao, *_ibo, *_shader);
    }

    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), _translationB);
        glm::mat4 mvp = _projMat * _viewMat * model;

        _shader->SetUniformMat4f("u_MVP", mvp);

        _shader->Bind();
        renderer.Draw(*_vao, *_ibo, *_shader);
    }

}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestTexture2D::OnImGuiRender()
{
    ImGui::SliderFloat3("TranslateA", &_translationA.x, 0.0f, 960.0f);
    ImGui::SliderFloat3("TranslateB", &_translationB.x, 0.0f, 960.0f);
}

}
