#pragma once

#include <GL/glew.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

class Renderer{
private:

public:
    void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void clear() const;

}; 