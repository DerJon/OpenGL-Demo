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

int main(int argc, char *argv[])
{
    // ----- Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "SDL could not initialize\n");
        return 1;
    }

    // ----- Create window
    SDL_Window *window = SDL_CreateWindow("Test App", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 1000, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!window)
    {
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
    std::cout << "OpenGL-Version " << glGetString(GL_VERSION) << std::endl; //Display Info about OpenGL-Version

    // ----- SDL v-sync
    SDL_GL_SetSwapInterval(1);

    // ----- GLEW
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Error in GLEW-Initalisation\n");
        return 3;
    }
    {

        //define vertices
        std::vector<float> positions = {
            //x  ,  y
            -50.f, -100.f,  //0
            -25.f, -100.f,  //1
            -25.f, -95.f,   //2
            -50.f, -95.f,   //3

            -45.f, -100.f,   //4
            -45.f,  100.f,   //5
            -50.f,  100.f,   //6

            -50.f,  95.f,   //7
            -25.f, 100.f,   //8
            -25.f,  95.f,   //9

            25.f, -100.f,    //10
            50.f, -100.f,    //11
            50.f,  -95.f,    //12
            25.f,  -95.f,    //13

            45.f, -100.f,   //14
            50.f,  100.f,   //15
            45.f,  100.f,   //16

            50.f,   95.f,   //17
            25.f,  100.f,   //18
            25.f,   95.f,   //19
        };

        for(long unsigned int i=0; i<positions.size(); i++)
            positions[i]+=500;

        //define vertices to use for triangles
        std::vector<unsigned int> indices = {
            0, 1, 2,
            2, 3, 0,
            0, 4, 5,
            5, 6, 0,
            6, 7, 8,
            8, 9, 7,

            10,11,12,
            12,13,10,
            14,15,11,
            15,16,14,
            17,18,19,
            18,17,15
        };

        // ---- Blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // ---- create Buffers
        VertexArray va;
        VertexBuffer vb(&positions[0], positions.size() * sizeof(float));

        VertexBufferLayout layout;
        layout.push<float>(2);
        va.addBuffer(vb, layout);

        IndexBuffer ib(&indices[0], indices.size());

        //Maths
        glm::mat4 proj = glm::ortho(0.0f, 1000.0f, 0.0f, 1000.0f, -1.0f, 1.0f); //Orthographic matrix
        glm::mat4 view = glm::translate(glm::mat4(1.0f),glm::vec3(0,0,0));

        //Shaders
        Shader shader("res/shaders/Basic.shader");
        shader.bind();
        shader.setUniform4f("u_Color", 0.f, 1.f, 0.f, 1.f);

        // glClearColor(1.0f,1.0f,1.f,1.f); //Set background-color to white

        //unbind programs & buffers to explicitly bind them again before drawing
        va.unbind();
        shader.unbind();
        vb.unbind();
        ib.unbind();

        //create renderer
        Renderer renderer;

        glm::vec3 translationA(-200,0,0);
        glm::vec3 translationB(200,0,0);

        // ----- Game loop
        bool quit = false;
        SDL_Event windowEvent;
        while (quit == false)
        {
            while (SDL_PollEvent(&windowEvent))
            {
                if (windowEvent.type == SDL_QUIT)
                {
                    quit = true;
                    break;
                }
            }

            //DRAWING
            renderer.clear();
            glDebugMessageCallback(GLDebugMessageCallback, nullptr); //Debugging-function

            shader.bind();
            shader.setUniform4f("u_Color", 0.f, 1.f, 0.f, 1.f);

            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
                glm::mat4 mvp = proj * view * model;
                shader.setUniformMat4f("u_MVP", mvp);
                renderer.draw(va, ib, shader);
            }
            { 
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
                glm::mat4 mvp = proj * view * model;
                shader.setUniformMat4f("u_MVP", mvp);
                renderer.draw(va, ib, shader);
            }



            SDL_GL_SwapWindow(window);
        }
    }
    SDL_GL_DeleteContext(glContext);

    return 0;
}