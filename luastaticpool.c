#include <stdlib.h>
#include <stdio.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "staticpool.h"

static int pool_init(lua_State *L)
{
	int count = lua_tointeger(L, 1);
	if (!count)
	{
		luaL_error(L, "pool init size error");
	}
	int ret = StaticPoolInit((uint16_t)count);
	lua_pushinteger(L, ret);
	return 1;
}

static int pool_free(lua_State *L)
{
	int ret = StaticPoolFree();
	lua_pushinteger(L, ret);
	return 1;
}

static int pool_add(lua_State *L)
{
	const char* name = lua_tostring(L, 1);
	if (!name)
	{
		luaL_error(L, "container name error");
	}
	int count = lua_tointeger(L, 2);
	if (!count)
	{
		luaL_error(L, "container size error");
	}
	int ret = StaticPoolAdd(name, (uint16_t)count);
	lua_pushinteger(L, ret);
	return 1;
}

static int pool_del(lua_State *L)
{
	const char* name = lua_tostring(L, 1);
	if (!name)
	{
		luaL_error(L, "container name error");
	}
	int ret = StaitcPoolDel(name);
	lua_pushinteger(L, ret);
	return 1;
}

static int pool_count(lua_State *L)
{
	int ret = StaticPoolCount();
	lua_pushinteger(L, ret);
	return 1;
}

static int pool_set(lua_State *L)
{
	const char* name = lua_tostring(L, 1);
	if (!name)
	{
		luaL_error(L, "container name error");
	}
	const char* key = lua_tostring(L, 2);
	if (!key)
	{
		luaL_error(L, "container key error");
	}
	const char* value = lua_tostring(L, 3);
	if (!value)
	{
		luaL_error(L, "container value error");
	}
	int ret = StaticPoolSetVal(name, key, value);
	lua_pushinteger(L, ret);
	return 1;
}

static int pool_get(lua_State *L)
{
	const char* name = lua_tostring(L, 1);
	if (!name)
	{
		luaL_error(L, "container name error");
	}
	const char* key = lua_tostring(L, 2);
	if (!key)
	{
		luaL_error(L, "container key error");
	}
	const char* value = StaticPoolGetVal(name, key);
	lua_pushstring(L, value);
	return 1;
}

static int pool_clear(lua_State *L)
{
	const char* name = lua_tostring(L, 1);
	if (!name)
	{
		luaL_error(L, "container name error");
	}
	const char* key = lua_tostring(L, 2);
	if (!key)
	{
		luaL_error(L, "container key error");
	}
	int ret = StaticPoolClearVal(name, key);
	lua_pushinteger(L, ret);
	return 1;
}

static int pool_size(lua_State *L)
{
	const char* name = lua_tostring(L, 1);
	if (!name)
	{
		luaL_error(L, "container name error");
	}
	int ret = StaticPoolConSize(name);
	lua_pushinteger(L, ret);
	return 1;
}

static const struct luaL_Reg staticpool_funs[] = {
	{"init",pool_init},
	{"free",pool_free},
	{"add",pool_add},
	{"del",pool_del},
	{"count",pool_count},
	{"set",pool_set},
	{"get",pool_get},
	{"clear",pool_clear},
	{"size",pool_size},
	{NULL,NULL}
};

int luaopen_staticpool(lua_State *L)
{
	luaL_checkversion(L);
	luaL_newlib(L,staticpool_funs);
	return 1;
}