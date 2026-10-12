#ifndef SRB2_LUAU_BUILD_CONFIG_H
#define SRB2_LUAU_BUILD_CONFIG_H

#ifdef __cplusplus
#define SRB2_LUA_API extern "C"
#define SRB2_LUACODE_API extern "C"
#else
#define SRB2_LUA_API extern
#define SRB2_LUACODE_API extern
#endif

#endif
