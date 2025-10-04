#pragma once
#include "precomp.h"

#include "Segment.hpp"
#include "IKChain.hpp"
#include "Perlin.hpp"
#include "Vertex.hpp"
#include "CollisionSolver.hpp"
#include "TransformComponent.hpp"

class Creature
{
public:
	Creature() = default;
	Creature(int difficulty, Camera* camera);
	~Creature();

	enum DIRECTIONS
	{
		NORTH,
		SOUTH,
		WEST,
		EAST
	};

	// Creature Options:
	float movementSpeed = 225.f, circularSpeed = 2.f;
	int hitpoints = 100;
	int regen = 10;
	int weaponSlots = 3;

	int creatureLenght = 5;
	int segmentSpacing = 10;

	float deltaTimeRef; // Hold a reference for deltaTime to use everywhere

	std::vector<int> creatureShape;
	std::vector<int> segmentShape;

	bool hasGeometry = false, hasWeapons = true;
	bool AIcontrolled = false;
	

	// Leg Options:
	bool gait = true;
	std::vector<bool> isStepping;
	std::vector<float> stepTimers;
	std::vector<glm::vec2> stepStartPositions;
	std::vector<glm::vec2> stepTargetPositions;
	int legCounter = 0;
	float legWidth = 2.5f, outwardBias = 0.5f, sideOffset = 30.f, forwardOffset = 30.0f, distanceToSnap = 10.f;
	std::vector<glm::vec2> footTargets;
	std::vector<IKChain*> legs;
	glm::vec2 lastSnap{ 0 };

	int points = 0, maxPoints = 1000, level = 1; // points > maxPoints -> Level Up
	int DNA = 0; // Currency 1
	int dust = 0; // Currency 2

	glm::vec2 lastLeft;
	glm::vec2 lastRight;

	std::vector<Segment*> segments;
	int weaponsCreated = 0;

	int maxRadius;

	GLuint spineVAO, spineVBO;
	std::vector<Vertex>spineVertices;

	Camera* activeCamera = nullptr;

	// Perlin Noise Skin
	// Put them in a struct
	float spotSize = 0.f;
	float pulseTime = 0.f;
	glm::vec2 offset{0.5f, 0.5f};
	float rotationSpeed = 1.f;
	float zoomFactor = 8.0f, dynamicSpeed = 0.1f;
	glm::vec3 primaryColor = glm::vec3(0);
	glm::vec3 secondaryColor = glm::vec3(255);
	
	std::vector<BoxCollider*> colliders;

	void Update(float deltaTime, TransformComponent* transform);
	void AI();

	// AI circular motion
	float theta = 0.f, radiusAI = 55.f, center = 0.f;

	void Render();
	void RenderDebug();
	void RenderInside();

	void AddPoints(int valueArg);
	void LevelUp();

	int desiredLenght;
	void IncreaseBodyLenght();
	void DecreaseBodyLenght();

	void IncreaseSpeed(float valueArg);
	void DecreaseSpeed(float valueArg);

	void IncreaseHealth(int valueArg);
	void DecreaseHealth(int valueArg);

	void IncreaseSizeUniformly();
	void DecreaseSizeUniformly();

	void IncreaseSizeSequentially();
	void DecreaseSizeSequentially();

	void AddLeg();
	void RemoveLeg();

	int desiredSegmentSpacing;
	void AddSpacing();
	void RemoveSpacing();

	void MoveWest();
	void MoveEast();
	void MoveSouth();
	void MoveNorth();

private:

};