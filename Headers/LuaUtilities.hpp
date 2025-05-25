extern "C"
{
#include "lua/include/lua.h"
#include "lua/include/lauxlib.h"
#include "lua/include/lualib.h"
}
#include "Logger.hpp"

static inline bool CheckLua(lua_State* L, int r)
{
	if (r != LUA_OK)
	{
		std::string errormsg = lua_tostring(L, -1);
		Logger::Error(errormsg);
		return false;
	}

	return true;
}

static inline int lua_GetRandomNumber(lua_State* L)
{
	float a = Random::RandomFloat(); // Generate random number
	lua_pushnumber(L, a); // Push it to Lua

	return 1;
}