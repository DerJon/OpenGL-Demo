#shader vertex
#version 330 core

layout (location=0) in vec2 positions;

void main()
{
    gl_Position = vec4(positions.x, positions.y, 1.0, 1.0);
};


#shader fragment
#version 330 core

out vec4 color;

void main()
{
    color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
};