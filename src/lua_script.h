// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 2012-2016 by John "JTE" Muniz.
// Copyright (C) 2012-2024 by Sonic Team Junior.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  lua_script.h
/// \brief Lua scripting basics

#ifndef LUA_SCRIPT_H
#define LUA_SCRIPT_H

#include "p_saveg.h"
#include "m_fixed.h"
#include "doomtype.h"
#include "d_player.h"
#include "g_state.h"
#include "taglist.h"

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <lua.h>
#include <lualib.h>
int LUA_LoadBuffer(lua_State *L, const char *buffer, size_t size, const char *name);
char *LUA_CompileBuffer(const char *buffer, size_t size, size_t *bytecode_size);
#ifdef __cplusplus
}
#endif

#undef lua_pushcfunction
#define lua_pushcfunction(L, f) lua_pushcclosurek((L), (f), #f, 0, NULL)
#undef lua_pushcclosure
#define lua_pushcclosure(L, f, n) lua_pushcclosurek((L), (f), #f, (n), NULL)
#define luaL_loadbuffer(L, b, s, n) LUA_LoadBuffer((L), (const char *)(b), (s), (n))
#define luaL_loadstring(L, s) LUA_LoadBuffer((L), (s), strlen(s), (s))
#define luaL_dostring(L, s) (luaL_loadstring((L), (s)) || lua_pcall((L), 0, LUA_MULTRET, 0))
#define LUA_QL(x) "'" x "'"
#define LUA_QS "%s"

#define luaL_typerror(L, n, t) (luaL_typeerror((L), (n), (t)), 0)
#define luaL_getn(L, i) lua_objlen((L), (i))
#undef luaL_error
#define luaL_error(L, ...) (luaL_errorL((L), __VA_ARGS__), 0)
#undef luaL_argerror
#define luaL_argerror(L, n, message) (luaL_argerrorL((L), (n), (message)), 0)
#define lua_register(L, n, f) (lua_pushcfunction((L), (f)), lua_setglobal((L), (n)))

static inline int LUA_CompatRef(lua_State *L, int table)
{
	int reference;

	if (lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		return LUA_REFNIL;
	}

	if (table == LUA_REGISTRYINDEX)
		reference = lua_ref(L, -1);
	else
	{
		lua_rawgeti(L, table, 0);
		reference = lua_tointeger(L, -1);
		lua_pop(L, 1);

		if (reference)
		{
			lua_rawgeti(L, table, reference);
			lua_rawseti(L, table, 0);
		}
		else
			reference = lua_objlen(L, table) + 1;

		lua_rawseti(L, table, reference);
	}

	lua_pop(L, 1);
	return reference;
}

static inline void LUA_CompatUnref(lua_State *L, int table, int reference)
{
	if (reference <= LUA_REFNIL)
		return;

	if (table == LUA_REGISTRYINDEX)
		lua_unref(L, reference);
	else
	{
		lua_rawgeti(L, table, 0);
		lua_rawseti(L, table, reference);
		lua_pushinteger(L, reference);
		lua_rawseti(L, table, 0);
	}
}

#define luaL_ref(L, table) LUA_CompatRef((L), (table))
#define luaL_unref(L, table, reference) LUA_CompatUnref((L), (table), (reference))

#define lua_optboolean(L, i) (!lua_isnoneornil(L, i) && lua_toboolean(L, i))
#define lua_opttrueboolean(L, i) (lua_isnoneornil(L, i) || lua_toboolean(L, i))

// fixed_t casting
// TODO add some distinction between fixed numbers and integer numbers
// for at least the purpose of printing and maybe math.
#define luaL_checkfixed(L, i) luaL_checkinteger(L, i)
#define lua_pushfixed(L, f) lua_pushinteger(L, f)

// angle_t casting
// TODO deal with signedness
#define luaL_checkangle(L, i) ((angle_t)luaL_checkinteger(L, i))
#define lua_pushangle(L, a) lua_pushinteger(L, a)

#ifdef _DEBUG
void LUA_ClearExtVars(void);
#endif

extern INT32 lua_lumploading; // is LUA_LoadLump being called?
extern INT32 lua_locallyloading; // is this wad file being loaded locally?

int LUA_GetErrorMessage(lua_State *L);
int LUA_Call(lua_State *L, int nargs, int nresults, int errorhandlerindex);
boolean LUA_LoadLump(UINT16 wad, UINT16 lump);
void LUA_DoLump(UINT16 wad, UINT16 lump, boolean noresults);
#ifdef LUA_ALLOW_BYTECODE
void LUA_DumpFile(const char *filename);
#endif
fixed_t LUA_EvalMath(const char *word);
void LUA_Step(void);
void LUA_Archive(save_t *save_p);
void LUA_UnArchive(save_t *save_p);
int LUA_PushGlobals(lua_State *L, const char *word);
int LUA_CheckGlobals(lua_State *L, const char *word);
void Got_Luacmd(UINT8 **cp, INT32 playernum); // lua_consolelib.c
void LUA_CVarChanged(void *cvar); // lua_consolelib.c
int Lua_optoption(lua_State *L, int narg, int def, int list_ref);
int Lua_CreateFieldTable(lua_State *L, const char *const lst[]);
void LUA_HookNetArchive(lua_CFunction archFunc);

void LUA_PushTaggableObjectArray
(		lua_State *L,
		const char *field,
		lua_CFunction iterator,
		lua_CFunction indexer,
		lua_CFunction counter,
		taggroup_t *garray[],
		size_t * max_elements,
		void * element_array,
		size_t sizeof_element,
		const char *meta);

void LUA_SetCFunctionField(lua_State *L, const char *name, lua_CFunction value);

void LUA_RegisterUserdataMetatable(
	lua_State *L,
	const char *name,
	lua_CFunction get,
	lua_CFunction set,
	lua_CFunction len
);

void LUA_CreateAndSetMetatable(
	lua_State *L,
	lua_CFunction get,
	lua_CFunction set,
	lua_CFunction len,
	boolean keep
);

void LUA_CreateAndSetUserdataField(
	lua_State *L,
	int index,
	const char *name,
	lua_CFunction get,
	lua_CFunction set,
	lua_CFunction len,
	boolean keep
);

void LUA_RegisterGlobalUserdata(
	lua_State *L,
	const char *name,
	lua_CFunction get,
	lua_CFunction set,
	lua_CFunction len
);

void LUA_InsertTaggroupIterator
(		lua_State *L,
		taggroup_t *garray[],
		size_t * max_elements,
		void * element_array,
		size_t sizeof_element,
		const char * meta);

typedef enum {
	LPUSHED_NIL,
	LPUSHED_NEW,
	LPUSHED_EXISTING,
} lpushed_t;

void LUA_PushUserdata(lua_State *L, void *data, const char *meta);
lpushed_t LUA_RawPushUserdata(lua_State *L, void *data);

void LUA_InvalidateUserdata(void *data);

void LUA_InvalidateLevel(void);
void LUA_InvalidateMapthings(void);
void LUA_InvalidatePlayer(player_t *player);

// Console wrapper
void COM_Lua_f(void);

#define LUA_ErrInvalid(L, type) luaL_error(L, "accessed " type " doesn't exist anymore, please check 'valid' before using " type ".");

#define LUA_ErrSetDirectly(L, type, field) luaL_error(L, type " field " LUA_QL(field) " cannot be set directly.")

// Deprecation warnings
// Shows once upon use. Then doesn't show again.
#define LUA_Deprecated(L,this_func,use_instead)\
{\
	static UINT8 seen = 0;\
	if (!seen) {\
		seen = 1;\
		CONS_Alert(CONS_WARNING,"\"%s\" is deprecated and will be removed.\nUse \"%s\" instead.\n", this_func, use_instead);\
	}\
}

// Warnings about incorrect function usage.
// Shows once, then never again, like deprecation
#define LUA_UsageWarning(L, warningmsg)\
{\
	static UINT8 seen = 0;\
	if (!seen) {\
		seen = 1;\
		CONS_Alert(CONS_WARNING,"%s\n", warningmsg);\
	}\
}

// uncomment if you want seg_t/node_t in Lua
// #define HAVE_LUA_SEGS

#define ISINLEVEL \
	(gamestate == GS_LEVEL || titlemapinaction)

#define INLEVEL if (! ISINLEVEL)\
return luaL_error(L, "This can only be used in a level!");

int LUA_HTTPLib(lua_State *L);
#endif/*LUA_SCRIPT_H*/
