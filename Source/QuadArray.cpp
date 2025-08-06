#include "QuadArray.hpp"

#define POSITION_LOCATION 0
#define PRIM_ID_LOCATION  1
#define NUM_VERTICES 6
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0])) // OGLDev Utility Macro

QuadArray::QuadArray(uint8_t MaxNumQuads)
{
    m_maxNumQuads = MaxNumQuads;

    glGenVertexArrays(1, &m_VAO);

    glBindVertexArray(m_VAO);

    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

    CreateVertexBuffer();

    CreateQuadIDBuffer();

    // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);
}


QuadArray::~QuadArray()
{

}


void QuadArray::CreateVertexBuffer()
{
    glm::vec2 vertices[] = { glm::vec2(0.0f, 0.0f),   // bottom left
                            glm::vec2(0.0f, 1.0f),   // top left
                            glm::vec2(1.0f, 1.0f),   // top right
                            glm::vec2(0.0f, 0.0f),   // bottom left
                            glm::vec2(1.0f, 1.0f),   // top right
                            glm::vec2(1.0f, 0.0f) }; // bottom right

    std::vector<glm::vec2> vertices_vec;
    vertices_vec.resize(m_maxNumQuads * NUM_VERTICES);

    for (uint8_t i = 0; i < m_maxNumQuads; i++) 
    {
        for (int j = 0; j < NUM_VERTICES; j++) 
        {
            vertices_vec[i * NUM_VERTICES + j] = vertices[j];
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, vertices_vec.size() * sizeof(glm::vec2), &vertices_vec[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);
}


void QuadArray::CreateQuadIDBuffer()
{
    std::vector<GLuint> quad_id_vec;
    quad_id_vec.resize(m_maxNumQuads * NUM_VERTICES);

    for (uint8_t i = 0; i < m_maxNumQuads; i++) 
    {
        for (int j = 0; j < NUM_VERTICES; j++) 
        {
            quad_id_vec[i * NUM_VERTICES + j] = i;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[QUAD_ID_VB]);
    glBufferData(GL_ARRAY_BUFFER, quad_id_vec.size() * sizeof(GLuint), &quad_id_vec[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(PRIM_ID_LOCATION);
    glVertexAttribIPointer(PRIM_ID_LOCATION, 1, GL_UNSIGNED_INT, 0, 0);
}


void QuadArray::Render(uint8_t NumQuads)
{
    assert(NumQuads <= m_maxNumQuads);

    if (NumQuads == 0) {
        NumQuads = m_maxNumQuads;
    }

    glBindVertexArray(m_VAO);

    glDrawArrays(GL_TRIANGLES, 0, NumQuads * NUM_VERTICES);

    glBindVertexArray(0);
}