#include "Scene.hpp"


Scene::Scene()
{

}

void Scene::Start()
{
	auto view = registry.view<ScriptComponent>();
	for (auto entity : view)
	{
		auto& script = view.get<ScriptComponent>(entity);

		// We initialize the scripts.
		script.state = sol::state();
		auto& Lua = script.state;

		Lua.open_libraries(sol::lib::base, sol::lib::math);

		Lua.set_function("print", [](sol::variadic_args args)
		{
			
		});

		BindTypes(Lua);
		BindEntity(Lua);

		// TODO: MODULARIZE FOR MANY COMPONENTS
		Lua["Self"] = entity;
		if (registry.any_of<TransformComponent>(entity))
			Lua["Transform"] = &registry.get<TransformComponent>(entity);

		Lua.script_file(script.scriptPath);

		script.onStart = Lua["Start"];
		script.onUpdate = Lua["Update"];
		script.onShutdown = Lua["Shutdown"];

		if (script.onStart.valid())
			script.onStart();
	}

	initialized = true;
}

void Scene::Render()
{

}

void Scene::Update()
{
	auto view = registry.view<ScriptComponent>();
	for (auto entity : view)
	{
		auto& script = view.get<ScriptComponent>(entity);
		if (script.onUpdate.valid())
			script.onUpdate(100);
	}
}

void Scene::Shutdown()
{
}

void Scene::BindTypes(sol::state& lua)
{
	lua.new_usertype<glm::vec3>("vec3",
		"x", &glm::vec3::x,
		"y", &glm::vec3::y,
		"z", &glm::vec3::z
	);

	// TODO: MODULARIZE FOR MANY COMPONENTS
	lua.new_usertype<TransformComponent>("Transform",
		"translation", &TransformComponent::Translation,
		"scale", &TransformComponent::Scale,
		"rotation", &TransformComponent::Rotation
	);
}

void Scene::BindEntity(sol::state& lua)
{
	lua.new_usertype<entt::entity>("Self");

	// TODO: MODULARIZE FOR MANY COMPONENTS
	lua.set_function("get_component", [this, &lua](entt::entity entity, const std::string& type) -> sol::object {
		if (type == "Transform" && registry.any_of<TransformComponent>(entity))
			return sol::make_object(lua, &registry.get<TransformComponent>(entity));
		return sol::nil;
		});
}

void Scene::Serialize()
{
	// TODO: yaml for components and entities.
}

void Scene::AddEntity(const std::string& name, const std::string& tag)
{
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

	for (auto [entity, transform, sprite] : view.each()) 
	{
		auto transform = view.get<TransformComponent>(entity);
		auto sprite = view.get<SpriteComponent>(entity);
	}
}