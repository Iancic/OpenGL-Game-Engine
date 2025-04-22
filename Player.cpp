#include "Player.h"

Player::Player(int difficulty, Camera* camera) 
{

	desiredSegmentSpacing = segmentSpacing;

	srand(time(NULL));

	glGenBuffers(1, &spineVBO);
	glGenVertexArrays(1, &spineVAO);

	glBindVertexArray(spineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, spineVBO);

	// Attribute 0
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);

	// Attribute 1
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(1);

	// Unbind to keep state clean
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Creature Lenght
	int baseLenght = 3;
	creatureLenght = difficulty + baseLenght;
	desiredLenght = creatureLenght;
	// Creature Width
	segmentSpacing = 10;
	int baseCreatureWidth = 8 + difficulty;

	//int shape = rand() % 2; // Exponential or Normal
	int shape = 1;

	if (shape) // Normal
	{
		for (int i = 0; i <= creatureLenght; i++)
		{
			creatureShape.push_back(baseCreatureWidth);
		}
	}
	else // Exponential 
	{
		for (int i = 0; i <= creatureLenght; i++)
		{
			creatureShape.push_back(i + baseCreatureWidth);
		}
	}

	// Creature Segment Shape
	int mixed = rand() % 2; // Does it have mixed or uniform shape?

	mixed = 0;

	if (mixed && shape) // Random Shapes For Creature
	{
		for (int i = 0; i <= creatureLenght; i++)
		{
			int shapeType = rand() % 3 + 3;
			segmentShape.push_back(shapeType);
		}
	}
	else // One Shape For The Whole Creature
	{
		//int shapeType = rand() % 3 + 3;
		int shapeType = 360;

		for (int i = 0; i <= creatureLenght; i++)
		{
			segmentShape.push_back(shapeType);
		}
	}


	activeCamera = camera;

	// Create Segments
	for (int i = 0; i <= creatureLenght; i++)
	{
		if (i == 0)
			segments.push_back(new Segment(NULL, segmentShape[i], creatureShape[i], segmentSpacing));
		else
			segments.push_back(new Segment(segments[i - 1], segmentShape[i], creatureShape[i], segmentSpacing));
	}

	for (int i = 0; i <= creatureLenght; i += 2)
	{
		if (weaponsCreated < weaponSlots)
		{
			weapons.push_back(new BaseWeapon(&segments[i]->transform, activeCamera));
			weaponsCreated++;
		}
	}

	// Spawn Position
	float positionX = 0;

	for (int i = creatureLenght; i >= 0; i--)
	{
		segments[i]->transform.position.y = 50;
		segments[i]->transform.position.x = positionX;
		positionX += segmentSpacing;
	}

	maxRadius = segments[0]->segmentRadius;

	// Leg Counter is used for initialization only
	for (int i = 0; i < legCounter; i++)
	{
		legs.push_back(new IKChain(glm::vec2(0, 0)));
		footTargets.push_back(glm::vec2(0, 0)); // Init empty target
	}

	spriteRenderer = new SpriteRenderer();
}

Player::~Player()
{

}

void Player::Shoot()
{
	for (auto& element : weapons)
	{
		element->Shoot();
	}
}

void Player::Update(Transform target, float deltaTime)
{
	if (points >= maxPoints) LevelUp();

	// Update each segment
	// Step 1: Update segment positions
	for (auto& segment : segments)
	{
		segment->Update(); // Your motion/target logic
	}

	// Step 2: Apply constraints in order (root to tip)
	for (size_t i = 1; i < segments.size(); ++i)
	{
		segments[i]->Constraint();
	}

	// Optional Step 3: Extra pass from tip to root for stability (e.g., for angle constraints)
	for (size_t i = segments.size() - 1; i > 0; --i)
	{
		segments[i]->Constraint(); // Or segments[i-1] if needed
	}

	// Update each weapons
	/*
	for (auto& element : weapons)
	{
		element->Update(target, deltaTime);
	}
	*/

	// Direction vectors
	for (int i = 0; i < creatureLenght; i++)
	{
		segments[i]->direction = {
			segments[i + 1]->transform.position.x - segments[i]->transform.position.x,
			segments[i + 1]->transform.position.y - segments[i]->transform.position.y };
	}

	for (int i = 0; i < legs.size(); i++)
	{
		Segment* seg = segments[i];
		glm::vec2 bodyPos = seg->transform.position;
		glm::vec2 forward = glm::normalize(seg->direction);
		glm::vec2 right = glm::normalize(glm::vec2(-forward.y, forward.x));

		float sideSign = (i % 2 == 0) ? 1.0f : -1.0f; // alternate legs

		float sideOffset = seg->segmentRadius * sideSign;

		glm::vec2 base = bodyPos + right * sideOffset;

		// Diagonal target = forward + sideways component
		glm::vec2 diagonalDir = glm::normalize(forward + right * sideSign * outwardBias);
		glm::vec2 idealFootTarget = base + diagonalDir * forwardOffset;

		if (glm::distance(footTargets[i], idealFootTarget) > distanceToSnap)
			footTargets[i] = idealFootTarget;

		legs[i]->SolveIK(footTargets[i].x, footTargets[i].y, base.x, base.y);
	}
}

bool Player::HandleInput(float deltaTime, SDL_Event eventArg)
{
	// Joysticks
	if (eventArg.type == SDL_JOYAXISMOTION)
	{
		if (eventArg.jaxis.which == 0) // Check which joystick moved
		{
			// Example of smooth joystick axis handling:
			float deadZone = 0.1f; // A threshold to avoid small, unintentional movements

			if (eventArg.jaxis.axis == 0) // X axis
			{
				float axisValue = eventArg.jaxis.value / 32767.0f; // Normalize between -1 and 1
				if (fabs(axisValue) > deadZone)
				{
					axisValue = (fabs(axisValue) - deadZone) / (1.0f - deadZone); // Smooth input response
					segments[0]->transform.position.x += axisValue * movementSpeed * deltaTime;
				}
			}

			if (eventArg.jaxis.axis == 1) // Y axis
			{
				float axisValue = eventArg.jaxis.value / 32767.0f; // Normalize between -1 and 1
				if (fabs(axisValue) > deadZone)
				{
					axisValue = (fabs(axisValue) - deadZone) / (1.0f - deadZone); // Smooth input response
					segments[0]->transform.position.y += axisValue * movementSpeed * deltaTime;
				}
			}

		}
	}

	const Uint8* keyState = SDL_GetKeyboardState(NULL);

	// Movement WASD:
	bool inputPressed = false;

	if (keyState[SDL_SCANCODE_W])
	{
		segments[0]->transform.position.y += movementSpeed * deltaTime;
		inputPressed = true;
	}
	if (keyState[SDL_SCANCODE_S])
	{
		segments[0]->transform.position.y -= movementSpeed * deltaTime;
		inputPressed = true;
	}
	if (keyState[SDL_SCANCODE_D])
	{
		segments[0]->transform.position.x += movementSpeed * deltaTime;
		inputPressed = true;
	}
	if (keyState[SDL_SCANCODE_A])
	{
		segments[0]->transform.position.x -= movementSpeed * deltaTime;
		inputPressed = true;
	}

	return inputPressed;

	if (eventArg.type == SDL_KEYDOWN)
	{
		if (!keyPressedOnce[eventArg.key.keysym.scancode])
		{
			if (eventArg.key.keysym.scancode == SDL_SCANCODE_SPACE)
				Shoot();
			if (eventArg.key.keysym.scancode == SDL_SCANCODE_U)
			{
				for (int i = 0; i <= 100; i ++)
					LevelUp();
			}
		}
	}

	if (eventArg.type == SDL_KEYUP)
	{
		keyPressedOnce[eventArg.key.keysym.scancode] = false; // Reset on key release
	}

	return false;
}

void Player::RenderOutline()
{
	if (hasGeometry)
	{
		for (auto& element : segments)
		{
			element->Render(activeCamera);
		}
	}

	/*
	if (hasWeapons)
	{
		for (auto& element : weapons)
		{
			element->Render(activeCamera);
		}
	}
	*/

	if (hasSpine)
	{
		Spine();
	}

	for (int i = 0; i < legs.size(); i++)
		legs[i]->Render(activeCamera);
}

void Player::RenderSprites()
{
	ResourceManager::getInstance()->spriteShader->use();

	// Model matrix
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(segments[0]->transform.position));
	model = glm::scale(model, glm::vec3(1.0f));
	ResourceManager::getInstance()->spriteShader->setMat4("model", model);

	// Projection
	glm::mat4 projection = glm::ortho(
		0.0f,
		static_cast<float>(SCREEN_WIDTH),
		static_cast<float>(SCREEN_HEIGHT),
		0.0f,
		-1.0f, 1.0f
	);
	ResourceManager::getInstance()->spriteShader->setMat4("projection", projection); // set this in constructor or per-frame

	// Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ResourceManager::getInstance()->smileyFace->ID);
	ResourceManager::getInstance()->spriteShader->setInt("image", 0);

	// Draw
	glBindVertexArray(spriteRenderer->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	spriteRenderer->DrawSprite(*ResourceManager::getInstance()->smileyFace, glm::vec2(0.0f, 0.0f), glm::vec2(100.0f, 100.0f), 0.f, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Player::Spine()
{
	for (int i = 0; i < creatureLenght - 1; i++)
	{
		// Get the start and end points for the current and next segment
		glm::vec2 startPos = segments[i]->transform.position;//- activeCamera->transform->position;
		glm::vec2 endPos = segments[i + 1]->transform.position;//- activeCamera->transform->position;

		glm::vec2 startDir = segments[i]->direction;
		glm::vec2 endDir = segments[i + 1]->direction;

		// Start Left & Right Positions:
		glm::vec2 leftPointStart = glm::vec2(startDir.y, -startDir.x); // Rotate counter-clockwise by 90 degrees
		glm::vec2 rightPointStart = glm::vec2(-startDir.y, startDir.x); // Rotate clockwise by 90 degrees

		leftPointStart = glm::normalize(leftPointStart);
		rightPointStart = glm::normalize(rightPointStart);

		// End Left & Right Positions:
		glm::vec2 leftPointEnd = glm::vec2(endDir.y, -endDir.x);
		glm::vec2 rightPointEnd = glm::vec2(-endDir.y, endDir.x);

		leftPointEnd = glm::normalize(leftPointEnd);
		rightPointEnd = glm::normalize(rightPointEnd);

		// Calculate the final positions using the segment radius
		glm::vec2 leftA = startPos + leftPointStart * glm::vec2(segments[i]->segmentRadius);
		glm::vec2 rightA = startPos + rightPointStart * glm::vec2(segments[i]->segmentRadius);

		// Compute average direction between current and next segment
		glm::vec2 avgDir = glm::normalize(startDir + endDir);

		// Rotate 90 degrees to get perpendicular vectors
		glm::vec2 avgLeft = glm::normalize(glm::vec2(avgDir.y, -avgDir.x));
		glm::vec2 avgRight = glm::normalize(glm::vec2(-avgDir.y, avgDir.x));

		// Average the radii too
		float avgRadius = 0.5f * (segments[i]->segmentRadius + segments[i + 1]->segmentRadius);

		// Now apply the average at the joint position (endPos)
		glm::vec2 leftB = endPos + avgLeft * avgRadius;
		glm::vec2 rightB = endPos + avgRight * avgRadius;

		ResourceManager::getInstance()->lineShader->use();
		ResourceManager::getInstance()->lineShader->setMat4("projection", activeCamera->GetProjectionMatrix());
		ResourceManager::getInstance()->lineShader->setMat4("view", activeCamera->GetViewMatrix());
		glm::mat4 model = glm::mat4(1.0f);
		ResourceManager::getInstance()->lineShader->setMat4("model", model);

		// Triangle 1
		spineVertices.push_back({ glm::vec3(leftA, 0.f) , glm::vec4(0, 255, 255, 255) });
		spineVertices.push_back({ glm::vec3(leftB, 0.f) , glm::vec4(0, 255, 255, 255) });
		spineVertices.push_back({ glm::vec3(startPos, 0.f) , glm::vec4(0, 255, 255, 255) });

		// Triangle 2
		spineVertices.push_back({ glm::vec3(rightA, 0.f) , glm::vec4(0, 255, 255, 255) });
		spineVertices.push_back({ glm::vec3(rightB, 0.f) , glm::vec4(0, 255, 255, 255) });
		spineVertices.push_back({ glm::vec3(startPos, 0.f) , glm::vec4(0, 255, 255, 255) });

		// Triangle 3
		spineVertices.push_back({ glm::vec3(rightB, 0.f) , glm::vec4(0, 255, 255, 255) });
		spineVertices.push_back({ glm::vec3(leftB, 0.f) , glm::vec4(0, 255, 255, 255) });
		spineVertices.push_back({ glm::vec3(startPos, 0.f) , glm::vec4(0, 255, 255, 255) });

		if (i + 1 < creatureLenght) 
		{
			glm::vec2 nextDir = segments[i + 1]->direction;
			glm::vec2 nextPos = segments[i + 1]->transform.position;

			// Calculate perpendicular directions for next segment
			glm::vec2 nextRight = glm::normalize(glm::vec2(-nextDir.y, nextDir.x));
			glm::vec2 nextLeft = glm::normalize(glm::vec2(nextDir.y, -nextDir.x));

			float nextRadius = segments[i + 1]->segmentRadius;

			glm::vec2 rightC = nextPos + nextRight * nextRadius;
			glm::vec2 leftC = nextPos + nextLeft * nextRadius;

			// Triangle 4
			spineVertices.push_back({ glm::vec3(rightC, 0.f) , glm::vec4(0, 255, 255, 255) });
			spineVertices.push_back({ glm::vec3(rightB, 0.f) , glm::vec4(0, 255, 255, 255) });
			spineVertices.push_back({ glm::vec3(nextPos, 0.f) , glm::vec4(0, 255, 255, 255) });

			// Triangle 5
			spineVertices.push_back({ glm::vec3(leftC, 0.f) , glm::vec4(0, 255, 255, 255) });
			spineVertices.push_back({ glm::vec3(leftB, 0.f) , glm::vec4(0, 255, 255, 255) });
			spineVertices.push_back({ glm::vec3(nextPos, 0.f) , glm::vec4(0, 255, 255, 255) });
		}

		

		// Outline 
		Line::getInstance()->Render(glm::vec3(leftA, 0.f), glm::vec3(leftB, 0.f), glm::vec4(255, 255, 255, 255));
		Line::getInstance()->Render(glm::vec3(rightA, 0.f), glm::vec3(rightB, 0.f), glm::vec4(255, 255, 255, 255));
	}

	// Draw Triangles
	glBindBuffer(GL_ARRAY_BUFFER, spineVBO);
	glBufferData(GL_ARRAY_BUFFER, spineVertices.size() * sizeof(Vertex), spineVertices.data(), GL_DYNAMIC_DRAW);

	glBindVertexArray(spineVAO);

	glDrawArrays(GL_TRIANGLES, 0, spineVertices.size() - (spineVertices.size() % 3));

	spineVertices.clear();
	glBindVertexArray(0);

	// HEAD (start cap)
	{
		auto& seg = segments[0];
		glm::vec2 startDir = glm::normalize(seg->direction);
		glm::vec2 startPos = seg->transform.position;

		glm::vec2 left = glm::normalize(glm::vec2(startDir.y, -startDir.x));
		glm::vec2 right = glm::normalize(glm::vec2(-startDir.y, startDir.x));

		glm::vec2 leftA = startPos + left * glm::vec2(seg->segmentRadius);
		glm::vec2 rightA = startPos + right * glm::vec2(seg->segmentRadius);

		Circle::getInstance()->DrawSemiCircleFilled(
			leftA.x, leftA.y, rightA.x, rightA.y,
			seg->segmentRadius, M_PI / 2,
			glm::vec4(255, 255, 255, 255), activeCamera
		);
	}

	// TAIL (end cap)
	{
		auto& seg = segments[creatureLenght - 1];
		glm::vec2 endDir = glm::normalize(seg->direction);
		glm::vec2 endPos = seg->transform.position;

		glm::vec2 left = glm::normalize(glm::vec2(endDir.y, -endDir.x));
		glm::vec2 right = glm::normalize(glm::vec2(-endDir.y, endDir.x));

		glm::vec2 leftB = endPos + left * glm::vec2(seg->segmentRadius);
		glm::vec2 rightB = endPos + right * glm::vec2(seg->segmentRadius);

		Circle::getInstance()->DrawSemiCircleFilled(
			leftB.x, leftB.y, rightB.x, rightB.y,
			seg->segmentRadius, -M_PI / 2,
			glm::vec4(255, 255, 255, 255), activeCamera
		);
	}
}

void Player::AddPoints(int valueArg)
{
	points += valueArg;
}

void Player::LevelUp()
{

}

void Player::IncreaseBodyLenght()
{
	creatureLenght += 1;
	segments.push_back(new Segment(
		segments[creatureLenght - 1],
		segments[creatureLenght - 1]->segmentShape,
		segments[creatureLenght - 1]->segmentRadius,
		10));
}

void Player::DecreaseBodyLenght()
{
	creatureLenght -= 1;
	segments.pop_back();
}

void Player::IncreaseSpeed(float valueArg)
{
	movementSpeed += valueArg;
}

void Player::DecreaseSpeed(float valueArg)
{
	movementSpeed -= valueArg;
}

void Player::IncreaseWeaponSlots()
{
	weaponSlots += 1;

	for (int i = weaponsCreated; i <= creatureLenght; i += 2)
	{
		if (weaponsCreated < weaponSlots)
		{
			weapons.push_back(new BaseWeapon(&segments[i]->transform, activeCamera));
			weaponsCreated += 1;
		}
	}
}

void Player::DecreaseWeaponSlots()
{
	weaponSlots -= 1;
	weaponsCreated -= 1;
	weapons.pop_back();
}

void Player::IncreaseHealth(int valueArg)
{
	hitpoints += valueArg;
}

void Player::DecreaseHealth(int valueArg)
{
	hitpoints -= valueArg;
}

void Player::IncreaseSizeUniformly()
{
	for (auto& segment : segments)
		segment->segmentRadius += 1;
}

void Player::DecreaseSizeUniformly()
{
	for (auto& segment : segments)
		segment->segmentRadius -= 1;
}

void Player::IncreaseSizeSequentially()
{
	unsigned i = 0;
	for (auto& segment : segments)
	{
		++i;
		if (segment->segmentRadius == maxRadius) { segment->segmentRadius += 1; break; }
		else if (i == creatureLenght) { segment->segmentRadius += 1; maxRadius++; break; }
	}
}

void Player::DecreaseSizeSequentially()
{
	unsigned i = 0;
	for (auto& segment : segments)
	{
		++i;
		if (segment->segmentRadius == maxRadius) { segment->segmentRadius -= 1; break; }
		else if (i == creatureLenght) { segment->segmentRadius -= 1; maxRadius--; break; }
	}
}

void Player::AddLeg()
{
	if (legs.size() + 1 <= creatureLenght)
	{
		legs.push_back(new IKChain(glm::vec2(0, 0)));
		footTargets.push_back(glm::vec2(0, 0)); // Init empty target
	}
	else
		Logger::Error("No more room for legs.");
}

void Player::RemoveLeg()
{
	if (legs.size() - 1 > 0)
	{
		legs.pop_back();
		footTargets.pop_back();
	}
	else
		Logger::Error("Trying to remove inexistent leg.");
}

void Player::AddSpacing()
{
	segmentSpacing += 1;

	for (int i = 0; i <= creatureLenght; i++)
	{
		segments[i]->segmentSpacing = segmentSpacing;
	}
}

void Player::RemoveSpacing()
{
	segmentSpacing -= 1;

	for (int i = 0; i <= creatureLenght; i++)
	{
		segments[i]->segmentSpacing = segmentSpacing;
	}
}