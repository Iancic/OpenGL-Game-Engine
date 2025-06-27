#pragma once
#include "entt/entt.hpp"

#include "NameComponent.hpp"
#include "TransformComponent.hpp"
#include "SpriteComponent.hpp"
#include "ScriptComponent.hpp"
#include "AudioComponent.hpp"
#include "EmitterComponent.hpp"
#include "TextComponent.hpp"

class Entity
{
public:
	Entity(entt::entity handle, entt::registry& registry)
		: handle(handle), registry(registry) {
	};

	// Utility: Remove & Add Component
	template<typename T, typename... Args>
	T& AddComponent(Args&&... args)
	{
		return registry.emplace<T>(handle, std::forward<Args>(args)...);
	}

	template<typename T>
	void RemoveComponent()
	{
		registry.remove<T>(handle);
	}

	// Getters
	template<typename T>
	T& GetComponent()
	{
		return registry.get<T>(handle);
	}

	entt::entity GetHandle() const
	{
		return handle;
	}

	// Utilities
	template<typename T>
	bool HasComponent()
	{
		return registry.any_of<T>(handle);
	};

	bool IsValid() const
	{
		return registry.valid(handle);
	};
	
private:
	entt::entity handle;
	entt::registry& registry;
};