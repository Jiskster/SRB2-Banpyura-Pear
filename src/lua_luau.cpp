// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 2026 by Sonic Team Junior.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif
#include <lua.h>
#include <lualib.h>
#include <luacode.h>
#ifdef __cplusplus
}
#endif

#include <cstdlib>

extern "C" char *LUA_CompileBuffer(const char *buffer, size_t size, size_t *bytecode_size)
{
	lua_CompileOptions options = {};
	options.optimizationLevel = 0;
	options.debugLevel = 1;

	return luau_compile(buffer, size, &options, bytecode_size);
}

extern "C" int LUA_LoadBuffer(lua_State *L, const char *buffer, size_t size, const char *name)
{
	size_t bytecode_size = 0;
	char *bytecode = LUA_CompileBuffer(buffer, size, &bytecode_size);
	int status;

	if (!bytecode)
	{
		lua_pushliteral(L, "Luau compiler failed to allocate bytecode");
		return LUA_ERRMEM;
	}

	status = luau_load(L, name, bytecode, bytecode_size, 0);
	std::free(bytecode);
	return status;
}
