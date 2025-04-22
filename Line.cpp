#include "Line.h"

Line* Line::line_Instance = nullptr;

Line::Line()
{
    shaderPtr = ResourceManager::getInstance()->lineShader;

    transformPtr = new Transform();

    transformPtr->setScale(1.f, 1.f, 1.f);

    // For a line, we just need two vertices
    vertices = new std::vector<float>
    {
        0.0f, 0.0f, 0.0f,  // start point
        1.f, 0.0f, 0.0f    // end point
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(float), vertices->data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Line::Render(glm::vec3 startPosition, glm::vec3 endPosition, glm::vec4 color)
{
    glUseProgram(shaderPtr->ID);

    // Local Space: Transform (Model)
    globalTransform = glm::mat4(1.f);

    vertices = new std::vector<float>
    {
        startPosition.x, startPosition.y, startPosition.z,
        endPosition.x, endPosition.y, endPosition.z
    };

    // Update VBO with the new vertices (line start and end)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(float), vertices->data(), GL_STATIC_DRAW);

    MVP = activeCamera->projectionMatrix * activeCamera->view * globalTransform;
    shaderPtr->setMat4("MVP", MVP);
    shaderPtr->setVec4("ARGB", color);

    // Draw Line
    glLineWidth(2.0f);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);

    delete vertices;
    glBindVertexArray(0);
}
