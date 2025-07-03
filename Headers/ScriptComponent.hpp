#pragma once
#include "LuaUtilities.hpp"

struct ScriptComponent
{
	sol::state state;
	std::string scriptPath = "Scripts/Default.lua";

	sol::function onStart;
	sol::function onUpdate;
	sol::function onShutdown;

	bool initialized = false;
};