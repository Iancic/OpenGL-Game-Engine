#include "precomp.h"
#include "Input.hpp"

Input::Input()
{
	int count = 0;
	SDL_JoystickID* ids = SDL_GetGamepads(&count);
	if (ids && count > 0)
		gamepad = SDL_OpenGamepad(ids[0]);
	SDL_free(ids);
}

void Input::PollEvents(SDL_Window* window, bool inDragRegion)
{
    // ---- Update gamepad axes (if present) ----
    if (gamepad && SDL_GamepadConnected(gamepad))
    {
        controllerAxisLeftX = NormalizeAxis(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX));
        controllerAxisLeftY = NormalizeAxis(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY));
        controllerAxisRightX = NormalizeAxis(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTX));
        controllerAxisRightY = NormalizeAxis(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTY));

        controllerMoved = (std::fabs(controllerAxisLeftX) > 0.1f) || (std::fabs(controllerAxisLeftY) > 0.1f);
    }
    else
    {
        controllerAxisLeftX = controllerAxisLeftY = controllerAxisRightX = controllerAxisRightY = 0.0f;
        controllerMoved = false;
    }

    // ---- Poll events ----
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        // Feed ImGui (SDL3 backend)
        ImGui_ImplSDL3_ProcessEvent(&e);

        switch (e.type)
        {
            // Quit / close
        case SDL_EVENT_QUIT:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            quitSDL = true;
            break;

            // Window resized
        case SDL_EVENT_WINDOW_RESIZED:
        {
            int newW = 0, newH = 0;
            SDL_GetWindowSizeInPixels(window, &newW, &newH); // pixel size is what GL cares about
            glViewport(0, 0, newW, newH);
            resizedWindow = true;
        } break;

        // Keyboard
        case SDL_EVENT_KEY_DOWN:
            if (!e.key.repeat)
            {
                // SDL3 event carries scancode directly
                const int sc = static_cast<int>(e.key.scancode);
                keyPressed[sc] = true;
                keyHeld[sc] = true;
            }
            break;

        case SDL_EVENT_KEY_UP:
        {
            const int sc = static_cast<int>(e.key.scancode);
            keyReleased[sc] = true;     // (your SDL2 code had 'false' here by mistake)
            keyHeld[sc] = false;
        } break;

        // Gamepad buttons (if you need them)
        case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            if (e.gbutton.button < SDL_GAMEPAD_BUTTON_COUNT) {
                controllerButtonPressed[e.gbutton.button] = true;
                controllerButtonHeld[e.gbutton.button] = true;
            }
            break;
        case SDL_EVENT_GAMEPAD_BUTTON_UP:
            if (e.gbutton.button < SDL_GAMEPAD_BUTTON_COUNT) {
                controllerButtonReleased[e.gbutton.button] = true;
                controllerButtonHeld[e.gbutton.button] = false;
            }
            break;

        default:
            break;
        }
    }
}

glm::vec2 Input::GetMousePosition()
{
	float x, y;
	SDL_GetMouseState(&x, &y);
	return glm::vec2(x, y);
}

bool Input::GetEvent(int key)
{
	switch (key)
	{
	case SDL_EVENT_QUIT:
	case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
		return quitSDL;
	case SDL_EVENT_WINDOW_RESIZED:
		return resizedWindow;
	default:
		return false;
	}
}

void Input::EndFrame()
{
	keyPressed.clear();
	keyReleased.clear();
	quitSDL = false;
	resizedWindow = false;
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
	if (gamepad && SDL_GamepadConnected(gamepad))
	{
		// low_freq, high_freq, duration_ms
		if (!SDL_RumbleGamepad(gamepad, 30000, 60000, 500))
			SDL_Log("Failed to start rumble: %s", SDL_GetError());
	}
}

float Input::NormalizeAxis(Sint16 value, float deadZone)
{
    if (abs(value) < deadZone)
        return 0.0f;

    // Convert to [-1.0, 1.0] range
    float result = value / 32767.0f;

    // Optional: Scale to start at 0.0 right after dead zone
    float adjusted = (abs(result) - (deadZone / 32767.0f)) / (1.0f - (deadZone / 32767.0f));
    return (value > 0) ? adjusted : -adjusted;
};