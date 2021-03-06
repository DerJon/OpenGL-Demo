#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "Shader.h"
#include "Render.h"


Shader::Shader(const std::string& filepath)
    : m_FilePath(filepath), m_RendererID(0)
{
    ShaderProgramSource source = parseShader(filepath);
    m_RendererID = createShader(source.VertexSource, source.FragmentSource);
};

Shader::~Shader(){
    glDeleteProgram(m_RendererID);
};

void Shader::bind() const{
    glUseProgram(m_RendererID);
};
void Shader::unbind() const{
    glUseProgram(0);
};

int Shader::getUniformLocation(const std::string& name)
{
    if(m_UniformLocationCache.find(name)!=m_UniformLocationCache.end())
        return m_UniformLocationCache[name];
    
    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if(location==-1)
        std::cout << "Warning: Uniform '" << name << "' does not exist" << std::endl;
    m_UniformLocationCache[name]=location;
    return location;
};


ShaderProgramSource Shader::parseShader(const std::string& filepath){
    std::ifstream stream(filepath);

    enum class ShaderType{
        NONE=-1, VERTEX=0, FRAGMENT=1
    };

    std::string line;
    std::stringstream ss[2]; 
    ShaderType type=ShaderType::NONE;
    while(getline(stream,line)){
        if (line.find("#shader") != std::string::npos){
            if(line.find("vertex") != std::string::npos){
                //set mode to vertex
                type=ShaderType::VERTEX;
            }
            else if(line.find("fragment")!=std::string::npos){
                //set mode to fragment
                type=ShaderType::FRAGMENT;
            }
        }
        else{
            ss[(int)type] << line << '\n';
        }
    }
    return {ss[0].str(), ss[1].str()};
}

unsigned int Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader){
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program,vs);
    glAttachShader(program,fs);
    glLinkProgram(program);
    glValidateProgram(program);

    int result;
    glGetProgramiv(program,GL_LINK_STATUS, &result);
    if(result == GL_FALSE){
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetProgramInfoLog(program,length,&length,message);
        std::cout<<"Failed to link program!" << std::endl;
        std::cout<<message<<std::endl;
        glDeleteProgram(program);
    }

    //Delete Shaders after succesful linking
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source){
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    //Error handeling
    int result;
    glGetShaderiv(id,GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE){
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length  * sizeof(char));
        glGetShaderInfoLog(id,length,&length,message);
        std::cout<<"Failed to compile " << (type==GL_VERTEX_SHADER? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout<<message<<std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3){
    glUniform4f(getUniformLocation(name), v0,v1,v2,v3);
};

void Shader::setUniform1i(const std::string& name, int value){
    glUniform1i(getUniformLocation(name), value);
};

void Shader::setUniformMat4f(const std::string& name, const glm::mat4& matrix){
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
};