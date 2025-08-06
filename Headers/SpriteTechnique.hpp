#pragma once
#include "Technique.h"
#include <glm-master/glm/glm.hpp>
#include <glm-master/glm/gtc/matrix_transform.hpp>
#include <glm-master/glm/gtc/type_ptr.hpp>
#include <cassert>

#define SPRITE_TECH_MAX_QUADS 100

class SpriteTechnique : public Technique
{
public:

    SpriteTechnique();

    virtual bool Init();

    void SetTextureUnit(unsigned int TextureUnit);

    void SetQuad(int Index,
        float NDCX, float NDCY, float Width, float Height,    // tile base position and dimensions
        float u, float v, float TexWidth, float TexHeight);   // texture coordinates

    void UpdateProgram();

private:

    void ConfigUniformBlock();

    struct {
        GLuint BasePos = -1;
        GLuint WidthHeight = -1;
        GLuint TexCoords = -1;
        GLuint TexWidthHeight = -1;
    } m_quadsLoc[SPRITE_TECH_MAX_QUADS];

    struct {
        GLuint BasePos = -1;
        GLuint WidthHeight = -1;
        GLuint TexCoords = -1;
        GLuint TexWidthHeight = -1;
    } m_quads1Loc;

    struct {
        GLint BasePos = 0;
        GLint WidthHeight = 0;
        GLint TexCoords = 0;
        GLint TexWidthHeight = 0;
    } m_quadInfoOffsets;

    GLuint m_samplerLoc = -1;
    GLubyte* m_quadInfoBuffer = NULL;
    GLuint m_uniformBuffer = 0;
    GLint m_blockSize = 0;
};