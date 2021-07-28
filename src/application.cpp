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
#include "Shader.h"
#include "texture.h"
#include "vendor/glm/glm/glm.hpp"
#include "vendor/glm/glm/gtc/matrix_transform.hpp"


int main(int argc, char* argv[]){
    // ----- Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        fprintf(stderr, "SDL could not initialize\n");
        return 1;
    }

    // ----- Create window
    SDL_Window* window = SDL_CreateWindow("Test App", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960, 540, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
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
            100.f,  100.f, 0.0f, 0.0f, //0
            200.f,  100.f, 1.0f, 0.0f, //1
            200.f,  200.f, 1.0f, 1.0f, //2
            100.f,  200.5f, 0.0f, 1.0f  //3
        }; 

        //define vertices to use for triangles
        unsigned int indices[] = {
            0,1,2,
            2,3,0
        };
        
        // ---- Blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // ---- create Buffers
        VertexArray va;
        VertexBuffer vb(positions,4*4*sizeof(float));

        VertexBufferLayout layout;
        layout.push<float>(2);
        layout.push<float>(2);
        va.addBuffer(vb, layout);

        IndexBuffer ib(indices,8);

        //Maths
        glm::mat4 proj = glm::ortho(0.0f ,960.0f, 0.0f, 540.0f, -1.0f, 1.0f);    //Orthographic matrix

        //Shaders
        Shader shader("res/shaders/Basic.shader");
        shader.bind();
        // shader.setUniform4f("u_Color",0.0f,0.0f,1.0f,1.0f);
        shader.setUniformMat4f("u_MVP",proj);

        // glClearColor(1.0f,1.0f,1.f,1.f); //Set background-color to white

        //set up textures
        Texture texture("res/textures/doge.png");
        texture.bind(0);
        shader.setUniform1i("u_Texture",0);

        //unbind programs & buffers to explicitly bind them again before drawing
        va.unbind();
        shader.unbind();
        vb.unbind();
        ib.unbind();

        //create renderer
        Renderer renderer;

        // ----- Game loop
        // float r = 0.0f,g=0.0f,b=1.0;
        // float increment = 0.05f;
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
            renderer.clear();
            glDebugMessageCallback(GLDebugMessageCallback,nullptr); //Debugging-function

            shader.bind();
            // shader.setUniform4f("u_Color", r,g,b,1.0f);   
            renderer.draw(va,ib,shader);


            // //Change color
            // if(r>1.0f || b>1.0f)
            //     increment = -0.05f;
            // else if (r<0.0f || b<0.0f) 
            //     increment = 0.05f;
            // r += increment;
            // b -= increment;

            SDL_GL_SwapWindow(window);
        }
    }
    SDL_GL_DeleteContext(glContext);

    return 0;
}