#include "IndexBuffer.h"
#include <GL/glew.h>

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
    :m_Count(count)
{
    glGenBuffers(1,&m_RendererID);    //generate buffer and safe adress
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_RendererID);   //select (=bind) bufer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*sizeof(unsigned int),data,GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer(){
    glDeleteBuffers(1,&m_RendererID);
}

void IndexBuffer::bind() const{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_RendererID);   //select (=bind) bufer
}

void IndexBuffer::unbind() const{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);   //select (=bind) bufer
}