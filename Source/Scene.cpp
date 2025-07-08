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

		if (!script.initialized)
		{
			// Initialize the lua script with functions and types.
			script.initialized = true;
			script.state = sol::state();
			auto& Lua = script.state;

			Lua.open_libraries(sol::lib::base, sol::lib::math);

			Lua.set_function("print", [](sol::variadic_args args)
			{
				std::ostringstream oss;	
				for (auto arg : args)
				{
					sol::type t = arg.get_type();
					switch (t)
					{
						case sol::type::string:
							oss << arg.as<std::string>();
							break;
						case sol::type::number:
							oss << arg.as<double>();
							break;
						case sol::type::boolean:
							oss << (arg.as<bool>() ? "true" : "false");
							break;
						case sol::type::nil:
							oss << "nil";
							break;
						default:
							oss << "[unsupported type]";
							break;
					}
					oss << " ";
				}
				Logger::Log(oss.str());
			});

			BindTypes(Lua);
			BindEntity(Lua);

			// TODO: MODULARIZE FOR MANY COMPONENTS
			Lua["Self"] = entity;
			if (registry.any_of<TransformComponent>(entity))
				Lua["Transform"] = &registry.get<TransformComponent>(entity);

			Lua.script_file("Scripts/Default.lua");//(script.scriptPath);
		}

		script.onStart = script.state["Start"];
		script.onUpdate = script.state["Update"];
		script.onShutdown = script.state["Shutdown"];

		if (script.onStart.valid())
			script.onStart();
	}

	initialized = true;
}

void Scene::Render()
{
	CreatureSystemRender();
	//SpriteSystemRendering();
}

void Scene::Update(Input& inputSystem, float deltaTime)
{
	auto view = registry.view<ScriptComponent>();
	for (auto entity : view)
	{
		auto& script = view.get<ScriptComponent>(entity);
		if (script.onUpdate.valid())
		{
			script.onUpdate(100);
		}
	}

	// Obtain Mouse Position for shooting target
	Transform target;
	target.setPosition(inputSystem.GetMousePosition().x, inputSystem.GetMousePosition().y);
	CreatureSystemUpdate(target, deltaTime);
}

void Scene::Shutdown()
{
	auto view = registry.view<ScriptComponent>();
	for (auto entity : view)
	{
		auto& script = view.get<ScriptComponent>(entity);

		if (script.onShutdown.valid())
			script.onShutdown();
	}

	initialized = false;

	Logger::Flush();
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
	if (scenePath.empty())
	{
		Logger::Warning("Scene not loaded yet.");
	}
	else
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		std::string fileName = std::filesystem::path(scenePath).filename().string();
		out << YAML::Key << "Scene" << YAML::Value << fileName;
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		for (auto entity : entities)
		{
			// TODO: entity.Serialize();
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(scenePath);
		fout << out.c_str();

		Logger::Log("Saving scene: ", scenePath);
	}
}

void Scene::Deserialize()
{
	scenePath = OpenFileDialog();
	Logger::Log("Loaded scene: ", scenePath);
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

void Scene::CreatureSystemRender()
{
	auto view = registry.view<Creature>();
	for (auto entity : view)
	{
		auto& creature = view.get<Creature>(entity);
		creature.Render();
	}
}

void Scene::CreatureSystemUpdate(Transform& target, float deltaTime)
{
	auto view = registry.view<Creature>();
	for (auto entity : view)
	{
		auto& creature = view.get<Creature>(entity);
		creature.Update(target, deltaTime);
		creature.AI();
	}
}

void Scene::SpriteSystemRendering()
{
	auto view = registry.view<TransformComponent, SpriteComponent>();

	for (auto [entity, transform, sprite] : view.each())
	{
		auto transform = view.get<TransformComponent>(entity);
		auto sprite = view.get<SpriteComponent>(entity);
	}
}

std::string Scene::OpenFileDialog()
{
	char filename[MAX_PATH] = "";

	OPENFILENAMEA ofn = { 0 };
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = sizeof(filename);
	ofn.lpstrFilter = "Scene Files\0*.yaml;\0All Files\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileNameA(&ofn))
		return std::string(filename);
	return ""; // Cancelled or failed
};
