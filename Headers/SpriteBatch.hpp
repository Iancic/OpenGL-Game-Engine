#pragma once
#include "precomp.h"

#include "Texture2D.hpp"
#include "QuadArray.hpp"
#include "ResourceManager.hpp" 

#define SPRITE_TECH_MAX_QUADS 100
#define SPRITE_BUFFER_BINDING 0

struct Vector2f
{
    union {
        float x = 0.0f;
        float u;
    };

    union {
        float y = 0.0f;
        float v;
    };

    Vector2f()
    {
    }

    Vector2f(float f)
    {
        x = f;
        y = f;
    }

    Vector2f(float _x, float _y)
    {
        x = _x;
        y = _y;
    }

    void Print(bool endl = true) const
    {
        printf("(%f, %f)", x, y);

        if (endl) {
            printf("\n");
        }
    }

    float Length() const
    {
        float len = sqrtf(x * x + y * y);
        return len;
    }


    void Normalize()
    {
        float len = Length();

        assert(len != 0);

        x /= len;
        y /= len;
    }
};


inline Vector2f operator*(const Vector2f& l, float f)
{
    Vector2f Ret(l.x * f, l.y * f);

    return Ret;
}

class SpriteBatch
{
public:

    SpriteBatch() = default;
    SpriteBatch(const char* pFilename, int NumSpritesX, int NumSpritesY, int WindowWidth, int WindowHeight);
    ~SpriteBatch() = default;

    void Render(const std::vector<SpriteInfo>& sprites, glm::vec2 position, float size, float rotate);

    void RenderAll();

    void InitSpriteSheet(const char* pFilename);

private:

    void InitSpriteTech();

    void CalcSpriteInfo();

    void ScreenPosToNDC(float mouse_x, float mouse_y, float& ndc_x, float& ndc_y);

    // constructor params
    const char* m_pFilename = NULL;
    float m_numSpritesX = 0.0f;
    float m_numSpritesY = 0.0f;
    float m_windowWidth = 0.0f;
    float m_windowHeight = 0.0f;

    // internal params
    float m_spriteAspectRatio = 0.0f;   // height/width of the sprite in the sprite sheet
    float m_texUSize = 0.0f;  // Sprite size in tex space - U
    float m_texVSize = 0.0f;  // Sprite size in tex space - V
    float m_windowAR = 0.0f;
    float m_ndcPixelX = 0.0f; // Size of pixel in NDC space - X
    float m_ndcPixelY = 0.0f; // Size of pixel in NDC space - y
    float m_tileHeightInPixels = 0.0f;
    float m_tileWidthInPixels = 0.0f;
    float m_tileWidthNDC = 0.0f;
    float m_tileHeightNDC = 0.0f;
    Texture2D* m_pSpriteSheet = NULL;
    QuadArray* m_pQuads = NULL;

    void SetQuad(int Index,
        float NDCX, float NDCY, float Width, float Height,    // tile base position and dimensions
        float u, float v, float TexWidth, float TexHeight);   // texture coordinates

    void ConfigUniformBlock();

    void UpdateProgram();

    struct
    {
        GLuint BasePos = -1;
        GLuint WidthHeight = -1;
        GLuint TexCoords = -1;
        GLuint TexWidthHeight = -1;
    } m_quadsLoc[SPRITE_TECH_MAX_QUADS];

    struct
    {
        GLuint BasePos = -1;
        GLuint WidthHeight = -1;
        GLuint TexCoords = -1;
        GLuint TexWidthHeight = -1;
    } m_quads1Loc;

    struct 
    {
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