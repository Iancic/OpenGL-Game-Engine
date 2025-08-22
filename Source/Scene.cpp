#include "precomp.h"
#include "Scene.hpp"

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

			Lua.set_function("error", [](sol::variadic_args args)
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
					Logger::Error(oss.str());
				});

			BindTypes(Lua);
			BindEntity(Lua);

			// TODO: MODULARIZE FOR MANY COMPONENTS
			Lua["Self"] = entity;
			if (registry.any_of<TransformComponent>(entity))
				Lua["Transform"] = &registry.get<TransformComponent>(entity);

			if (registry.any_of<Emitter>(entity))
				Lua["Emitter"] = &registry.get<Emitter>(entity);

			Lua.script_file(script.scriptPath);
		}

		script.onStart = script.state["Start"];
		script.onUpdate = script.state["Update"];
		script.onShutdown = script.state["Shutdown"];

		if (script.onStart.valid())
			script.onStart();
	}

	initialized = true;
}

// TODO: This is weird because I call them here manually but i go for ecs way with scripting so the scripts
// should say Emitter.Update()
void Scene::Update(float deltaTime, Input& inputSystem)
{
	// Update all existing entities with Script Component attached.
	auto view = registry.view<ScriptComponent>();
	for (auto entity : view)
	{
		auto& script = view.get<ScriptComponent>(entity);
		if (script.onUpdate.valid())
		{
			script.onUpdate(deltaTime);
		}
	}
	System_Emitter(deltaTime);
	System_Creature(deltaTime);
}

void Scene::Render(Camera* activeCamera)
{
	// TODO: This current system goes through each component and does a draw call.
	System_Sprite(activeCamera);
	System_Animation(activeCamera);

	System_Emitter_Render(activeCamera);
	System_Creature_Render();

	// TODO: Go through each component, gather rendering data from all.
	// Do one batch render for all.
	// Tilemaps, animations, sprites, particles.
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
	// TODO: MODULARIZE FOR MANY COMPONENTS

	lua.new_usertype<glm::vec3>("vec3",
		"x", &glm::vec3::x,
		"y", &glm::vec3::y,
		"z", &glm::vec3::z
	);

	sol::usertype<Emitter> emitter_type = lua.new_usertype<Emitter>("Emitter",
		sol::constructors<Emitter()>()
	);

	emitter_type["Emit"] = &Emitter::Emit;

	lua.new_usertype<TransformComponent>("Transform",
		sol::constructors<TransformComponent()>()
		//"translation", &TransformComponent::Translation,
		//"scale", &TransformComponent::Scale,
		//"rotation", &TransformComponent::Rotation
	);
	
}

void Scene::BindEntity(sol::state& lua)
{
	lua.new_usertype<entt::entity>("Self");

	
	lua.set_function("get_component", [this, &lua](entt::entity entity, const std::string& type) -> sol::object {
		if (type == "Transform" && registry.any_of<TransformComponent>(entity))
			return sol::make_object(lua, &registry.get<TransformComponent>(entity));
		return sol::nil;
		});
}

void Scene::Serialize()
{
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
			// TODO: MODULARIZE FOR MANY COMPONENTS
		}

		//out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(scenePath);
		fout << out.c_str();

		Logger::Log("Saving scene: ", scenePath);
	}
}

void Scene::Deserialize()
{
	scenePath = OpenFileDialog();
	sceneName = std::filesystem::path(scenePath).filename().string();

	Logger::Log("Loaded scene: ", scenePath);
}

void Scene::AddEntity(const std::string& name, const std::string& tag)
{
	// NOTE: When you add entities to a scene they come with a name, hierarchy and order component.
	entt::entity handle = registry.create();
	registry.emplace<NameComponent>(handle, name, tag);
	registry.emplace<HierarchyComponent>(handle);

	// Order is automatic based on the size of the registry to keep it simple.
	auto view = registry.view<OrderComponent>();
	int nextOrder = static_cast<int>(view.size());
	registry.emplace<OrderComponent>(handle, nextOrder);
}

void Scene::DestroyEntity(entt::entity handle)
{
	registry.destroy(handle);
}

void Scene::System_Sprite(Camera* activeCamera)
{
	auto view = registry.view<TransformComponent, SpriteComponent>();

	for (auto [entity, transform, sprite] : view.each())
	{
		sprite.spriteRenderer.DrawSprite(activeCamera, sprite.texture, glm::vec2{ transform.GetTranslation(transform.WorldMatrix).x, transform.GetTranslation(transform.WorldMatrix).y },
			glm::vec2{ transform.GetScale(transform.WorldMatrix).x,  transform.GetScale(transform.WorldMatrix).y },
			transform.GetRotationEuler(transform.WorldMatrix).x);
	}
}

void Scene::System_Animation(Camera* activeCamera)
{
	auto view = registry.view<TransformComponent, AnimationComponent>();

	for (auto [entity, transform, animation] : view.each())
	{
		if (animation.animations.size() != 0)
		{
			Animation currentAnim = animation.animations.at(animation.currentAnimation);
			currentAnim.Render(glm::vec2{ transform.GetTranslation(transform.WorldMatrix).x,  transform.GetTranslation(transform.WorldMatrix).y }, transform.GetScale(transform.WorldMatrix).x, transform.GetRotationEuler(transform.WorldMatrix).y);

			//currentAnim.texture.Render(currentAnim.frameInfo, glm::vec2{ transform.Translation.x, transform.Translation.y }, transform.Scale.x, transform.Rotation.x);
		}
	}
}

void Scene::System_SceneGraph()
{
	auto viewHierarchy = registry.view<TransformComponent, HierarchyComponent>();

	for (auto [entity, transform, hierarchy] : viewHierarchy.each())
	{
		// Root nodes (no parent)
		if (hierarchy.parentID == entt::null)
		{
			// For root, world = local
			transform.WorldMatrix = transform.LocalMatrix;

			// Recursively apply to children
			System_RecurseTransform(entity, transform.WorldMatrix);
		}
	}
}

void Scene::System_RecurseTransform(entt::entity parent, const glm::mat4& parentWorldMatrix)
{
	const auto& hierarchy = registry.get<HierarchyComponent>(parent);

	for (entt::entity child : hierarchy.childrenID)
	{
		if (registry.all_of<TransformComponent, HierarchyComponent>(child))
		{
			auto& childTransform = registry.get<TransformComponent>(child);

			// Compute world transform: parent world * child local
			childTransform.WorldMatrix = parentWorldMatrix * childTransform.LocalMatrix;

			// Recurse if the child has its own children
			const auto& childHierarchy = registry.get<HierarchyComponent>(child);
			if (!childHierarchy.childrenID.empty())
				System_RecurseTransform(child, childTransform.WorldMatrix);
		}
	}

}

void Scene::System_Creature(float deltaTime)
{
	const auto& view = registry.view<Creature, TransformComponent>();

	for (auto [entity, creature, transform] : view.each())
	{
		creature.Update(deltaTime, &transform);
	}
}

void Scene::System_Creature_Render()
{
	const auto& view = registry.view<Creature, TransformComponent>();

	for (auto [entity, creature, transform] : view.each())
	{
		creature.Render();
	}
}

void Scene::System_Emitter(float deltaTime)
{
	const auto& view = registry.view<Emitter, TransformComponent>();

	for (auto [entity, emitter, transform] : view.each())
	{
		emitter.Update(deltaTime, &transform);
	}
}

void Scene::System_Emitter_Render(Camera* activeCamera)
{
	const auto& view = registry.view<Emitter, TransformComponent>();

	for (auto [entity, emitter, transform] : view.each())
	{
		emitter.Render(activeCamera);
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