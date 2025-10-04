#pragma once
#include "precomp.h"

struct ScriptComponent
{
	sol::state state;
	std::string scriptPath = "Assets/Scripts/Default.lua";

	sol::function onStart;
	sol::function onUpdate;
	sol::function onShutdown;

	bool initialized = false;
};