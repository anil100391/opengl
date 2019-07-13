#ifndef _shader_h_
#define _shader_h_

#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class Shader
{
public:

    Shader(const std::string& filepath);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    // Set uniforms
    void SetUniform1i(const std::string& name, int i0);
    void SetUniform1f(const std::string& name, float f0);
    void SetUniform4f(const std::string& name, float f0, float f1, float f2, float f3);

private:

    ShaderProgramSource ParseShader(const std::string& filepath);
    unsigned int CompileShader(unsigned int type, const std::string& source);
    unsigned int CreateShader( const std::string& vertexShader,
                               const std::string& fragmentShader );
    int GetUniformLocation(const std::string& name);

    unsigned int    _rendererID = 0;
    std::string     _filePath;
    std::unordered_map<std::string, int> _uniformLocationCache;
};

#endif // _shader_h_
