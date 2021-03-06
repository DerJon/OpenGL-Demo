#pragma once
#include <vector>
#include <GL/glew.h>

struct VertexBufferElement{
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int getSizeOfType(unsigned int type){
        switch(type){
            case GL_FLOAT :         return 4;
            case GL_UNSIGNED_INT:   return 4;
            case GL_UNSIGNED_BYTE:  return 1;
        }
        return 0;
    }
};

class VertexBufferLayout{
private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;
public:
    VertexBufferLayout()
        : m_Stride(0){};

    template<typename T>
    void push(unsigned int count){
        static_assert(sizeof(T) == 0, "Type not implemented!");
    }

    inline const std::vector<VertexBufferElement> getElements() const& { return m_Elements; }
    inline unsigned int getStride() const {return m_Stride;}
};

template<> inline
void VertexBufferLayout::push<float>(unsigned int count){
    VertexBufferLayout::m_Elements.push_back({GL_FLOAT, count, GL_FALSE});
    VertexBufferLayout::m_Stride += count*VertexBufferElement::getSizeOfType(GL_FLOAT);
}
template<> inline
void VertexBufferLayout::push<unsigned int>(unsigned int count){
    VertexBufferLayout::m_Elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
    VertexBufferLayout::m_Stride += count*VertexBufferElement::getSizeOfType(GL_UNSIGNED_INT);
}
template<> inline
void VertexBufferLayout::push<unsigned char>(unsigned int count){
    VertexBufferLayout::m_Elements.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE});
    VertexBufferLayout::m_Stride += count*VertexBufferElement::getSizeOfType(GL_UNSIGNED_BYTE);
}
