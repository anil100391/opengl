#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];

    ShaderType type = NONE;
    while (getline(stream, line))
    {
        if ( line.find("#shader") != std::string::npos )
        {
            if ( line.find("vertex") != std::string::npos )
                type = ShaderType::VERTEX;
            if ( line.find("fragment") != std::string::npos )
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[type] << line << "\n";
        }
    }

    return { ss[0].str(), ss[1].str() };
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if ( result == GL_FALSE )
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char *message = (char *)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragement") << " shader!\n";
        std::cout << message << "\n";
        glDeleteShader(id);
        return 0;
    }

    return id;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static unsigned int CreateShader( const std::string& vertexShader,
                                  const std::string& fragmentShader )
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if ( GLEW_OK == glewInit() )
        std::cout << glGetString(GL_VERSION) << std::endl;
    else
        return 0;

    float positions[6] = { -0.5f, -0.5f,
                            0.0f, 0.5f,
                            0.5f, -0.5f };

    unsigned int buffer = 0;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    ShaderProgramSource source = ParseShader("res/shaders/basic.shader");
    unsigned int program = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(program);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(program);
    glfwTerminate();
    return 0;
}

