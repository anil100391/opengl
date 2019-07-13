#include "renderer.h"
#include "vertexarray.h"
#include "vertexbufferlayout.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "shader.h"

#include <iostream>
#include <string>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if ( GLEW_OK == glewInit() )
        std::cout << glGetString(GL_VERSION) << std::endl;
    else
        return 0;

    float positions[] = { -0.5f, -0.5f,
                           0.5f, -0.5f,
                           0.5f,  0.5f,
                          -0.5f,  0.5f };

    unsigned int indices[] = { 0, 1, 2,
                               2, 3, 0 };

    {
    VertexArray va;
    VertexBuffer vb(positions, 4 * 2 * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);

    IndexBuffer ib(indices, 6);

    Shader shader("res/shaders/basic.shader");
    shader.Bind();

    // reset ogl states
    shader.Unbind();
    va.Unbind();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    float r = 0.0f;
    float increment = 0.05f;

    Renderer renderer;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.Clear();

        if ( r < 0.0f )
            increment = 0.05f;
        else if ( r > 1.0f )
            increment = -0.05f;

        r += increment;

        shader.Bind();
        shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

        renderer.Draw(va, ib, shader);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    }

    glfwTerminate();
    return 0;
}

