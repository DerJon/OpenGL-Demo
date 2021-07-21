#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <signal.h>

#include <stdio.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "GLDebugMessageCallback.h"

void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length,
    const GLchar* msg, const void* data)
{
    const char* _source;
    const char* _type;
    const char* _severity;

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        _source = "API";
        break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "WINDOW SYSTEM";
        break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "SHADER COMPILER";
        break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "THIRD PARTY";
        break;

    case GL_DEBUG_SOURCE_APPLICATION:
        _source = "APPLICATION";
        break;

    case GL_DEBUG_SOURCE_OTHER:
        _source = "UNKNOWN";
        break;

    default:
        _source = "UNKNOWN";
        break;
    }

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        _type = "ERROR";
        break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "DEPRECATED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "UDEFINED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_PORTABILITY:
        _type = "PORTABILITY";
        break;

    case GL_DEBUG_TYPE_PERFORMANCE:
        _type = "PERFORMANCE";
        break;

    case GL_DEBUG_TYPE_OTHER:
        _type = "OTHER";
        break;

    case GL_DEBUG_TYPE_MARKER:
        _type = "MARKER";
        break;

    default:
        _type = "UNKNOWN";
        break;
    }

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        _severity = "HIGH";
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
        _severity = "MEDIUM";
        break;

    case GL_DEBUG_SEVERITY_LOW:
        _severity = "LOW";
        break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
        _severity = "NOTIFICATION";
        break;

    default:
        _severity = "UNKNOWN";
        break;
    }

    // ignore notification severity (you can add your own ignores)
    // + Adds __debugbreak if _DEBUG is defined (automatic in visual studio)
    // note: __debugbreak is specific for MSVC, won't work with gcc/clang
    // -> in that case remove it and manually set breakpoints
    if (_severity != "NOTIFICATION") {
        printf("OpenGL error [%d]: %s of %s severity, raised from %s: %s\n",
            id, _type, _severity, _source, msg);
        raise(SIGTRAP);
    }
}

struct ShaderProgramSource{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource parseShader(const std::string& filepath){
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

static unsigned int compileShader(unsigned int type, const std::string& source){
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

static unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader){
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

int main(int argc, char* argv[]){
    // ----- Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        fprintf(stderr, "SDL could not initialize\n");
        return 1;
    }

    // ----- Create window
    SDL_Window* window = SDL_CreateWindow("Test App", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!window){
        fprintf(stderr, "Error creating window.\n");
        return 2;
    }

    // ----- SDL OpenGL context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    std::cout << "OpenGL-Version " <<glGetString(GL_VERSION) << std::endl;  //Display Info about OpenGL-Version
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    // ----- GLEW
    if(glewInit()!=GLEW_OK){
        fprintf(stderr,"Error in GLEW-Initalisation\n");
        return 3;
    }
 
    //define vertices
    float positions []={
        //x  ,  y
        -0.5f, -0.5f,   //0
         0.5f, -0.5f,   //1
         0.5f,  0.5f,   //2
        -0.5f,  0.5f,   //3
    }; 

    //define vertices to use for triangles
    unsigned int indices[] = {
        0,1,2,
        2,3,0
    };
    
    // ---- create vertexBuffer
    unsigned int buffer;    //Adress of buffer
    glGenBuffers(1,&buffer);    //generate buffer and safe adress
    glBindBuffer(GL_ARRAY_BUFFER,buffer);   //select (=bind) bufer
    glBufferData(GL_ARRAY_BUFFER, 6*2*sizeof(float),positions,GL_STATIC_DRAW);
    // ---- create indexBuffer
    unsigned int ibo;    //Adress of buffer
    glGenBuffers(1,&ibo);    //generate buffer and safe adress
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo);   //select (=bind) bufer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(unsigned int),indices,GL_STATIC_DRAW);

    //Layout-Definition
    glVertexAttribPointer(0, 2, GL_FLOAT,GL_FALSE, sizeof(float)*2, (const void*)0);
    glEnableVertexAttribArray(0);

    //Shaders
    ShaderProgramSource source = parseShader("res/shaders/Basic.shader");
    unsigned int shader = createShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    //glClearColor(0.2f,0.2f,1.f,0.f); //Set background-color

    // ----- Game loop
    bool quit = false;
    SDL_Event windowEvent;
    while (quit == false){
        while (SDL_PollEvent(&windowEvent)){
            if (windowEvent.type == SDL_QUIT){
                quit = true;
                break;
            }
        }

        //DRAWING
        glClear(GL_COLOR_BUFFER_BIT);
        glDebugMessageCallback(GLDebugMessageCallback,nullptr);
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        SDL_GL_SwapWindow(window);
    }

    // ----- Clean up
    glDeleteProgram(shader);
    SDL_GL_DeleteContext(glContext);

    return 0;
}