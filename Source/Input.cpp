#include "Input.hpp"

Input::Input()
{
	controller = SDL_GameControllerOpen(0);
}

void Input::PollEvents(SDL_Window* window, bool inDragRegion)
{
	// Controller Pressed
	controllerAxisLeftX = NormalizeAxis(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX));
	controllerAxisLeftY = NormalizeAxis(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY));
	controllerAxisRightX = NormalizeAxis(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX));
	controllerAxisRightY = NormalizeAxis(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY));

	controllerMoved = fabs(controllerAxisLeftX) > 0.1f || fabs(controllerAxisLeftY) > 0.1f;

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		switch (event.type)
		{
		case SDL_QUIT:
			quitSDL = true;
			break;

		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				int newWidth = event.window.data1;
				int newHeight = event.window.data2;

				glViewport(0, 0, newWidth, newHeight);
				resizedWindow = true;
			}
			break;

		case SDL_KEYDOWN:
			if (!event.key.repeat)
			{
				keyPressed[event.key.keysym.scancode] = true;
				keyHeld[event.key.keysym.scancode] = true;
			}
			break;

		case SDL_KEYUP:
			keyReleased[event.key.keysym.scancode] = false;
			keyHeld[event.key.keysym.scancode] = false;
			break;

		case SDL_CONTROLLERBUTTONDOWN:
			break;

		case SDL_CONTROLLERBUTTONUP:
			break;

		// ----------------------------- Code for moving the window around ---------------------------------------
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT && inDragRegion) 
			{
				dragging = true;

				int x, y;
				SDL_GetGlobalMouseState(&x, &y);

				int win_x, win_y;
				SDL_GetWindowPosition(window, &win_x, &win_y);

				drag_offset_x = x - win_x;
				drag_offset_y = y - win_y;
			}
        break;

        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT) 
			{
                dragging = false;
            }
            break;

        case SDL_MOUSEMOTION:
            if (dragging && inDragRegion) 
			{
                int x, y;
                SDL_GetGlobalMouseState(&x, &y);
                SDL_SetWindowPosition(window, x - drag_offset_x, y - drag_offset_y);
            }
            break;
		// -------------------------------------------------------------------------------------------------------

		default:
			break;
		}
	}
}

void Input::EndFrame()
{
	keyPressed.clear();
	keyReleased.clear();
	quitSDL = false;
	resizedWindow = false;
}

glm::vec2 Input::GetMousePosition()
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	return glm::vec2(x, y);
}

bool Input::GetEvent(int key)
{
	switch (key)
	{
	case SDL_QUIT:
		return quitSDL;
		break;
	case SDL_WINDOWEVENT_RESIZED:
		return resizedWindow;
		break;
	}
}

bool Input::GetKey(int key, int action)
{
	switch (action)
	{
		case Action::DOWN:
			return keyPressed[key];
			break;
		case Action::UP:
			return keyReleased[key];
			break;
		case Action::HOLD:
			return keyHeld[key];
			break;
	}
}

bool Input::GetButton(int key, int action)
{
	switch (action)
	{
	case Action::DOWN:
		return keyPressed[key];
		break;
	case Action::UP:
		return keyReleased[key];
		break;
	case Action::HOLD:
		return keyHeld[key];
		break;
	}
}

void Input::StartRumble()
{
	if (SDL_GameControllerRumble(controller, 30000, 60000, 500) != 0)
	{
		SDL_Log("Failed to start rumble: %s", SDL_GetError());
	}
}