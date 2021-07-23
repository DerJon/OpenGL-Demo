#pragma once
#include <string>

struct ShaderProgramSource{
    std::string VertexSource;
    std::string FragmentSource;
};

class Shader {
private:
    std::string m_FilePath;
    unsigned int m_RendererID;

    ShaderProgramSource parseShader(const std::string& filepath);
    unsigned int getUniformLocation(const std::string& name);
    unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);
    unsigned int compileShader(unsigned int type, const std::string& source);

public:
    Shader(const std::string& filename);
    ~Shader();

    void bind() const;
    void unbind() const;

    //set uniforms
    void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
};