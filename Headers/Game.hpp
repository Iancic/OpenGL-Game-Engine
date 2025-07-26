#pragma once
#include "Program.hpp"
#include "Utilities.hpp"
#include "Scene.hpp"

class Game : public Program
{

private:
	static Game* game_Instance;

	Game() = default;
	Game(const char* title) : Program(title) {}

public:
	// Prevent copying or assignment 
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;

	// Get the instance
	static Game* getInstance(const char* title = "My Game")
	{
		if (game_Instance == nullptr)
		{
			game_Instance = new Game(title);
		}

		return game_Instance;
	}

	~Game() {}

	void Init() override;
	void Shutdown() override;
	void Render() override;
	void HandleInput() override;
	void Update() override;

	static inline Camera* activeCamera = nullptr;

	Scene* activeScene = nullptr;
	std::vector<Scene*> scenes;

private:

};