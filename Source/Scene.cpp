#include "Scene.hpp"

Scene::Scene()
{
	// TODO: Add entities during runtime.
	// TODO: Read entities from file.

	player = new Entity(registry.create(), registry);
	player->AddComponent<TransformComponent>(glm::vec3{ 0.f }, glm::vec3{ 0.f }, glm::vec3{ 0.f });
	player->AddComponent<NameComponent>("Player", "Default");
	player->AddComponent<SpriteComponent>();
}