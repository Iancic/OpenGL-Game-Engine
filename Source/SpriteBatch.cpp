#include "precomp.h"
#include "SpriteBatch.hpp"

SpriteBatch::SpriteBatch(const char* pFilename, int NumSpritesX, int NumSpritesY, int WindowWidth, int WindowHeight)
{
    m_pFilename = pFilename;
    m_numSpritesX = NumSpritesX;
    m_numSpritesY = NumSpritesY;
    m_windowWidth = (float)WindowWidth;
    m_windowHeight = (float)WindowHeight;
    m_windowAR = m_windowHeight / m_windowWidth;

    int NumSprites = NumSpritesX * NumSpritesY;
    m_pQuads = new QuadArray(NumSprites);

    ConfigUniformBlock();

    InitSpriteSheet(pFilename);

    InitSpriteTech();

    CalcSpriteInfo();
}


void SpriteBatch::InitSpriteSheet(const char* pFilename)
{
    m_pSpriteSheet = new Texture2D();
    int width, height, nrChannels;
    unsigned char* data = stbi_load(pFilename, &width, &height, &nrChannels, 0);
    m_pSpriteSheet->Generate(width, height, data);
    stbi_image_free(data);
}


void SpriteBatch::InitSpriteTech()
{
    ResourceManager::getInstance()->animationShader->use();
    GLuint Location = glGetUniformLocation(ResourceManager::getInstance()->animationShader->ID, "gSampler");
    glUniform1i(Location, 0);
}


void SpriteBatch::CalcSpriteInfo()
{
    float SpriteWidth = (float)m_pSpriteSheet->Width / m_numSpritesX;
    float SpriteHeight = (float)m_pSpriteSheet->Height / m_numSpritesY;

    m_spriteAspectRatio = SpriteHeight / SpriteWidth;

    m_texUSize = 1.0f / m_numSpritesX;
    m_texVSize = 1.0f / m_numSpritesY;

    m_ndcPixelX = 2.0f / m_windowWidth;
    m_ndcPixelY = 2.0f / m_windowHeight;

    m_tileHeightInPixels = 0.0f;
    m_tileWidthInPixels = 0.0f;

    float ImageWidthToWindowWidthRatio = m_pSpriteSheet->Width / m_windowWidth;
    float ImageHeightToWindowHeightRatio = m_pSpriteSheet->Height / m_windowHeight;

    if (ImageWidthToWindowWidthRatio < ImageHeightToWindowHeightRatio) 
    {
        m_tileHeightInPixels = m_windowHeight / m_numSpritesY;
        m_tileWidthInPixels = m_tileHeightInPixels / m_spriteAspectRatio;
    }
    else 
    {
        m_tileWidthInPixels = m_windowWidth / m_numSpritesX;
        m_tileHeightInPixels = m_tileWidthInPixels * m_spriteAspectRatio;
    }

    m_tileWidthNDC = m_ndcPixelX * m_tileWidthInPixels;
    m_tileHeightNDC = m_ndcPixelY * m_tileHeightInPixels;
}


void SpriteBatch::ScreenPosToNDC(float mouse_x, float mouse_y, float& ndc_x, float& ndc_y)
{
    ndc_x = (2.0f * mouse_x) / m_windowWidth - 1.0f;
    ndc_y = (2.0f * mouse_y) / m_windowHeight - 1.0f;
}

void SpriteBatch::Render(SpriteInfo& frameInfo, glm::vec2 position, float size, float rotate)
{
    ResourceManager::getInstance()->animationShader->use();

    float gWVP[16] = 
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    GLuint wvpLoc = glGetUniformLocation(ResourceManager::getInstance()->animationShader->ID, "gWVP");
    glUniformMatrix4fv(wvpLoc, 1, GL_FALSE, gWVP);

    GLuint scaleLoc = glGetUniformLocation(ResourceManager::getInstance()->animationShader->ID, "gScale");
    glUniform1f(scaleLoc, size);

    float NDCX, NDCY;

    // Transform actual world position to screen ndc here
    ScreenPosToNDC(position.x, position.y, NDCX, NDCY);

    float TileWidthNDC = m_ndcPixelX * frameInfo.SpriteWidth;
    float TileHeightNDC = TileWidthNDC / m_spriteAspectRatio;

    float UBase = (float)frameInfo.SpriteCol * m_texUSize;
    float VBase = (float)frameInfo.SpriteRow * m_texVSize;

    float quadX = NDCX - (TileWidthNDC / 2.0f);
    float quadY = NDCY - (TileHeightNDC / 2.0f);
    printf("Drawing frame col=%d, row=%d, u=%f, v=%f\n", frameInfo.SpriteCol, frameInfo.SpriteRow, UBase, VBase);
    SetQuad(4, quadX, quadY, TileWidthNDC, TileHeightNDC, UBase, VBase, m_texUSize, m_texVSize);
    
    UpdateProgram();

    m_pSpriteSheet->Bind();
    m_pQuads->Render();
}

// TODO: This one may be usefull for a tilemap?
void SpriteBatch::RenderAll()
{
    /*
    ResourceManager::getInstance()->animationShader->use();

    for (int h = 0; h < m_numSpritesY; h++) 
    {
        for (int w = 0; w < m_numSpritesX; w++)
        {
            int TileIndex = h * m_numSpritesX + w;

            float PosX = w * m_tileWidthInPixels;
            float PosY = h * m_tileHeightInPixels;

            float NDCX, NDCY;
            ScreenPosToNDC(PosX, PosY, NDCX, NDCY);

            float UBase = w * m_texUSize;
            float VBase = h * m_texVSize;

            SetQuad(TileIndex, NDCX, NDCY, m_tileWidthNDC, m_tileHeightNDC, UBase, VBase, m_texUSize, m_texVSize);
        }
    }

    UpdateProgram();
    m_pSpriteSheet->Bind();
    m_pQuads->Render();
    */
}

void SpriteBatch::RenderSingleSprite(SpriteInfo& frameInfo, glm::vec2 position, float size, float rotate)
{
    auto shader = ResourceManager::getInstance()->spriteShader;
    shader->use();

    // Set transform uniforms (you can build your own MVP or just pass position/size)
    float gWVP[16] = { /* Identity or your matrix here */ };
    GLuint wvpLoc = glGetUniformLocation(shader->ID, "gWVP");
    glUniformMatrix4fv(wvpLoc, 1, GL_FALSE, gWVP);

    GLuint scaleLoc = glGetUniformLocation(shader->ID, "gScale");
    glUniform1f(scaleLoc, size);

    // Calculate UV coordinates for this frame
    float u = frameInfo.SpriteCol * m_texUSize;
    float v = frameInfo.SpriteRow * m_texVSize;
    float uSize = m_texUSize;
    float vSize = m_texVSize;

    GLuint uLoc = glGetUniformLocation(shader->ID, "uBase");
    GLuint vLoc = glGetUniformLocation(shader->ID, "vBase");
    GLuint uSizeLoc = glGetUniformLocation(shader->ID, "uSize");
    GLuint vSizeLoc = glGetUniformLocation(shader->ID, "vSize");

    glUniform1f(uLoc, u);
    glUniform1f(vLoc, v);
    glUniform1f(uSizeLoc, uSize);
    glUniform1f(vSizeLoc, vSize);

    m_pSpriteSheet->Bind();

    // Bind a simple VAO with a single quad vertex data (position + texcoords),
    // or generate it on the fly here if you want, then:
    glBindVertexArray(m_pQuads->m_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void SpriteBatch::SetQuad(int Index, float NDCX, float NDCY, float Width, float Height, float u, float v, float TexWidth, float TexHeight)
{
    Vector2f* pBasePos = (Vector2f*)(m_quadInfoBuffer + m_quadInfoOffsets.BasePos);
    Vector2f* pWidthHeight = (Vector2f*)(m_quadInfoBuffer + m_quadInfoOffsets.WidthHeight);
    Vector2f* pTexCoords = (Vector2f*)(m_quadInfoBuffer + m_quadInfoOffsets.TexCoords);
    Vector2f* pTexWidthHeight = (Vector2f*)(m_quadInfoBuffer + m_quadInfoOffsets.TexWidthHeight);

    pBasePos[Index].x = NDCX;
    pBasePos[Index].y = NDCY;
    pWidthHeight[Index].x = Width;
    pWidthHeight[Index].y = Height;
    pTexCoords[Index].x = u;
    pTexCoords[Index].y = v;
    pTexWidthHeight[Index].x = TexWidth;
    pTexWidthHeight[Index].y = TexHeight;  
}

void SpriteBatch::ConfigUniformBlock()
{
    GLuint BlockIndex = glGetUniformBlockIndex(ResourceManager::getInstance()->animationShader->ID, "QuadInfo");

    glUniformBlockBinding(ResourceManager::getInstance()->animationShader->ID, SPRITE_BUFFER_BINDING, BlockIndex);

    glGetActiveUniformBlockiv(ResourceManager::getInstance()->animationShader->ID, BlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &m_blockSize);

    m_quadInfoBuffer = (GLubyte*)malloc(m_blockSize);

    const GLchar* Names[] = { "BasePos", "WidthHeight", "TexCoords", "TexWidthHeight" };
    GLuint Indices[4] = { 0 };
    glGetUniformIndices(ResourceManager::getInstance()->animationShader->ID, 4, Names, Indices);

    GLint Offsets[4];
    glGetActiveUniformsiv(ResourceManager::getInstance()->animationShader->ID, 4, Indices, GL_UNIFORM_OFFSET, Offsets);

    m_quadInfoOffsets.BasePos = Offsets[0];
    m_quadInfoOffsets.WidthHeight = Offsets[1];
    m_quadInfoOffsets.TexCoords = Offsets[2];
    m_quadInfoOffsets.TexWidthHeight = Offsets[3];

    glGenBuffers(1, &m_uniformBuffer);
}

void SpriteBatch::UpdateProgram()
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, m_blockSize, m_quadInfoBuffer, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, SPRITE_BUFFER_BINDING, m_uniformBuffer);
}