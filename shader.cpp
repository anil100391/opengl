#include <iostream>
#include "renderer.h"
#include "shader.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Shader::Shader(const std::string& filepath)
    : _filePath(filepath)
{
    auto[vertexSource, fragmentSource] = ParseShader(filepath);
    _rendererID = CreateShader(vertexSource, fragmentSource);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Shader::~Shader()
{
    glDeleteProgram(_rendererID);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Shader::Bind() const
{
    glUseProgram(_rendererID);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Shader::Unbind() const
{
    glUseProgram(0);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Shader::SetUniform1i(const std::string& name, int i0)
{
    glUniform1i(GetUniformLocation(name), i0);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Shader::SetUniform1f(const std::string& name, float f0)
{
    glUniform1f(GetUniformLocation(name), f0);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Shader::SetUniform2f(const std::string& name, float f0, float f1)
{
    glUniform2f(GetUniformLocation(name), f0, f1);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Shader::SetUniform3f(const std::string& name, float f0, float f1, float f2)
{
    glUniform3f(GetUniformLocation(name), f0, f1, f2);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Shader::SetUniform4f(const std::string& name, float f0, float f1, float f2, float f3)
{
    glUniform4f(GetUniformLocation(name), f0, f1, f2, f3);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Shader::SetUniform3f(const std::string& name, const glm::vec3 &v)
{
    glUniform3f(GetUniformLocation(name), v[0], v[1], v[2]);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Shader::SetUniform4f(const std::string& name, const glm::vec4 &v)
{
    glUniform4f(GetUniformLocation(name), v[0], v[1], v[2], v[3]);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& mat)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int Shader::GetUniformLocation(const std::string& name)
{
    int location = -1;

    auto it = _uniformLocationCache.find(name);
    if ( it != _uniformLocationCache.end() )
        location  = it->second;
    else
    {
        location = glGetUniformLocation(_rendererID, name.c_str());
        _uniformLocationCache[name] = location;
    }

    return location;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::tuple<std::string, std::string> Shader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];

    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if ( line.find("#shader") != std::string::npos )
        {
            if ( line.find("vertex") != std::string::npos )
                type = ShaderType::VERTEX;
            if ( line.find("fragment") != std::string::npos )
                type = ShaderType::FRAGMENT;
        }
        else if ( type == ShaderType::VERTEX || type == ShaderType::FRAGMENT )
        {
            ss[(int)type] << line << "\n";
        }
    }

    return { ss[0].str(), ss[1].str() };
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
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
unsigned int Shader::CreateShader( const std::string& vertexShader,
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

