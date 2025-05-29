#include "Scene.hpp"


Scene::Scene()
{
	// TODO: Add entities during runtime.
	// TODO: Read entities from file.
	testerMata = new SpriteRenderer();

	player = new Entity(registry.create(), registry);
	player->AddComponent<TransformComponent>(glm::vec3{ 0.f }, glm::vec3{ 0.f }, glm::vec3{ 0.f });
	player->AddComponent<NameComponent>("Player", "Default");
	player->AddComponent<SpriteComponent>();
	auto& sprite = registry.get<SpriteComponent>(player->GetHandle());
	sprite.texture = Texture2D();
	int width, height, nrChannels;
	unsigned char* data = stbi_load("Assets/Textures/awesomeface.png", &width, &height, &nrChannels, 0);
	sprite.texture.Generate(width, height, data);
	stbi_image_free(data);
	sprite.spriteRenderer = SpriteRenderer();

}

void Scene::AddEntity(const std::string& name, const std::string& tag)
{
	// By default an entity should have a name component.
	entt::entity handle = registry.create();
	registry.emplace<NameComponent>(handle, name, tag);
}

void Scene::DestroyEntity(entt::entity handle)
{
	registry.destroy(handle);
}

void Scene::RenderingSystem(Camera* activeCamera)
{
	auto view = registry.view<TransformComponent, SpriteComponent>();

	for (auto entity : view) 
	{
		auto transform = view.get<TransformComponent>(entity);
		auto sprite = view.get<SpriteComponent>(entity);

		// rendering:
		ResourceManager::getInstance()->spriteShader->use();

		// Model matrix
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(transform.Translation));
		model = glm::scale(model, glm::vec3(1.0f));
		ResourceManager::getInstance()->spriteShader->setMat4("model", model);
		ResourceManager::getInstance()->spriteShader->setMat4("projection", activeCamera->GetProjectionMatrix()); // set this in constructor or per-frame

		// Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ResourceManager::getInstance()->smileyFace->ID);
		ResourceManager::getInstance()->spriteShader->setInt("image", 0);

		// Draw
		glBindVertexArray(testerMata->quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		testerMata->DrawSprite(sprite.texture, glm::vec2(transform.Translation.x, transform.Translation.y), glm::vec2(transform.Scale.x, transform.Scale.y), transform.Rotation.x);
	}
}