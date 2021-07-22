#include "VertexBuffer.h"
#include <GL/glew.h>

VertexBuffer::VertexBuffer(const void* data, unsigned int size){
    glGenBuffers(1,&m_RendererID);    //generate buffer and safe adress
    glBindBuffer(GL_ARRAY_BUFFER,m_RendererID);   //select (=bind) bufer
    glBufferData(GL_ARRAY_BUFFER, size,data,GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer(){
    glDeleteBuffers(1,&m_RendererID);
}

void VertexBuffer::bind() const{
    glBindBuffer(GL_ARRAY_BUFFER,m_RendererID);   //select (=bind) bufer
}

void VertexBuffer::unbind() const{
    glBindBuffer(GL_ARRAY_BUFFER,0);   //select (=bind) bufer
}