#pragma once
#include <cstdint>
#include <cassert>
#include <vector>
#include <glad/glad.h>
#include <glm-master/glm/glm.hpp>
#include <glm-master/glm/gtc/matrix_transform.hpp>
#include <glm-master/glm/gtc/type_ptr.hpp>

class QuadArray
{
public:
    QuadArray(int MaxNumQuads);

    ~QuadArray();

    void Render(uint8_t NumQuads = 0);

private:

    void CreateVertexBuffer();
    void CreateQuadIDBuffer();

    enum BUFFER_TYPE {
        POS_VB = 0,
        QUAD_ID_VB = 1,
        NUM_BUFFERS = 2
    };

    uint8_t m_maxNumQuads = 0;
    GLuint m_VAO = -1;
    GLuint m_Buffers[NUM_BUFFERS] = { 0 };
};
