#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <stdio.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "GLDebugMessageCallback.h"
#include "Render.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"


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

    // ----- SDL OpenGL context & settings
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    std::cout << "OpenGL-Version " <<glGetString(GL_VERSION) << std::endl;  //Display Info about OpenGL-Version

    // ----- SDL v-sync
    SDL_GL_SetSwapInterval(1);

    // ----- GLEW
    if(glewInit()!=GLEW_OK){
        fprintf(stderr,"Error in GLEW-Initalisation\n");
        return 3;
    }
    {
        //define vertices
        float positions []={
            //x  ,  y
            -0.5f, -0.5f,   //0
             0.5f,  -0.5f,  //1
             0.5f,   0.5f,  //2
            -0.5f,  0.5f,   //3
        }; 

        //define vertices to use for triangles
        unsigned int indices[] = {
            0,1,2,
            2,3,0
        };
        
        // ---- create Buffers
        VertexArray va;
        VertexBuffer vb(positions,4*2*sizeof(float));

        VertexBufferLayout layout;
        layout.push<float>(2);
        va.addBuffer(vb, layout);

        IndexBuffer ib(indices,6);

        //Layout-Definition


        //Shaders
        ShaderProgramSource source = parseShader("res/shaders/Basic.shader");
        unsigned int shader = createShader(source.VertexSource, source.FragmentSource);
        glUseProgram(shader);

        //glClearColor(0.2f,0.2f,1.f,0.f); //Set background-color

        int location = glGetUniformLocation(shader, "u_Color");

        //unbind programs & buffers to explicitly bind them again before drawing
        glBindVertexArray(0);
        glUseProgram(0);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // ----- Game loop
        float r = 0.0f,g=0.0f,b=1.0;
        float increment = 0.05f;
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
            glDebugMessageCallback(GLDebugMessageCallback,nullptr); //Debugging-function

            glUseProgram(shader);
            glUniform4f(location, r,g,b,1.0f);

            va.bind();
            ib.bind();    

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);


            //Change color
            if(r>1.0f || b>1.0f)
                increment = -0.05f;
            else if (r<0.0f || b<0.0f) 
                increment = 0.05f;
            r += increment;
            b -= increment;

            SDL_GL_SwapWindow(window);
        }

        // ----- Clean up
        glDeleteProgram(shader);
    }
    SDL_GL_DeleteContext(glContext);

    return 0;
}