#pragma once
#include "Shader.hpp"
#include "Texture2D.hpp"
#include "ResourceManager.hpp"
#include <glm-master/glm/glm.hpp>
#include <glm-master/glm/gtc/matrix_transform.hpp>
#include <glm-master/glm/gtc/type_ptr.hpp>

class SpriteRenderer
{
public:
    SpriteRenderer();
    ~SpriteRenderer();

    void DrawSprite(Texture2D& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
    unsigned int quadVAO;
private:
    Shader* shader;
    

    void initRenderData();

};

