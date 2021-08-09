#pragma once

class VertexBuffer{
    private:
        unsigned int m_RendererID;

    public:
        VertexBuffer(const float* data, unsigned int size);
        ~VertexBuffer();

        void bind() const;
        void unbind() const;
};