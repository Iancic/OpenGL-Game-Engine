#include "SpriteTechnique.hpp"

#define SPRITE_BUFFER_BINDING 0

SpriteTechnique::SpriteTechnique()
{
}

bool SpriteTechnique::Init()
{
    /*
    if (!Technique::Init()) {
        return false;
    }

    if (!AddShader(GL_VERTEX_SHADER, "Assets/Shaders/vAnim.glsl")) {
        return false;
    }

    if (!AddShader(GL_FRAGMENT_SHADER, "Assets/Shaders/fAnim.glsl")) {
        return false;
    }

    if (!Finalize()) {
        return false;
    }

    m_samplerLoc = GetUniformLocation("gSampler");

    ConfigUniformBlock();
    */
    return true;
}


void SpriteTechnique::ConfigUniformBlock()
{
    GLuint BlockIndex = glGetUniformBlockIndex(m_shaderProg, "QuadInfo");

    glUniformBlockBinding(m_shaderProg, SPRITE_BUFFER_BINDING, BlockIndex);
    printf("BlockIndex %d\n", BlockIndex);

    glGetActiveUniformBlockiv(m_shaderProg, BlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &m_blockSize);

    printf("Size %d\n", m_blockSize);

    m_quadInfoBuffer = (GLubyte*)malloc(m_blockSize);

    const GLchar* Names[] = { "BasePos", "WidthHeight", "TexCoords", "TexWidthHeight" };
    GLuint Indices[4] = { 0 };
    glGetUniformIndices(m_shaderProg, 4, Names, Indices);

    GLint Offsets[4];
    glGetActiveUniformsiv(m_shaderProg, 4, Indices, GL_UNIFORM_OFFSET, Offsets);

    m_quadInfoOffsets.BasePos = Offsets[0];
    m_quadInfoOffsets.WidthHeight = Offsets[1];
    m_quadInfoOffsets.TexCoords = Offsets[2];
    m_quadInfoOffsets.TexWidthHeight = Offsets[3];

    for (uint8_t i = 0; i < 4; i++) {
        printf("%s: %d %d\n", Names[i], Indices[i], Offsets[i]);
    }

    glGenBuffers(1, &m_uniformBuffer);
    printf("Uniform buffer %d\n", m_uniformBuffer);
}


void SpriteTechnique::SetTextureUnit(unsigned int TextureUnit)
{
    glUniform1i(m_samplerLoc, TextureUnit);
}


void SpriteTechnique::SetQuad(int Index,
    float NDCX, float NDCY, float Width, float Height,
    float u, float v, float TexWidth, float TexHeight)
{
    assert(Index < SPRITE_TECH_MAX_QUADS);

    glm::vec2* pBasePos = (glm::vec2*)(m_quadInfoBuffer + m_quadInfoOffsets.BasePos);
    glm::vec2* pWidthHeight = (glm::vec2*)(m_quadInfoBuffer + m_quadInfoOffsets.WidthHeight);
    glm::vec2* pTexCoords = (glm::vec2*)(m_quadInfoBuffer + m_quadInfoOffsets.TexCoords);
    glm::vec2* pTexWidthHeight = (glm::vec2*)(m_quadInfoBuffer + m_quadInfoOffsets.TexWidthHeight);

    pBasePos[Index].x = NDCX;
    pBasePos[Index].y = NDCY;
    pWidthHeight[Index].x = Width;
    pWidthHeight[Index].y = Height;
    pTexCoords[Index].x = u;
    pTexCoords[Index].y = v;
    pTexWidthHeight[Index].x = TexWidth;
    pTexWidthHeight[Index].y = TexHeight;
}


void SpriteTechnique::UpdateProgram()
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, m_blockSize, m_quadInfoBuffer, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, SPRITE_BUFFER_BINDING, m_uniformBuffer);
}