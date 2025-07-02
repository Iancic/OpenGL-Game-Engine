#include "Creature.hpp"

Creature::Creature(int difficulty, Camera* camera)
{
	desiredSegmentSpacing = segmentSpacing;

	srand(static_cast<unsigned int>(time(NULL)));

	glGenVertexArrays(1, &spineVAO);
	glGenBuffers(1, &spineVBO);

	// Bind the VAO
	glBindVertexArray(spineVAO);

	// Bind the VBO and upload the vertex data
	glBindBuffer(GL_ARRAY_BUFFER, spineVBO);
	glBufferData(GL_ARRAY_BUFFER, spineVertices.size() * sizeof(Vertex), spineVertices.data(), GL_STATIC_DRAW);

	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	// Color
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(1);

	// TexCoord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(2);

	// Unbind VAO (optional but good practice)
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

	int legCount = static_cast<int>(segments.size());
	isStepping.resize(legCount, false);
	stepTimers.resize(legCount, 0.0f);
	stepStartPositions.resize(legCount);
	stepTargetPositions.resize(legCount);

	spriteRenderer = new SpriteRenderer();

	// Setup Colliders
	for (auto& segment : segments) colliders.push_back(segment->collider);
}

Creature::~Creature()
{
	for (auto segment : segments) delete segment;
	for (auto weapon : weapons) delete weapon;
	delete spriteRenderer;
}

void Creature::Shoot()
{
	for (auto& element : weapons)
	{
		element->Shoot();
	}
}

void Creature::Update(Transform& target, float deltaTime)
{
	deltaTimeRef = deltaTime;
	pulseTime += deltaTime; // This is required to make animated noise.

	if (points >= maxPoints) LevelUp();

	// Update each segment
	// Step 1: Update segment positions
	for (auto& segment : segments)
	{
		segment->Update();
	}

	// Step 2: Apply constraints in order (root to tip)
	for (size_t i = 1; i < segments.size(); ++i)
	{
		segments[i]->Constraint();
	}

	// Optional Step 3: Extra pass from tip to root for stability (e.g., for angle constraints)
	for (size_t i = segments.size() - 1; i > 0; --i)
	{
		segments[i]->Constraint();
	}

	// Update each weapon
	for (auto& element : weapons)
	{
		element->Update(target, deltaTime);
	}
	
	// Direction vectors
	for (int i = 0; i < creatureLenght; i++)
	{
		segments[i]->direction = {
			segments[i + 1]->transform.position.x - segments[i]->transform.position.x,
			segments[i + 1]->transform.position.y - segments[i]->transform.position.y };
	}

	if (!gait)
	{
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
	else // TODO: This gait snippet is generate with AI so refactor
	{
		float stepDuration = 0.2f;

		for (int i = 0; i < legs.size(); i++) {
			Segment* seg = segments[i];
			glm::vec2 bodyPos = seg->transform.position;
			glm::vec2 forward = glm::normalize(seg->direction);
			glm::vec2 right = glm::normalize(glm::vec2(-forward.y, forward.x));
			float sideSign = (i % 2 == 0) ? 1.0f : -1.0f;

			float sideOffset = seg->segmentRadius * sideSign;
			glm::vec2 base = bodyPos + right * sideOffset;

			glm::vec2 diagonalDir = glm::normalize(forward + right * sideSign * outwardBias);
			glm::vec2 idealFootTarget = base + diagonalDir * forwardOffset;

			// Trigger a step if far from target and not already stepping
			if (!isStepping[i] && glm::distance(footTargets[i], idealFootTarget) > distanceToSnap) {
				isStepping[i] = true;
				stepTimers[i] = 0.0f;
				stepStartPositions[i] = footTargets[i];
				stepTargetPositions[i] = idealFootTarget;
			}

			// Handle ongoing step
			if (isStepping[i]) {
				stepTimers[i] += deltaTime;
				float t = glm::clamp(stepTimers[i] / stepDuration, 0.0f, 1.0f);
				float smoothT = t * t * (3 - 2 * t); // smoothstep
				glm::vec2 interpolatedPos = glm::mix(stepStartPositions[i], stepTargetPositions[i], smoothT);
				footTargets[i] = interpolatedPos;

				if (t >= 1.0f) {
					isStepping[i] = false;
				}
			}

			// Solve IK
			legs[i]->SolveIK(footTargets[i].x, footTargets[i].y, base.x, base.y);
		}
	}

	int middleSegment = creatureLenght / 2;
	if (creatureLenght % 2 == 1) middleSegment++;

	Transform targetBroadPhase = segments[middleSegment]->transform;

	float broadPhaseSize{};
	for (auto& segment : segments) { broadPhaseSize += segment->segmentRadius + segment->segmentSpacing; }
}

void Creature::AI()
{
	float center = 0.f;
	float r = 55.f;
	segments[0]->transform.position.x = center + r * cos(theta);
	segments[0]->transform.position.y = center + r * sin(theta);
	theta += circularSpeed * deltaTimeRef;
}

void Creature::Render()
{
	if (hasGeometry) for (auto& element : segments) element->Render(activeCamera); // Render Individual Segment Geometry

	for (int i = 0; i < legs.size(); i++) legs[i]->Render(activeCamera, legWidth);  // Render Legs

	RenderInside();
	RenderSprites();

	if (hasWeapons) for (auto& element : weapons) element->Render(); // Render Weapons
}

void Creature::RenderDebug()
{
	for (auto& element : segments) element->RenderDebug();
}

void Creature::RenderSprites()
{
	ResourceManager::getInstance()->spriteShader->use();

	// Model matrix
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(segments[0]->transform.position));
	model = glm::scale(model, glm::vec3(1.0f));
	ResourceManager::getInstance()->spriteShader->setMat4("model", model);
	ResourceManager::getInstance()->spriteShader->setMat4("projection", activeCamera->GetProjectionMatrix()); // set this in constructor or per-frame

	// Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ResourceManager::getInstance()->smileyFace->ID);
	ResourceManager::getInstance()->spriteShader->setInt("image", 0);

	// Draw
	glBindVertexArray(spriteRenderer->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	spriteRenderer->DrawSprite(*ResourceManager::getInstance()->smileyFace, glm::vec2(0.0f, 0.0f), glm::vec2(100.0f, 100.0f), 45.f, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Creature::RenderInside()
{
	glm::vec4 color = glm::vec4(1, 1, 1, 1);

	// HEAD (start cap)
	{
		auto& seg = segments[0];
		glm::vec2 startDir = glm::normalize(seg->direction);
		glm::vec2 startPos = seg->transform.position;

		glm::vec2 left = glm::normalize(glm::vec2(startDir.y, -startDir.x));
		glm::vec2 right = glm::normalize(glm::vec2(-startDir.y, startDir.x));

		glm::vec2 leftA = startPos + left * glm::vec2(seg->segmentRadius);
		glm::vec2 rightA = startPos + right * glm::vec2(seg->segmentRadius);

		// Draw semicircle outline for the head (start cap)
		Circle::getInstance()->SendSemiCircleTriangles(leftA.x, leftA.y, rightA.x, rightA.y, seg->segmentRadius, M_PI / 2, color, spineVertices);
	}

	for (int i = 0; i < creatureLenght - 1; i++)
	{
		glm::vec2 startPos = segments[i]->transform.position;
		glm::vec2 endPos = segments[i + 1]->transform.position;

		glm::vec2 startDir = segments[i]->direction;
		glm::vec2 endDir = segments[i + 1]->direction;

		glm::vec2 leftPointStart = glm::normalize(glm::vec2(startDir.y, -startDir.x));
		glm::vec2 rightPointStart = glm::normalize(glm::vec2(-startDir.y, startDir.x));
		glm::vec2 leftPointEnd = glm::normalize(glm::vec2(endDir.y, -endDir.x));
		glm::vec2 rightPointEnd = glm::normalize(glm::vec2(-endDir.y, endDir.x));

		glm::vec2 leftA = startPos + leftPointStart * glm::vec2(segments[i]->segmentRadius);
		glm::vec2 rightA = startPos + rightPointStart * glm::vec2(segments[i]->segmentRadius);
		glm::vec2 leftB = endPos + leftPointEnd * glm::vec2(segments[i + 1]->segmentRadius);
		glm::vec2 rightB = endPos + rightPointEnd * glm::vec2(segments[i + 1]->segmentRadius);

		auto shader = ResourceManager::getInstance()->perlinShader;
		shader->use();

		// Set proper uniforms for perlin skin.
		shader->setFloat("time", pulseTime);

		shader->setVec2("offset", offset);

		shader->setVec3("primaryColor", primaryColor);
		shader->setVec3("secondaryColor", secondaryColor);

		shader->setFloat("zoomFactor", zoomFactor);
		shader->setFloat("motionSpeed", dynamicSpeed);
		shader->setFloat("rotationSpeed", rotationSpeed);

		shader->setFloat("spotSize", spotSize);

		glm::mat4 globalTransform = glm::mat4(1.f);
		glm::mat4 MVP = activeCamera->projectionMatrix * activeCamera->view * globalTransform;
		shader->setMat4("MVP", MVP);

		float textureScale = 1.f;
		float t = i / float(creatureLenght - 1);
		float vCoord = t * textureScale;
		float vCoordNext = (i + 1) / float(creatureLenght - 1) * textureScale;

		// Triangle 1
		spineVertices.push_back({ glm::vec3(leftA, 0.f), color, glm::vec2(0.f, vCoord) });
		spineVertices.push_back({ glm::vec3(leftB, 0.f), color, glm::vec2(0.f, vCoordNext) });
		spineVertices.push_back({ glm::vec3(startPos, 0.f), color, glm::vec2(0.5f, vCoord) });

		// Triangle 2
		spineVertices.push_back({ glm::vec3(rightA, 0.f), color, glm::vec2(1.f, vCoord) });
		spineVertices.push_back({ glm::vec3(rightB, 0.f), color, glm::vec2(1.f, vCoordNext) });
		spineVertices.push_back({ glm::vec3(startPos, 0.f), color, glm::vec2(0.5f, vCoord) });

		// Triangle 3
		spineVertices.push_back({ glm::vec3(rightB, 0.f), color, glm::vec2(1.f, vCoordNext) });
		spineVertices.push_back({ glm::vec3(leftB, 0.f), color, glm::vec2(0.f, vCoordNext) });
		spineVertices.push_back({ glm::vec3(startPos, 0.f), color, glm::vec2(0.5f, vCoord) });

		if (i + 1 < creatureLenght)
		{
			glm::vec2 nextDir = segments[i + 1]->direction;
			glm::vec2 nextPos = segments[i + 1]->transform.position;
			float nextRadius = segments[i + 1]->segmentRadius;

			glm::vec2 nextRight = glm::normalize(glm::vec2(-nextDir.y, nextDir.x));
			glm::vec2 nextLeft = glm::normalize(glm::vec2(nextDir.y, -nextDir.x));

			glm::vec2 rightC = nextPos + nextRight * nextRadius;
			glm::vec2 leftC = nextPos + nextLeft * nextRadius;

			float vCoordNextNext = (i + 2) / float(creatureLenght - 1) * textureScale;

			// Triangle 4
			spineVertices.push_back({ glm::vec3(rightC, 0.f), color, glm::vec2(1.f, vCoordNextNext) });
			spineVertices.push_back({ glm::vec3(rightB, 0.f), color, glm::vec2(1.f, vCoordNext) });
			spineVertices.push_back({ glm::vec3(nextPos, 0.f), color, glm::vec2(0.5f, vCoordNext) });

			// Triangle 5
			spineVertices.push_back({ glm::vec3(leftC, 0.f), color, glm::vec2(0.f, vCoordNextNext) });
			spineVertices.push_back({ glm::vec3(leftB, 0.f), color, glm::vec2(0.f, vCoordNext) });
			spineVertices.push_back({ glm::vec3(nextPos, 0.f), color, glm::vec2(0.5f, vCoordNext) });
		}
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

		// Draw semicircle outline for the tail (end cap)
		Circle::getInstance()->SendSemiCircleTriangles(leftB.x, leftB.y, rightB.x, rightB.y, seg->segmentRadius, -M_PI / 2, color, spineVertices);
	}

	// Upload & draw spine
	glBindVertexArray(spineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, spineVBO);
	glBufferData(GL_ARRAY_BUFFER, spineVertices.size() * sizeof(Vertex), spineVertices.data(), GL_DYNAMIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, spineVertices.size());
	glBindVertexArray(0);
	spineVertices.clear();

}

void Creature::AddPoints(int valueArg)
{
	points += valueArg;
}

void Creature::LevelUp()
{

}

void Creature::IncreaseBodyLenght()
{
	creatureLenght += 1;
	segments.push_back(new Segment(
		segments[creatureLenght - 1],
		segments[creatureLenght - 1]->segmentShape,
		segments[creatureLenght - 1]->segmentRadius,
		10));
}

void Creature::DecreaseBodyLenght()
{
	creatureLenght -= 1;
	segments.pop_back();
}

void Creature::IncreaseSpeed(float valueArg)
{
	movementSpeed += valueArg;
}

void Creature::DecreaseSpeed(float valueArg)
{
	movementSpeed -= valueArg;
}

void Creature::IncreaseWeaponSlots()
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

void Creature::DecreaseWeaponSlots()
{
	weaponSlots -= 1;
	weaponsCreated -= 1;
	weapons.pop_back();
}

void Creature::IncreaseHealth(int valueArg)
{
	hitpoints += valueArg;
}

void Creature::DecreaseHealth(int valueArg)
{
	hitpoints -= valueArg;
}

void Creature::IncreaseSizeUniformly()
{
	for (auto& segment : segments)
		segment->segmentRadius += 1;
}

void Creature::DecreaseSizeUniformly()
{
	for (auto& segment : segments)
		segment->segmentRadius -= 1;
}

void Creature::IncreaseSizeSequentially()
{
	unsigned i = 0;
	for (auto& segment : segments)
	{
		++i;
		if (segment->segmentRadius == maxRadius) { segment->segmentRadius += 1; break; }
		else if (i == creatureLenght) { segment->segmentRadius += 1; maxRadius++; break; }
	}
}

void Creature::DecreaseSizeSequentially()
{
	unsigned i = 0;
	for (auto& segment : segments)
	{
		++i;
		if (segment->segmentRadius == maxRadius) { segment->segmentRadius -= 1; break; }
		else if (i == creatureLenght) { segment->segmentRadius -= 1; maxRadius--; break; }
	}
}

void Creature::AddLeg()
{
	if (legs.size() + 1 <= creatureLenght)
	{
		legs.push_back(new IKChain(glm::vec2(0, 0)));
		footTargets.push_back(glm::vec2(0, 0)); // Init empty target
	}
	else
		Logger::Error("No more room for legs.");
}

void Creature::RemoveLeg()
{
	if (legs.size() - 1 >= 0)
	{
		legs.pop_back();
		footTargets.pop_back();
	}
	else
		Logger::Error("Trying to remove inexistent leg.");
}

void Creature::AddSpacing()
{
	segmentSpacing += 1;

	for (int i = 0; i <= creatureLenght; i++)
	{
		segments[i]->segmentSpacing = segmentSpacing;
	}
}

void Creature::RemoveSpacing()
{
	segmentSpacing -= 1;

	for (int i = 0; i <= creatureLenght; i++)
	{
		segments[i]->segmentSpacing = segmentSpacing;
	}
}

void Creature::MoveWest()
{
	segments[0]->transform.position.x -= movementSpeed * deltaTimeRef;
}

void Creature::MoveEast()
{
	segments[0]->transform.position.x += movementSpeed * deltaTimeRef;
}

void Creature::MoveSouth()
{
	segments[0]->transform.position.y -= movementSpeed * deltaTimeRef;
}

void Creature::MoveNorth()
{
	segments[0]->transform.position.y += movementSpeed * deltaTimeRef;
}