#include "SpriteBatch.hpp"


SpriteBatch::SpriteBatch(const char* pFilename, int NumSpritesX, int NumSpritesY, int WindowWidth, int WindowHeight)
{
    m_pFilename = pFilename;
    m_numSpritesX = (float)NumSpritesX;
    m_numSpritesY = (float)NumSpritesY;
    m_windowWidth = (float)WindowWidth;
    m_windowHeight = (float)WindowHeight;
    m_windowAR = m_windowHeight / m_windowWidth;

    uint8_t NumSprites = NumSpritesX * NumSpritesY;
    m_pQuads = new QuadArray(NumSprites);

    ConfigUniformBlock();

    InitSpriteSheet(pFilename);

    InitSpriteTech();

    CalcSpriteInfo();
    
    
}


void SpriteBatch::InitSpriteSheet(const char* pFilename)
{
    stbi_set_flip_vertically_on_load(true);
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
    float SpriteWidth = (float)m_pSpriteSheet->Width * 5 / m_numSpritesX;
    float SpriteHeight = (float)m_pSpriteSheet->Height * 5 / m_numSpritesY;

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

void SpriteBatch::Render(const std::vector<SpriteInfo>& sprites)
{
    ResourceManager::getInstance()->animationShader->use();

    float scale = 6.0f;  // example: 2x bigger
    float gWVP[16] = {
        scale, 0.0f, 0.0f, 0.0f,
        0.0f, scale, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    GLuint wvpLoc = glGetUniformLocation(ResourceManager::getInstance()->animationShader->ID, "gWVP");
    glUniformMatrix4fv(wvpLoc, 1, GL_FALSE, gWVP);

    GLuint scaleLoc = glGetUniformLocation(ResourceManager::getInstance()->animationShader->ID, "gScale");
    glUniform1f(scaleLoc, scale);

    float NDCX, NDCY;

    for (int SpriteIndex = 0; SpriteIndex < sprites.size(); SpriteIndex++) {

        const SpriteInfo& Info = sprites[SpriteIndex];

        ScreenPosToNDC((float)Info.PixelX, (float)Info.PixelY, NDCX, NDCY);

        float TileWidthNDC = m_ndcPixelX * Info.SpriteWidth;
        float TileHeightNDC = TileWidthNDC / m_spriteAspectRatio;

        float UBase = (float)Info.SpriteCol * m_texUSize;
        float VBase = (float)Info.SpriteRow * m_texVSize;

        // TODO: THIS HERE IS HARDCODED
       // {
            int testIndex = 0;  // first quad

            float centerX = -0.1f;  // NDC center X
            float centerY = -0.1f;  // NDC center Y

            float biggerSize = 0.1f;  // about 30% of the screen size (NDC)

           // float uBase = 0.0f;      // start at left of texture atlas (change as needed)
            //float vBase = 0.0f;      // start at top of texture atlas
            float uSize = 1.0f / m_numSpritesX;  // one sprite width in UV
            float vSize = 1.0f / m_numSpritesY;  // one sprite height in UV

            SetQuad(testIndex, centerX, centerY, biggerSize, biggerSize, UBase, VBase, uSize, vSize);
       // }

        // TODO: THIS IS HOW IT SHOULD WORK PROPERLY
        //SetQuad(SpriteIndex, NDCX, NDCY, TileWidthNDC, TileHeightNDC, UBase, VBase, m_texUSize, m_texVSize);
    }

    UpdateProgram();

    m_pSpriteSheet->Bind();
    m_pQuads->Render((int)sprites.size());
}

void SpriteBatch::RenderAll()
{
    ResourceManager::getInstance()->animationShader->use();

    for (uint8_t h = 0; h < (uint8_t)m_numSpritesY; h++) 
    {
        for (uint8_t w = 0; w < (uint8_t)m_numSpritesX; w++)
        {
            uint8_t TileIndex = h * (uint8_t)m_numSpritesX + w;

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
}

void SpriteBatch::SetQuad(int Index, float NDCX, float NDCY, float Width, float Height, float u, float v, float TexWidth, float TexHeight)
{
    //assert(Index < SPRITE_TECH_MAX_QUADS);

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

    printf("[SetQuad %d] Pos: %.2f,%.2f | Size: %.2f,%.2f | UV: %.2f,%.2f | UVSize: %.2f,%.2f\n",
        Index, NDCX, NDCY, Width, Height, u, v, TexWidth, TexHeight);
}

void SpriteBatch::ConfigUniformBlock()
{
    GLuint BlockIndex = glGetUniformBlockIndex(ResourceManager::getInstance()->animationShader->ID, "QuadInfo");

    glUniformBlockBinding(ResourceManager::getInstance()->animationShader->ID, SPRITE_BUFFER_BINDING, BlockIndex);
    printf("BlockIndex %d\n", BlockIndex);

    glGetActiveUniformBlockiv(ResourceManager::getInstance()->animationShader->ID, BlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &m_blockSize);

    printf("Size %d\n", m_blockSize);

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

    for (uint8_t i = 0; i < 4; i++) {
        printf("%s: %d %d\n", Names[i], Indices[i], Offsets[i]);
    }

    glGenBuffers(1, &m_uniformBuffer);
    printf("Uniform buffer %d\n", m_uniformBuffer);
}

void SpriteBatch::UpdateProgram()
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, m_blockSize, m_quadInfoBuffer, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, SPRITE_BUFFER_BINDING, m_uniformBuffer);
}