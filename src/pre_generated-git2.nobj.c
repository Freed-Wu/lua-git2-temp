/***********************************************************************************************
************************************************************************************************
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!! Warning this file was generated from a set of *.nobj.lua definition files !!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
************************************************************************************************
***********************************************************************************************/

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

/* some Lua 5.0 compatibility support. */
#if !defined(lua_pushliteral)
#define lua_pushliteral(L, s) lua_pushstring(L, "" s, (sizeof(s)/sizeof(char))-1)
#endif

#if !defined(LUA_VERSION_NUM)
#define lua_pushinteger(L, n) lua_pushnumber(L, (lua_Number)n)
#define luaL_Reg luaL_reg
#endif

/*
** Adapted from Lua 5.2.0 luaL_setfuncs.
*/
static void nobj_setfuncs (lua_State *L, const luaL_Reg *l, int nup) {
  luaL_checkstack(L, nup, "too many upvalues");
  for (; l->name != NULL; l++) {  /* fill the table with given functions */
    int i;
    for (i = 0; i < nup; i++)  /* copy upvalues to the top */
      lua_pushvalue(L, -nup);
    lua_pushstring(L, l->name);
    lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
    lua_settable(L, -(nup + 3));
  }
  lua_pop(L, nup);  /* remove upvalues */
}

/* some Lua 5.1 compatibility support. */
#if !defined(LUA_VERSION_NUM) || (LUA_VERSION_NUM == 501)

#define lua_load_no_mode(L, reader, data, source) \
	lua_load(L, reader, data, source)

#define lua_rawlen(L, idx) lua_objlen(L, idx)

#endif

#if LUA_VERSION_NUM >= 502

#define lua_load_no_mode(L, reader, data, source) \
	lua_load(L, reader, data, source, NULL)

static int luaL_typerror (lua_State *L, int narg, const char *tname) {
  const char *msg = lua_pushfstring(L, "%s expected, got %s",
                                    tname, luaL_typename(L, narg));
  return luaL_argerror(L, narg, msg);
}

#endif

#define REG_PACKAGE_IS_CONSTRUCTOR 0
#define REG_MODULES_AS_GLOBALS 0
#define REG_OBJECTS_AS_GLOBALS 0
#define OBJ_DATA_HIDDEN_METATABLE 0
#define USE_FIELD_GET_SET_METHODS 0
#define LUAJIT_FFI 0


#include "git2.h"



#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#ifdef _MSC_VER
#define __WINDOWS__
#else
#if defined(_WIN32)
#define __WINDOWS__
#endif
#endif

#ifdef __WINDOWS__

/* for MinGW32 compiler need to include <stdint.h> */
#ifdef __GNUC__
#include <stdint.h>
#include <stdbool.h>
#else

/* define some standard types missing on Windows. */
#ifndef __INT32_MAX__
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
#endif
#ifndef __INT64_MAX__
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#endif
typedef int bool;
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

#endif

/* wrap strerror_s(). */
#ifdef __GNUC__
#ifndef strerror_r
#define strerror_r(errno, buf, buflen) do { \
	strncpy((buf), strerror(errno), (buflen)-1); \
	(buf)[(buflen)-1] = '\0'; \
} while(0)
#endif
#else
#ifndef strerror_r
#define strerror_r(errno, buf, buflen) strerror_s((buf), (buflen), (errno))
#endif
#endif

#define FUNC_UNUSED

#define LUA_NOBJ_API __declspec(dllexport)

#else

#define LUA_NOBJ_API LUALIB_API

#include <stdint.h>
#include <stdbool.h>

#define FUNC_UNUSED __attribute__((unused))

#endif

#if defined(__GNUC__) && (__GNUC__ >= 4)
#define assert_obj_type(type, obj) \
	assert(__builtin_types_compatible_p(typeof(obj), type *))
#else
#define assert_obj_type(type, obj)
#endif

void *nobj_realloc(void *ptr, size_t osize, size_t nsize);

void *nobj_realloc(void *ptr, size_t osize, size_t nsize) {
	(void)osize;
	if(0 == nsize) {
		free(ptr);
		return NULL;
	}
	return realloc(ptr, nsize);
}

#define obj_type_free(type, obj) do { \
	assert_obj_type(type, obj); \
	nobj_realloc((obj), sizeof(type), 0); \
} while(0)

#define obj_type_new(type, obj) do { \
	assert_obj_type(type, obj); \
	(obj) = nobj_realloc(NULL, 0, sizeof(type)); \
} while(0)

typedef struct obj_type obj_type;

typedef void (*base_caster_t)(void **obj);

typedef void (*dyn_caster_t)(void **obj, obj_type **type);

#define OBJ_TYPE_FLAG_WEAK_REF (1<<0)
#define OBJ_TYPE_SIMPLE (1<<1)
#define OBJ_TYPE_IMPORT (1<<2)
struct obj_type {
	dyn_caster_t    dcaster;  /**< caster to support casting to sub-objects. */
	int32_t         id;       /**< type's id. */
	uint32_t        flags;    /**< type's flags (weak refs) */
	const char      *name;    /**< type's object name. */
};

typedef struct obj_base {
	int32_t        id;
	base_caster_t  bcaster;
} obj_base;

typedef enum obj_const_type {
	CONST_UNKOWN    = 0,
	CONST_BOOLEAN   = 1,
	CONST_NUMBER    = 2,
	CONST_STRING    = 3
} obj_const_type;

typedef struct obj_const {
	const char      *name;    /**< constant's name. */
	const char      *str;
	double          num;
	obj_const_type  type;
} obj_const;

typedef enum obj_field_type {
	TYPE_UNKOWN    = 0,
	TYPE_UINT8     = 1,
	TYPE_UINT16    = 2,
	TYPE_UINT32    = 3,
	TYPE_UINT64    = 4,
	TYPE_INT8      = 5,
	TYPE_INT16     = 6,
	TYPE_INT32     = 7,
	TYPE_INT64     = 8,
	TYPE_DOUBLE    = 9,
	TYPE_FLOAT     = 10,
	TYPE_STRING    = 11
} obj_field_type;

typedef struct obj_field {
	const char      *name;  /**< field's name. */
	uint32_t        offset; /**< offset to field's data. */
	obj_field_type  type;   /**< field's data type. */
	uint32_t        flags;  /**< is_writable:1bit */
} obj_field;

typedef enum {
	REG_OBJECT,
	REG_PACKAGE,
	REG_META,
} module_reg_type;

typedef struct reg_impl {
	const char *if_name;
	const void *impl;
} reg_impl;

typedef struct reg_sub_module {
	obj_type        *type;
	module_reg_type req_type;
	const luaL_Reg  *pub_funcs;
	const luaL_Reg  *methods;
	const luaL_Reg  *metas;
	const obj_base  *bases;
	const obj_field *fields;
	const obj_const *constants;
	const reg_impl  *implements;
	int             bidirectional_consts;
} reg_sub_module;

#define OBJ_UDATA_FLAG_OWN (1<<0)
#define OBJ_UDATA_FLAG_LOOKUP (1<<1)
#define OBJ_UDATA_LAST_FLAG (OBJ_UDATA_FLAG_LOOKUP)
typedef struct obj_udata {
	void     *obj;
	uint32_t flags;  /**< lua_own:1bit */
} obj_udata;

/* use static pointer as key to weak userdata table. */
static char obj_udata_weak_ref_key[] = "obj_udata_weak_ref_key";

/* use static pointer as key to module's private table. */
static char obj_udata_private_key[] = "obj_udata_private_key";

#if LUAJIT_FFI
typedef int (*ffi_export_func_t)(void);
typedef struct ffi_export_symbol {
	const char *name;
	union {
	void               *data;
	ffi_export_func_t  func;
	} sym;
} ffi_export_symbol;
#endif



typedef int GitError;

static void error_code__GitError__push(lua_State *L, GitError err);
static void dyn_caster_Object(void **obj, obj_type **type);


static obj_type obj_types[] = {
#define obj_type_id_CheckoutOptions 0
#define obj_type_CheckoutOptions (obj_types[obj_type_id_CheckoutOptions])
  { NULL, 0, OBJ_TYPE_FLAG_WEAK_REF, "CheckoutOptions" },
#define obj_type_id_DiffFindOptions 1
#define obj_type_DiffFindOptions (obj_types[obj_type_id_DiffFindOptions])
  { NULL, 1, OBJ_TYPE_FLAG_WEAK_REF, "DiffFindOptions" },
#define obj_type_id_DiffOptions 2
#define obj_type_DiffOptions (obj_types[obj_type_id_DiffOptions])
  { NULL, 2, OBJ_TYPE_FLAG_WEAK_REF, "DiffOptions" },
#define obj_type_id_DiffDelta 3
#define obj_type_DiffDelta (obj_types[obj_type_id_DiffDelta])
  { NULL, 3, OBJ_TYPE_FLAG_WEAK_REF, "DiffDelta" },
#define obj_type_id_DiffStats 4
#define obj_type_DiffStats (obj_types[obj_type_id_DiffStats])
  { NULL, 4, OBJ_TYPE_FLAG_WEAK_REF, "DiffStats" },
#define obj_type_id_DiffFile 5
#define obj_type_DiffFile (obj_types[obj_type_id_DiffFile])
  { NULL, 5, OBJ_TYPE_FLAG_WEAK_REF, "DiffFile" },
#define obj_type_id_Diff 6
#define obj_type_Diff (obj_types[obj_type_id_Diff])
  { NULL, 6, OBJ_TYPE_FLAG_WEAK_REF, "Diff" },
#define obj_type_id_StatusOptions 7
#define obj_type_StatusOptions (obj_types[obj_type_id_StatusOptions])
  { NULL, 7, OBJ_TYPE_FLAG_WEAK_REF, "StatusOptions" },
#define obj_type_id_StatusList 8
#define obj_type_StatusList (obj_types[obj_type_id_StatusList])
  { NULL, 8, OBJ_TYPE_FLAG_WEAK_REF, "StatusList" },
#define obj_type_id_StatusEntry 9
#define obj_type_StatusEntry (obj_types[obj_type_id_StatusEntry])
  { NULL, 9, OBJ_TYPE_FLAG_WEAK_REF, "StatusEntry" },
#define obj_type_id_StrArray 10
#define obj_type_StrArray (obj_types[obj_type_id_StrArray])
  { NULL, 10, OBJ_TYPE_SIMPLE, "StrArray" },
#define obj_type_id_Repository 11
#define obj_type_Repository (obj_types[obj_type_id_Repository])
  { NULL, 11, OBJ_TYPE_FLAG_WEAK_REF, "Repository" },
#define obj_type_id_Config 12
#define obj_type_Config (obj_types[obj_type_id_Config])
  { NULL, 12, OBJ_TYPE_FLAG_WEAK_REF, "Config" },
#define obj_type_id_OdbObject 13
#define obj_type_OdbObject (obj_types[obj_type_id_OdbObject])
  { NULL, 13, OBJ_TYPE_FLAG_WEAK_REF, "OdbObject" },
#define obj_type_id_OID 14
#define obj_type_OID (obj_types[obj_type_id_OID])
  { NULL, 14, OBJ_TYPE_SIMPLE, "OID" },
#define obj_type_id_OID_Shorten 15
#define obj_type_OID_Shorten (obj_types[obj_type_id_OID_Shorten])
  { NULL, 15, OBJ_TYPE_FLAG_WEAK_REF, "OID_Shorten" },
#define obj_type_id_ODB 16
#define obj_type_ODB (obj_types[obj_type_id_ODB])
  { NULL, 16, OBJ_TYPE_FLAG_WEAK_REF, "ODB" },
#define obj_type_id_ODBBackend 17
#define obj_type_ODBBackend (obj_types[obj_type_id_ODBBackend])
  { NULL, 17, OBJ_TYPE_FLAG_WEAK_REF, "ODBBackend" },
#define obj_type_id_Index 18
#define obj_type_Index (obj_types[obj_type_id_Index])
  { NULL, 18, OBJ_TYPE_FLAG_WEAK_REF, "Index" },
#define obj_type_id_IndexEntry 19
#define obj_type_IndexEntry (obj_types[obj_type_id_IndexEntry])
  { NULL, 19, OBJ_TYPE_FLAG_WEAK_REF, "IndexEntry" },
#define obj_type_id_IndexEntryUnmerged 20
#define obj_type_IndexEntryUnmerged (obj_types[obj_type_id_IndexEntryUnmerged])
  { NULL, 20, OBJ_TYPE_FLAG_WEAK_REF, "IndexEntryUnmerged" },
#define obj_type_id_Object 21
#define obj_type_Object (obj_types[obj_type_id_Object])
  { dyn_caster_Object, 21, OBJ_TYPE_FLAG_WEAK_REF, "Object" },
#define obj_type_id_Blob 22
#define obj_type_Blob (obj_types[obj_type_id_Blob])
  { NULL, 22, OBJ_TYPE_FLAG_WEAK_REF, "Blob" },
#define obj_type_id_Signature 23
#define obj_type_Signature (obj_types[obj_type_id_Signature])
  { NULL, 23, OBJ_TYPE_FLAG_WEAK_REF, "Signature" },
#define obj_type_id_Commit 24
#define obj_type_Commit (obj_types[obj_type_id_Commit])
  { NULL, 24, OBJ_TYPE_FLAG_WEAK_REF, "Commit" },
#define obj_type_id_Tree 25
#define obj_type_Tree (obj_types[obj_type_id_Tree])
  { NULL, 25, OBJ_TYPE_FLAG_WEAK_REF, "Tree" },
#define obj_type_id_TreeEntry 26
#define obj_type_TreeEntry (obj_types[obj_type_id_TreeEntry])
  { NULL, 26, OBJ_TYPE_FLAG_WEAK_REF, "TreeEntry" },
#define obj_type_id_Tag 27
#define obj_type_Tag (obj_types[obj_type_id_Tag])
  { NULL, 27, OBJ_TYPE_FLAG_WEAK_REF, "Tag" },
#define obj_type_id_RevWalk 28
#define obj_type_RevWalk (obj_types[obj_type_id_RevWalk])
  { NULL, 28, OBJ_TYPE_FLAG_WEAK_REF, "RevWalk" },
#define obj_type_id_Reference 29
#define obj_type_Reference (obj_types[obj_type_id_Reference])
  { NULL, 29, OBJ_TYPE_FLAG_WEAK_REF, "Reference" },
  {NULL, -1, 0, NULL},
};


#if LUAJIT_FFI

/* nobj_ffi_support_enabled_hint should be set to 1 when FFI support is enabled in at-least one
 * instance of a LuaJIT state.  It should never be set back to 0. */
static int nobj_ffi_support_enabled_hint = 0;
static const char nobj_ffi_support_key[] = "LuaNativeObject_FFI_SUPPORT";
static const char nobj_check_ffi_support_code[] =
"local stat, ffi=pcall(require,\"ffi\")\n" /* try loading LuaJIT`s FFI module. */
"if not stat then return false end\n"
"return true\n";

static int nobj_check_ffi_support(lua_State *L) {
	int rc;
	int err;

	/* check if ffi test has already been done. */
	lua_pushstring(L, nobj_ffi_support_key);
	lua_rawget(L, LUA_REGISTRYINDEX);
	if(!lua_isnil(L, -1)) {
		rc = lua_toboolean(L, -1);
		lua_pop(L, 1);
		/* use results of previous check. */
		goto finished;
	}
	lua_pop(L, 1); /* pop nil. */

	err = luaL_loadbuffer(L, nobj_check_ffi_support_code,
		sizeof(nobj_check_ffi_support_code) - 1, nobj_ffi_support_key);
	if(0 == err) {
		err = lua_pcall(L, 0, 1, 0);
	}
	if(err) {
		const char *msg = "<err not a string>";
		if(lua_isstring(L, -1)) {
			msg = lua_tostring(L, -1);
		}
		printf("Error when checking for FFI-support: %s\n", msg);
		lua_pop(L, 1); /* pop error message. */
		return 0;
	}
	/* check results of test. */
	rc = lua_toboolean(L, -1);
	lua_pop(L, 1); /* pop results. */
		/* cache results. */
	lua_pushstring(L, nobj_ffi_support_key);
	lua_pushboolean(L, rc);
	lua_rawset(L, LUA_REGISTRYINDEX);

finished:
	/* turn-on hint that there is FFI code enabled. */
	if(rc) {
		nobj_ffi_support_enabled_hint = 1;
	}

	return rc;
}

typedef struct {
	const char **ffi_init_code;
	int offset;
} nobj_reader_state;

static const char *nobj_lua_Reader(lua_State *L, void *data, size_t *size) {
	nobj_reader_state *state = (nobj_reader_state *)data;
	const char *ptr;

	(void)L;
	ptr = state->ffi_init_code[state->offset];
	if(ptr != NULL) {
		*size = strlen(ptr);
		state->offset++;
	} else {
		*size = 0;
	}
	return ptr;
}

static int nobj_try_loading_ffi(lua_State *L, const char *ffi_mod_name,
		const char *ffi_init_code[], const ffi_export_symbol *ffi_exports, int priv_table)
{
	nobj_reader_state state = { ffi_init_code, 0 };
	int err;

	/* export symbols to priv_table. */
	while(ffi_exports->name != NULL) {
		lua_pushstring(L, ffi_exports->name);
		lua_pushlightuserdata(L, ffi_exports->sym.data);
		lua_settable(L, priv_table);
		ffi_exports++;
	}
	err = lua_load_no_mode(L, nobj_lua_Reader, &state, ffi_mod_name);
	if(0 == err) {
		lua_pushvalue(L, -2); /* dup C module's table. */
		lua_pushvalue(L, priv_table); /* move priv_table to top of stack. */
		lua_remove(L, priv_table);
		lua_pushvalue(L, LUA_REGISTRYINDEX);
		err = lua_pcall(L, 3, 0, 0);
	}
	if(err) {
		const char *msg = "<err not a string>";
		if(lua_isstring(L, -1)) {
			msg = lua_tostring(L, -1);
		}
		printf("Failed to install FFI-based bindings: %s\n", msg);
		lua_pop(L, 1); /* pop error message. */
	}
	return err;
}
#endif


typedef struct {
	void *impl;
	void *obj;
} obj_implement;

static FUNC_UNUSED void *obj_implement_luaoptional(lua_State *L, int _index, void **impl, char *if_name) {
	void *ud;
	if(lua_isnoneornil(L, _index)) {
		return NULL;
	}
	/* get the implements table for this interface. */
	lua_pushlightuserdata(L, if_name);
	lua_rawget(L, LUA_REGISTRYINDEX);

	/* get pointer to userdata value & check if it is a userdata value. */
	ud = (obj_implement *)lua_touserdata(L, _index);
	if(ud != NULL) {
		/* get the userdata's metatable */
		if(lua_getmetatable(L, _index)) {
			/* lookup metatable in interface table for this object's implementation of the interface. */
			lua_gettable(L, -2);
		} else {
			/* no metatable. */
			goto no_interface;
		}
#if LUAJIT_FFI
	} else if(nobj_ffi_support_enabled_hint) { /* handle cdata. */
		/* get cdata interface check function from interface table. */
		lua_getfield(L, -1, "cdata");
		if(lua_isfunction(L, -1)) {
			/* pass cdata to function, return value should be an implmentation. */
			lua_pushvalue(L, _index);
			lua_call(L, 1, 1);
			/* get pointer to cdata. */
			ud = (void *)lua_topointer(L, _index);
		} else {
			lua_pop(L, 1); /* pop non-function. */
			goto no_interface;
		}
#endif
	} else {
		goto no_interface;
	}

	if(!lua_isnil(L, -1)) {
		*impl = lua_touserdata(L, -1);
		lua_pop(L, 2); /* pop interface table & implementation. */
		/* object implements interface. */
		return ud;
	} else {
		lua_pop(L, 1); /* pop nil. */
	}
no_interface:
	lua_pop(L, 1); /* pop interface table. */
	return NULL;
}

static FUNC_UNUSED void *obj_implement_luacheck(lua_State *L, int _index, void **impl, char *type) {
	void *ud = obj_implement_luaoptional(L, _index, impl, type);
	if(ud == NULL) {
#define ERROR_BUFFER_SIZE 256
		char buf[ERROR_BUFFER_SIZE];
		snprintf(buf, ERROR_BUFFER_SIZE-1,"Expected object with %s interface", type);
		/* value doesn't implement this interface. */
		luaL_argerror(L, _index, buf);
	}
	return ud;
}

/* use static pointer as key to interfaces table. (version 1.0) */
static char obj_interfaces_table_key[] = "obj_interfaces<1.0>_table_key";

static void obj_get_global_interfaces_table(lua_State *L) {
	/* get global interfaces table. */
	lua_getfield(L, LUA_REGISTRYINDEX, obj_interfaces_table_key);
	if(lua_isnil(L, -1)) {
		/* Need to create global interfaces table. */
		lua_pop(L, 1); /* pop nil */
		lua_createtable(L, 0, 4); /* 0 size array part, small hash part. */
		lua_pushvalue(L, -1); /* dup table. */
		/* store interfaces table in Lua registery. */
		lua_setfield(L, LUA_REGISTRYINDEX, obj_interfaces_table_key);
	}
}

static void obj_get_interface(lua_State *L, const char *name, int global_if_tab) {
	/* get a interface's implementation table */
	lua_getfield(L, global_if_tab, name);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil */
		/* new interface. (i.e. no object implement it yet.)
		 *
		 * create an empty table for this interface that will be used when an
		 * implementation is registered for this interface.
		 */
		lua_createtable(L, 0, 2); /* 0 size array part, small hash part. */
		lua_pushvalue(L, -1); /* dup table. */
		lua_setfield(L, global_if_tab, name); /* store interface in global interfaces table. */
	}
}

static int obj_get_userdata_interface(lua_State *L) {
	/* get the userdata's metatable */
	if(lua_getmetatable(L, 2)) {
		/* lookup metatable in interface table for the userdata's implementation of the interface. */
		lua_gettable(L, 1);
		if(!lua_isnil(L, -1)) {
			/* return the implementation. */
			return 1;
		}
	}
	/* no metatable or no implementation. */
	return 0;
}

static void obj_interface_register(lua_State *L, char *name, int global_if_tab) {
	/* get the table of implementations for this interface. */
	obj_get_interface(L, name, global_if_tab);

	/* check for 'userdata' function. */
	lua_getfield(L, -1, "userdata");
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		/* add C function for getting a userdata's implementation. */
		lua_pushcfunction(L, obj_get_userdata_interface);
		lua_setfield(L, -2, "userdata");
	} else {
		/* already have function. */
		lua_pop(L, 1); /* pop C function. */
	}
	/* we are going to use a lightuserdata pointer for fast lookup of the interface's impl. table. */
	lua_pushlightuserdata(L, name);
	lua_insert(L, -2);
	lua_settable(L, LUA_REGISTRYINDEX);
}

static void obj_register_interfaces(lua_State *L, char *interfaces[]) {
	int i;
	int if_tab;
	/* get global interfaces table. */
	obj_get_global_interfaces_table(L);
	if_tab = lua_gettop(L);

	for(i = 0; interfaces[i] != NULL ; i++) {
		obj_interface_register(L, interfaces[i], if_tab);
	}
	lua_pop(L, 1); /* pop global interfaces table. */
}

static void obj_type_register_implement(lua_State *L, const reg_impl *impl, int global_if_tab, int mt_tab) {
	/* get the table of implementations for this interface. */
	obj_get_interface(L, impl->if_name, global_if_tab);

	/* register object's implement in the interface table. */
	lua_pushvalue(L, mt_tab);
	lua_pushlightuserdata(L, (void *)impl->impl);
	lua_settable(L, -3);

	lua_pop(L, 1); /* pop inteface table. */
}

static void obj_type_register_implements(lua_State *L, const reg_impl *impls) {
	int if_tab;
	int mt_tab;
	/* get absolute position of object's metatable. */
	mt_tab = lua_gettop(L);
	/* get global interfaces table. */
	obj_get_global_interfaces_table(L);
	if_tab = lua_gettop(L);

	for(; impls->if_name != NULL ; impls++) {
		obj_type_register_implement(L, impls, if_tab, mt_tab);
	}
	lua_pop(L, 1); /* pop global interfaces table. */
}

#ifndef REG_PACKAGE_IS_CONSTRUCTOR
#define REG_PACKAGE_IS_CONSTRUCTOR 1
#endif

#ifndef REG_MODULES_AS_GLOBALS
#define REG_MODULES_AS_GLOBALS 0
#endif

/* For Lua >=5.2 don't register modules as globals. */
#if LUA_VERSION_NUM >= 502
#undef REG_MODULES_AS_GLOBALS
#define REG_MODULES_AS_GLOBALS 0
#endif

#ifndef REG_OBJECTS_AS_GLOBALS
#define REG_OBJECTS_AS_GLOBALS 0
#endif

#ifndef OBJ_DATA_HIDDEN_METATABLE
#define OBJ_DATA_HIDDEN_METATABLE 1
#endif

static FUNC_UNUSED int obj_import_external_type(lua_State *L, obj_type *type) {
	/* find the external type's metatable using it's name. */
	lua_pushstring(L, type->name);
	lua_rawget(L, LUA_REGISTRYINDEX); /* external type's metatable. */
	if(!lua_isnil(L, -1)) {
		/* found it.  Now we will map our 'type' pointer to the metatable. */
		/* REGISTERY[lightuserdata<type>] = REGISTERY[type->name] */
		lua_pushlightuserdata(L, type); /* use our 'type' pointer as lookup key. */
		lua_pushvalue(L, -2); /* dup. type's metatable. */
		lua_rawset(L, LUA_REGISTRYINDEX); /* save external type's metatable. */
		/* NOTE: top of Lua stack still has the type's metatable. */
		return 1;
	} else {
		lua_pop(L, 1); /* pop nil. */
	}
	return 0;
}

static FUNC_UNUSED int obj_import_external_ffi_type(lua_State *L, obj_type *type) {
	/* find the external type's metatable using it's name. */
	lua_pushstring(L, type->name);
	lua_rawget(L, LUA_REGISTRYINDEX); /* external type's metatable. */
	if(!lua_isnil(L, -1)) {
		/* found it.  Now we will map our 'type' pointer to the C check function. */
		/* _priv_table[lightuserdata<type>] = REGISTERY[type->name].c_check */
		lua_getfield(L, -1, "c_check");
		lua_remove(L, -2); /* remove metatable. */
		if(lua_isfunction(L, -1)) {
			lua_pushlightuserdata(L, type); /* use our 'type' pointer as lookup key. */
			lua_pushvalue(L, -2); /* dup. check function */
			lua_rawset(L, -4); /* save check function to module's private table. */
			/* NOTE: top of Lua stack still has the type's C check function. */
			return 1;
		} else {
			lua_pop(L, 1); /* pop non function value. */
		}
	} else {
		lua_pop(L, 1); /* pop nil. */
	}
	return 0;
}

static FUNC_UNUSED obj_udata *obj_udata_toobj(lua_State *L, int _index) {
	obj_udata *ud;
	size_t len;

	/* make sure it's a userdata value. */
	ud = (obj_udata *)lua_touserdata(L, _index);
	if(ud == NULL) {
		luaL_typerror(L, _index, "userdata"); /* is not a userdata value. */
	}
	/* verify userdata size. */
	len = lua_rawlen(L, _index);
	if(len != sizeof(obj_udata)) {
		/* This shouldn't be possible */
		luaL_error(L, "invalid userdata size: size=%d, expected=%d", len, sizeof(obj_udata));
	}
	return ud;
}

static FUNC_UNUSED int obj_udata_is_compatible(lua_State *L, obj_udata *ud, void **obj, base_caster_t *caster, obj_type *type) {
	obj_base *base;
	obj_type *ud_type;
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
recheck_metatable:
	if(lua_rawequal(L, -1, -2)) {
		*obj = ud->obj;
		/* same type no casting needed. */
		return 1;
	} else if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		if((type->flags & OBJ_TYPE_IMPORT) == 0) {
			/* can't resolve internal type. */
			luaL_error(L, "Unknown object type(id=%d, name=%s)", type->id, type->name);
		}
		/* try to import external type. */
		if(obj_import_external_type(L, type)) {
			/* imported type, re-try metatable check. */
			goto recheck_metatable;
		}
		/* External type not yet available, so the object can't be compatible. */
	} else {
		/* Different types see if we can cast to the required type. */
		lua_rawgeti(L, -2, type->id);
		base = lua_touserdata(L, -1);
		lua_pop(L, 1); /* pop obj_base or nil */
		if(base != NULL) {
			*caster = base->bcaster;
			/* get the obj_type for this userdata. */
			lua_pushliteral(L, ".type");
			lua_rawget(L, -3); /* type's metatable. */
			ud_type = lua_touserdata(L, -1);
			lua_pop(L, 1); /* pop obj_type or nil */
			if(base == NULL) {
				luaL_error(L, "bad userdata, missing type info.");
				return 0;
			}
			/* check if userdata is a simple object. */
			if(ud_type->flags & OBJ_TYPE_SIMPLE) {
				*obj = ud;
			} else {
				*obj = ud->obj;
			}
			return 1;
		}
	}
	return 0;
}

static FUNC_UNUSED obj_udata *obj_udata_luacheck_internal(lua_State *L, int _index, void **obj, obj_type *type, int not_delete) {
	obj_udata *ud;
	base_caster_t caster = NULL;
	/* make sure it's a userdata value. */
	ud = (obj_udata *)lua_touserdata(L, _index);
	if(ud != NULL) {
		/* check object type by comparing metatables. */
		if(lua_getmetatable(L, _index)) {
			if(obj_udata_is_compatible(L, ud, obj, &(caster), type)) {
				lua_pop(L, 2); /* pop both metatables. */
				/* apply caster function if needed. */
				if(caster != NULL && *obj != NULL) {
					caster(obj);
				}
				/* check object pointer. */
				if(*obj == NULL) {
					if(not_delete) {
						luaL_error(L, "null %s", type->name); /* object was garbage collected? */
					}
					return NULL;
				}
				return ud;
			}
		}
	} else if(!lua_isnoneornil(L, _index)) {
		/* handle cdata. */
		/* get private table. */
		lua_pushlightuserdata(L, obj_udata_private_key);
		lua_rawget(L, LUA_REGISTRYINDEX); /* private table. */
		/* get cdata type check function from private table. */
		lua_pushlightuserdata(L, type);
		lua_rawget(L, -2);

		/* check for function. */
		if(!lua_isnil(L, -1)) {
got_check_func:
			/* pass cdata value to type checking function. */
			lua_pushvalue(L, _index);
			lua_call(L, 1, 1);
			if(!lua_isnil(L, -1)) {
				/* valid type get pointer from cdata. */
				lua_pop(L, 2);
				*obj = *(void **)lua_topointer(L, _index);
				return ud;
			}
			lua_pop(L, 2);
		} else {
			lua_pop(L, 1); /* pop nil. */
			if(type->flags & OBJ_TYPE_IMPORT) {
				/* try to import external ffi type. */
				if(obj_import_external_ffi_type(L, type)) {
					/* imported type. */
					goto got_check_func;
				}
				/* External type not yet available, so the object can't be compatible. */
			}
		}
	}
	if(not_delete) {
		luaL_typerror(L, _index, type->name); /* is not a userdata value. */
	}
	return NULL;
}

static FUNC_UNUSED void *obj_udata_luacheck(lua_State *L, int _index, obj_type *type) {
	void *obj = NULL;
	obj_udata_luacheck_internal(L, _index, &(obj), type, 1);
	return obj;
}

static FUNC_UNUSED void *obj_udata_luaoptional(lua_State *L, int _index, obj_type *type) {
	void *obj = NULL;
	if(lua_isnoneornil(L, _index)) {
		return obj;
	}
	obj_udata_luacheck_internal(L, _index, &(obj), type, 1);
	return obj;
}

static FUNC_UNUSED void *obj_udata_luadelete(lua_State *L, int _index, obj_type *type, int *flags) {
	void *obj;
	obj_udata *ud = obj_udata_luacheck_internal(L, _index, &(obj), type, 0);
	if(ud == NULL) return NULL;
	*flags = ud->flags;
	/* null userdata. */
	ud->obj = NULL;
	ud->flags = 0;
	/* clear the metatable in invalidate userdata. */
	lua_pushnil(L);
	lua_setmetatable(L, _index);
	return obj;
}

static FUNC_UNUSED void obj_udata_luapush(lua_State *L, void *obj, obj_type *type, int flags) {
	obj_udata *ud;
	/* convert NULL's into Lua nil's. */
	if(obj == NULL) {
		lua_pushnil(L);
		return;
	}
#if LUAJIT_FFI
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	if(nobj_ffi_support_enabled_hint && lua_isfunction(L, -1)) {
		/* call special FFI "void *" to FFI object convertion function. */
		lua_pushlightuserdata(L, obj);
		lua_pushinteger(L, flags);
		lua_call(L, 2, 1);
		return;
	}
#endif
	/* check for type caster. */
	if(type->dcaster) {
		(type->dcaster)(&obj, &type);
	}
	/* create new userdata. */
	ud = (obj_udata *)lua_newuserdata(L, sizeof(obj_udata));
	ud->obj = obj;
	ud->flags = flags;
	/* get obj_type metatable. */
#if LUAJIT_FFI
	lua_insert(L, -2); /* move userdata below metatable. */
#else
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
#endif
	lua_setmetatable(L, -2);
}

static FUNC_UNUSED void *obj_udata_luadelete_weak(lua_State *L, int _index, obj_type *type, int *flags) {
	void *obj;
	obj_udata *ud = obj_udata_luacheck_internal(L, _index, &(obj), type, 0);
	if(ud == NULL) return NULL;
	*flags = ud->flags;
	/* null userdata. */
	ud->obj = NULL;
	ud->flags = 0;
	/* clear the metatable in invalidate userdata. */
	lua_pushnil(L);
	lua_setmetatable(L, _index);
	/* get objects weak table. */
	lua_pushlightuserdata(L, obj_udata_weak_ref_key);
	lua_rawget(L, LUA_REGISTRYINDEX); /* weak ref table. */
	/* remove object from weak table. */
	lua_pushlightuserdata(L, obj);
	lua_pushnil(L);
	lua_rawset(L, -3);
	return obj;
}

static FUNC_UNUSED void obj_udata_luapush_weak(lua_State *L, void *obj, obj_type *type, int flags) {
	obj_udata *ud;

	/* convert NULL's into Lua nil's. */
	if(obj == NULL) {
		lua_pushnil(L);
		return;
	}
	/* check for type caster. */
	if(type->dcaster) {
		(type->dcaster)(&obj, &type);
	}
	/* get objects weak table. */
	lua_pushlightuserdata(L, obj_udata_weak_ref_key);
	lua_rawget(L, LUA_REGISTRYINDEX); /* weak ref table. */
	/* lookup userdata instance from pointer. */
	lua_pushlightuserdata(L, obj);
	lua_rawget(L, -2);
	if(!lua_isnil(L, -1)) {
		lua_remove(L, -2);     /* remove objects table. */
		return;
	}
	lua_pop(L, 1);  /* pop nil. */

#if LUAJIT_FFI
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	if(nobj_ffi_support_enabled_hint && lua_isfunction(L, -1)) {
		lua_remove(L, -2);
		/* call special FFI "void *" to FFI object convertion function. */
		lua_pushlightuserdata(L, obj);
		lua_pushinteger(L, flags);
		lua_call(L, 2, 1);
		return;
	}
#endif
	/* create new userdata. */
	ud = (obj_udata *)lua_newuserdata(L, sizeof(obj_udata));

	/* init. obj_udata. */
	ud->obj = obj;
	ud->flags = flags;
	/* get obj_type metatable. */
#if LUAJIT_FFI
	lua_insert(L, -2); /* move userdata below metatable. */
#else
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
#endif
	lua_setmetatable(L, -2);

	/* add weak reference to object. */
	lua_pushlightuserdata(L, obj); /* push object pointer as the 'key' */
	lua_pushvalue(L, -2);          /* push object's udata */
	lua_rawset(L, -4);             /* add weak reference to object. */
	lua_remove(L, -2);     /* remove objects table. */
}

/* default object equal method. */
static FUNC_UNUSED int obj_udata_default_equal(lua_State *L) {
	obj_udata *ud1 = obj_udata_toobj(L, 1);
	obj_udata *ud2 = obj_udata_toobj(L, 2);

	lua_pushboolean(L, (ud1->obj == ud2->obj));
	return 1;
}

/* default object tostring method. */
static FUNC_UNUSED int obj_udata_default_tostring(lua_State *L) {
	obj_udata *ud = obj_udata_toobj(L, 1);

	/* get object's metatable. */
	lua_getmetatable(L, 1);
	lua_remove(L, 1); /* remove userdata. */
	/* get the object's name from the metatable */
	lua_getfield(L, 1, ".name");
	lua_remove(L, 1); /* remove metatable */
	/* push object's pointer */
	lua_pushfstring(L, ": %p, flags=%d", ud->obj, ud->flags);
	lua_concat(L, 2);

	return 1;
}

/*
 * Simple userdata objects.
 */
static FUNC_UNUSED void *obj_simple_udata_toobj(lua_State *L, int _index) {
	void *ud;

	/* make sure it's a userdata value. */
	ud = lua_touserdata(L, _index);
	if(ud == NULL) {
		luaL_typerror(L, _index, "userdata"); /* is not a userdata value. */
	}
	return ud;
}

static FUNC_UNUSED void * obj_simple_udata_luacheck(lua_State *L, int _index, obj_type *type) {
	void *ud;
	/* make sure it's a userdata value. */
	ud = lua_touserdata(L, _index);
	if(ud != NULL) {
		/* check object type by comparing metatables. */
		if(lua_getmetatable(L, _index)) {
			lua_pushlightuserdata(L, type);
			lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
recheck_metatable:
			if(lua_rawequal(L, -1, -2)) {
				lua_pop(L, 2); /* pop both metatables. */
				return ud;
			} else if(lua_isnil(L, -1)) {
				lua_pop(L, 1); /* pop nil. */
				if((type->flags & OBJ_TYPE_IMPORT) == 0) {
					/* can't resolve internal type. */
					luaL_error(L, "Unknown object type(id=%d, name=%s)", type->id, type->name);
				}
				/* try to import external type. */
				if(obj_import_external_type(L, type)) {
					/* imported type, re-try metatable check. */
					goto recheck_metatable;
				}
				/* External type not yet available, so the object can't be compatible. */
				return 0;
			}
		}
	} else if(!lua_isnoneornil(L, _index)) {
		/* handle cdata. */
		/* get private table. */
		lua_pushlightuserdata(L, obj_udata_private_key);
		lua_rawget(L, LUA_REGISTRYINDEX); /* private table. */
		/* get cdata type check function from private table. */
		lua_pushlightuserdata(L, type);
		lua_rawget(L, -2);

		/* check for function. */
		if(!lua_isnil(L, -1)) {
got_check_func:
			/* pass cdata value to type checking function. */
			lua_pushvalue(L, _index);
			lua_call(L, 1, 1);
			if(!lua_isnil(L, -1)) {
				/* valid type get pointer from cdata. */
				lua_pop(L, 2);
				return (void *)lua_topointer(L, _index);
			}
		} else {
			if(type->flags & OBJ_TYPE_IMPORT) {
				/* try to import external ffi type. */
				if(obj_import_external_ffi_type(L, type)) {
					/* imported type. */
					goto got_check_func;
				}
				/* External type not yet available, so the object can't be compatible. */
			}
		}
	}
	luaL_typerror(L, _index, type->name); /* is not a userdata value. */
	return NULL;
}

static FUNC_UNUSED void * obj_simple_udata_luaoptional(lua_State *L, int _index, obj_type *type) {
	if(lua_isnoneornil(L, _index)) {
		return NULL;
	}
	return obj_simple_udata_luacheck(L, _index, type);
}

static FUNC_UNUSED void * obj_simple_udata_luadelete(lua_State *L, int _index, obj_type *type) {
	void *obj;
	obj = obj_simple_udata_luacheck(L, _index, type);
	/* clear the metatable to invalidate userdata. */
	lua_pushnil(L);
	lua_setmetatable(L, _index);
	return obj;
}

static FUNC_UNUSED void *obj_simple_udata_luapush(lua_State *L, void *obj, int size, obj_type *type)
{
	void *ud;
#if LUAJIT_FFI
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	if(nobj_ffi_support_enabled_hint && lua_isfunction(L, -1)) {
		/* call special FFI "void *" to FFI object convertion function. */
		lua_pushlightuserdata(L, obj);
		lua_call(L, 1, 1);
		return obj;
	}
#endif
	/* create new userdata. */
	ud = lua_newuserdata(L, size);
	memcpy(ud, obj, size);
	/* get obj_type metatable. */
#if LUAJIT_FFI
	lua_insert(L, -2); /* move userdata below metatable. */
#else
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
#endif
	lua_setmetatable(L, -2);

	return ud;
}

/* default simple object equal method. */
static FUNC_UNUSED int obj_simple_udata_default_equal(lua_State *L) {
	void *ud1 = obj_simple_udata_toobj(L, 1);
	size_t len1 = lua_rawlen(L, 1);
	void *ud2 = obj_simple_udata_toobj(L, 2);
	size_t len2 = lua_rawlen(L, 2);

	if(len1 == len2) {
		lua_pushboolean(L, (memcmp(ud1, ud2, len1) == 0));
	} else {
		lua_pushboolean(L, 0);
	}
	return 1;
}

/* default simple object tostring method. */
static FUNC_UNUSED int obj_simple_udata_default_tostring(lua_State *L) {
	void *ud = obj_simple_udata_toobj(L, 1);

	/* get object's metatable. */
	lua_getmetatable(L, 1);
	lua_remove(L, 1); /* remove userdata. */
	/* get the object's name from the metatable */
	lua_getfield(L, 1, ".name");
	lua_remove(L, 1); /* remove metatable */
	/* push object's pointer */
	lua_pushfstring(L, ": %p", ud);
	lua_concat(L, 2);

	return 1;
}

static int obj_constructor_call_wrapper(lua_State *L) {
	/* replace '__call' table with constructor function. */
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_replace(L, 1);

	/* call constructor function with all parameters after the '__call' table. */
	lua_call(L, lua_gettop(L) - 1, LUA_MULTRET);
	/* return all results from constructor. */
	return lua_gettop(L);
}

static void obj_type_register_constants(lua_State *L, const obj_const *constants, int tab_idx,
		int bidirectional) {
	/* register constants. */
	while(constants->name != NULL) {
		lua_pushstring(L, constants->name);
		switch(constants->type) {
		case CONST_BOOLEAN:
			lua_pushboolean(L, constants->num != 0.0);
			break;
		case CONST_NUMBER:
			lua_pushnumber(L, constants->num);
			break;
		case CONST_STRING:
			lua_pushstring(L, constants->str);
			break;
		default:
			lua_pushnil(L);
			break;
		}
		/* map values back to keys. */
		if(bidirectional) {
			/* check if value already exists. */
			lua_pushvalue(L, -1);
			lua_rawget(L, tab_idx - 3);
			if(lua_isnil(L, -1)) {
				lua_pop(L, 1);
				/* add value->key mapping. */
				lua_pushvalue(L, -1);
				lua_pushvalue(L, -3);
				lua_rawset(L, tab_idx - 4);
			} else {
				/* value already exists. */
				lua_pop(L, 1);
			}
		}
		lua_rawset(L, tab_idx - 2);
		constants++;
	}
}

static void obj_type_register_package(lua_State *L, const reg_sub_module *type_reg) {
	const luaL_Reg *reg_list = type_reg->pub_funcs;

	/* create public functions table. */
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register functions */
		nobj_setfuncs(L, reg_list, 0);
	}

	obj_type_register_constants(L, type_reg->constants, -1, type_reg->bidirectional_consts);

	lua_pop(L, 1);  /* drop package table */
}

static void obj_type_register_meta(lua_State *L, const reg_sub_module *type_reg) {
	const luaL_Reg *reg_list;

	/* create public functions table. */
	reg_list = type_reg->pub_funcs;
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register functions */
		nobj_setfuncs(L, reg_list, 0);
	}

	obj_type_register_constants(L, type_reg->constants, -1, type_reg->bidirectional_consts);

	/* register methods. */
	nobj_setfuncs(L, type_reg->methods, 0);

	/* create metatable table. */
	lua_newtable(L);
	nobj_setfuncs(L, type_reg->metas, 0); /* fill metatable */
	/* setmetatable on meta-object. */
	lua_setmetatable(L, -2);

	lua_pop(L, 1);  /* drop meta-object */
}

static void obj_type_register(lua_State *L, const reg_sub_module *type_reg, int priv_table) {
	const luaL_Reg *reg_list;
	obj_type *type = type_reg->type;
	const obj_base *base = type_reg->bases;

	if(type_reg->req_type == REG_PACKAGE) {
		obj_type_register_package(L, type_reg);
		return;
	}
	if(type_reg->req_type == REG_META) {
		obj_type_register_meta(L, type_reg);
		return;
	}

	/* create public functions table. */
	reg_list = type_reg->pub_funcs;
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register "constructors" as to object's public API */
		nobj_setfuncs(L, reg_list, 0); /* fill public API table. */

		/* make public API table callable as the default constructor. */
		lua_newtable(L); /* create metatable */
		lua_pushliteral(L, "__call");
		lua_pushcfunction(L, reg_list[0].func); /* push first constructor function. */
		lua_pushcclosure(L, obj_constructor_call_wrapper, 1); /* make __call wrapper. */
		lua_rawset(L, -3);         /* metatable.__call = <default constructor> */

#if OBJ_DATA_HIDDEN_METATABLE
		lua_pushliteral(L, "__metatable");
		lua_pushboolean(L, 0);
		lua_rawset(L, -3);         /* metatable.__metatable = false */
#endif

		/* setmetatable on public API table. */
		lua_setmetatable(L, -2);

		lua_pop(L, 1); /* pop public API table, don't need it any more. */
		/* create methods table. */
		lua_newtable(L);
	} else {
		/* register all methods as public functions. */
#if OBJ_DATA_HIDDEN_METATABLE
		lua_pop(L, 1); /* pop public API table, don't need it any more. */
		/* create methods table. */
		lua_newtable(L);
#endif
	}

	nobj_setfuncs(L, type_reg->methods, 0); /* fill methods table. */

	luaL_newmetatable(L, type->name); /* create metatable */
	lua_pushliteral(L, ".name");
	lua_pushstring(L, type->name);
	lua_rawset(L, -3);    /* metatable['.name'] = "<object_name>" */
	lua_pushliteral(L, ".type");
	lua_pushlightuserdata(L, type);
	lua_rawset(L, -3);    /* metatable['.type'] = lightuserdata -> obj_type */
	lua_pushlightuserdata(L, type);
	lua_pushvalue(L, -2); /* dup metatable. */
	lua_rawset(L, LUA_REGISTRYINDEX);    /* REGISTRY[type] = metatable */

	/* add metatable to 'priv_table' */
	lua_pushstring(L, type->name);
	lua_pushvalue(L, -2); /* dup metatable. */
	lua_rawset(L, priv_table);    /* priv_table["<object_name>"] = metatable */

	nobj_setfuncs(L, type_reg->metas, 0); /* fill metatable */

	/* add obj_bases to metatable. */
	while(base->id >= 0) {
		lua_pushlightuserdata(L, (void *)base);
		lua_rawseti(L, -2, base->id);
		base++;
	}

	obj_type_register_constants(L, type_reg->constants, -2, type_reg->bidirectional_consts);

	obj_type_register_implements(L, type_reg->implements);

	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -3);               /* dup methods table */
	lua_rawset(L, -3);                  /* metatable.__index = methods */
#if OBJ_DATA_HIDDEN_METATABLE
	lua_pushliteral(L, "__metatable");
	lua_pushboolean(L, 0);
	lua_rawset(L, -3);                  /* hide metatable:
	                                       metatable.__metatable = false */
#endif
	lua_pop(L, 2);                      /* drop metatable & methods */
}

static FUNC_UNUSED int lua_checktype_ref(lua_State *L, int _index, int _type) {
	luaL_checktype(L,_index,_type);
	lua_pushvalue(L,_index);
	return luaL_ref(L, LUA_REGISTRYINDEX);
}

/* use static pointer as key to weak callback_state table. */
static char obj_callback_state_weak_ref_key[] = "obj_callback_state_weak_ref_key";

static FUNC_UNUSED void *nobj_get_callback_state(lua_State *L, int owner_idx, int size) {
	void *cb_state;

	lua_pushlightuserdata(L, obj_callback_state_weak_ref_key); /* key for weak table. */
	lua_rawget(L, LUA_REGISTRYINDEX);  /* check if weak table exists already. */
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		/* create weak table for callback_state */
		lua_newtable(L);               /* weak table. */
		lua_newtable(L);               /* metatable for weak table. */
		lua_pushliteral(L, "__mode");
		lua_pushliteral(L, "k");
		lua_rawset(L, -3);             /* metatable.__mode = 'k'  weak keys. */
		lua_setmetatable(L, -2);       /* add metatable to weak table. */
		lua_pushlightuserdata(L, obj_callback_state_weak_ref_key); /* key for weak table. */
		lua_pushvalue(L, -2);          /* dup weak table. */
		lua_rawset(L, LUA_REGISTRYINDEX);  /* add weak table to registry. */
	}

	/* check weak table for callback_state. */
	lua_pushvalue(L, owner_idx); /* dup. owner as lookup key. */
	lua_rawget(L, -2);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		lua_pushvalue(L, owner_idx); /* dup. owner as lookup key. */
		/* create new callback state. */
		cb_state = lua_newuserdata(L, size);
		lua_rawset(L, -3);
		lua_pop(L, 1); /* pop <weak table> */
	} else {
		/* got existing callback state. */
		cb_state = lua_touserdata(L, -1);
		lua_pop(L, 2); /* pop <weak table>, <callback_state> */
	}

	return cb_state;
}

static FUNC_UNUSED void *nobj_delete_callback_state(lua_State *L, int owner_idx) {
	void *cb_state = NULL;

	lua_pushlightuserdata(L, obj_callback_state_weak_ref_key); /* key for weak table. */
	lua_rawget(L, LUA_REGISTRYINDEX);  /* check if weak table exists already. */
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil.  no weak table, so there is no callback state. */
		return NULL;
	}
	/* get callback state. */
	lua_pushvalue(L, owner_idx); /* dup. owner */
	lua_rawget(L, -2);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 2); /* pop <weak table>, nil.  No callback state for the owner. */
	} else {
		cb_state = lua_touserdata(L, -1);
		lua_pop(L, 1); /* pop <state> */
		/* remove callback state. */
		lua_pushvalue(L, owner_idx); /* dup. owner */
		lua_pushnil(L);
		lua_rawset(L, -3);
		lua_pop(L, 1); /* pop <weak table> */
	}

	return cb_state;
}



static char *obj_interfaces[] = {
  NULL,
};



#define obj_type_CheckoutOptions_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_CheckoutOptions))
#define obj_type_CheckoutOptions_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_CheckoutOptions))
#define obj_type_CheckoutOptions_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_CheckoutOptions), flags)
#define obj_type_CheckoutOptions_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_CheckoutOptions), flags)

#define obj_type_DiffFindOptions_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_DiffFindOptions))
#define obj_type_DiffFindOptions_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_DiffFindOptions))
#define obj_type_DiffFindOptions_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_DiffFindOptions), flags)
#define obj_type_DiffFindOptions_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_DiffFindOptions), flags)

#define obj_type_DiffOptions_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_DiffOptions))
#define obj_type_DiffOptions_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_DiffOptions))
#define obj_type_DiffOptions_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_DiffOptions), flags)
#define obj_type_DiffOptions_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_DiffOptions), flags)

#define obj_type_DiffDelta_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_DiffDelta))
#define obj_type_DiffDelta_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_DiffDelta))
#define obj_type_DiffDelta_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_DiffDelta), flags)
#define obj_type_DiffDelta_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_DiffDelta), flags)

#define obj_type_DiffStats_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_DiffStats))
#define obj_type_DiffStats_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_DiffStats))
#define obj_type_DiffStats_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_DiffStats), flags)
#define obj_type_DiffStats_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_DiffStats), flags)

#define obj_type_DiffFile_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_DiffFile))
#define obj_type_DiffFile_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_DiffFile))
#define obj_type_DiffFile_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_DiffFile), flags)
#define obj_type_DiffFile_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_DiffFile), flags)

#define obj_type_Diff_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_Diff))
#define obj_type_Diff_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_Diff))
#define obj_type_Diff_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_Diff), flags)
#define obj_type_Diff_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_Diff), flags)

#define obj_type_StatusOptions_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_StatusOptions))
#define obj_type_StatusOptions_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_StatusOptions))
#define obj_type_StatusOptions_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_StatusOptions), flags)
#define obj_type_StatusOptions_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_StatusOptions), flags)

#define obj_type_StatusList_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_StatusList))
#define obj_type_StatusList_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_StatusList))
#define obj_type_StatusList_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_StatusList), flags)
#define obj_type_StatusList_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_StatusList), flags)

#define obj_type_StatusEntry_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_StatusEntry))
#define obj_type_StatusEntry_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_StatusEntry))
#define obj_type_StatusEntry_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_StatusEntry), flags)
#define obj_type_StatusEntry_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_StatusEntry), flags)

#define obj_type_StrArray_check(L, _index) \
	(StrArray *)obj_simple_udata_luacheck(L, _index, &(obj_type_StrArray))
#define obj_type_StrArray_optional(L, _index) \
	(StrArray *)obj_simple_udata_luaoptional(L, _index, &(obj_type_StrArray))
#define obj_type_StrArray_delete(L, _index) \
	(StrArray *)obj_simple_udata_luadelete(L, _index, &(obj_type_StrArray))
#define obj_type_StrArray_push(L, obj) \
	obj_simple_udata_luapush(L, obj, sizeof(StrArray), &(obj_type_StrArray))

#define obj_type_Repository_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_Repository))
#define obj_type_Repository_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_Repository))
#define obj_type_Repository_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_Repository), flags)
#define obj_type_Repository_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_Repository), flags)

#define obj_type_Config_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_Config))
#define obj_type_Config_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_Config))
#define obj_type_Config_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_Config), flags)
#define obj_type_Config_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_Config), flags)

#define obj_type_OdbObject_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_OdbObject))
#define obj_type_OdbObject_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_OdbObject))
#define obj_type_OdbObject_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_OdbObject), flags)
#define obj_type_OdbObject_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_OdbObject), flags)

#define obj_type_OID_check(L, _index) \
	*((OID *)obj_simple_udata_luacheck(L, _index, &(obj_type_OID)))
#define obj_type_OID_optional(L, _index) \
	*((OID *)obj_simple_udata_luaoptional(L, _index, &(obj_type_OID)))
#define obj_type_OID_delete(L, _index) \
	*((OID *)obj_simple_udata_luadelete(L, _index, &(obj_type_OID)))
#define obj_type_OID_push(L, obj) \
	obj_simple_udata_luapush(L, &(obj), sizeof(OID), &(obj_type_OID))

#define obj_type_OID_Shorten_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_OID_Shorten))
#define obj_type_OID_Shorten_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_OID_Shorten))
#define obj_type_OID_Shorten_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_OID_Shorten), flags)
#define obj_type_OID_Shorten_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_OID_Shorten), flags)

#define obj_type_ODB_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_ODB))
#define obj_type_ODB_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_ODB))
#define obj_type_ODB_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_ODB), flags)
#define obj_type_ODB_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_ODB), flags)

#define obj_type_ODBBackend_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_ODBBackend))
#define obj_type_ODBBackend_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_ODBBackend))
#define obj_type_ODBBackend_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_ODBBackend), flags)
#define obj_type_ODBBackend_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_ODBBackend), flags)

#define obj_type_Index_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_Index))
#define obj_type_Index_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_Index))
#define obj_type_Index_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_Index), flags)
#define obj_type_Index_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_Index), flags)

#define obj_type_IndexEntry_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_IndexEntry))
#define obj_type_IndexEntry_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_IndexEntry))
#define obj_type_IndexEntry_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_IndexEntry), flags)
#define obj_type_IndexEntry_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_IndexEntry), flags)

#define obj_type_IndexEntryUnmerged_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_IndexEntryUnmerged))
#define obj_type_IndexEntryUnmerged_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_IndexEntryUnmerged))
#define obj_type_IndexEntryUnmerged_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_IndexEntryUnmerged), flags)
#define obj_type_IndexEntryUnmerged_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_IndexEntryUnmerged), flags)

#define obj_type_Object_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_Object))
#define obj_type_Object_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_Object))
#define obj_type_Object_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_Object), flags)
#define obj_type_Object_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_Object), flags)

#define obj_type_Blob_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_Blob))
#define obj_type_Blob_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_Blob))
#define obj_type_Blob_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_Blob), flags)
#define obj_type_Blob_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_Blob), flags)

#define obj_type_Signature_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_Signature))
#define obj_type_Signature_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_Signature))
#define obj_type_Signature_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_Signature), flags)
#define obj_type_Signature_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_Signature), flags)

#define obj_type_Commit_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_Commit))
#define obj_type_Commit_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_Commit))
#define obj_type_Commit_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_Commit), flags)
#define obj_type_Commit_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_Commit), flags)

#define obj_type_Tree_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_Tree))
#define obj_type_Tree_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_Tree))
#define obj_type_Tree_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_Tree), flags)
#define obj_type_Tree_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_Tree), flags)

#define obj_type_TreeEntry_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_TreeEntry))
#define obj_type_TreeEntry_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_TreeEntry))
#define obj_type_TreeEntry_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_TreeEntry), flags)
#define obj_type_TreeEntry_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_TreeEntry), flags)

#define obj_type_Tag_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_Tag))
#define obj_type_Tag_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_Tag))
#define obj_type_Tag_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_Tag), flags)
#define obj_type_Tag_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_Tag), flags)

#define obj_type_RevWalk_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_RevWalk))
#define obj_type_RevWalk_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_RevWalk))
#define obj_type_RevWalk_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_RevWalk), flags)
#define obj_type_RevWalk_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_RevWalk), flags)

#define obj_type_Reference_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_Reference))
#define obj_type_Reference_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_Reference))
#define obj_type_Reference_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_Reference), flags)
#define obj_type_Reference_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_Reference), flags)



typedef git_checkout_options CheckoutOptions;

typedef git_diff_find_options DiffFindOptions;

typedef git_diff_options DiffOptions;

typedef git_diff_delta DiffDelta;

typedef git_diff_stats DiffStats;

typedef git_diff_file DiffFile;

typedef git_diff Diff;

typedef git_status_options StatusOptions;

typedef git_status_list StatusList;

typedef git_status_entry StatusEntry;

typedef git_strarray StrArray;

#include <git2/sys/repository.h>
typedef git_repository Repository;

typedef git_config Config;

typedef git_odb_object OdbObject;

typedef git_oid OID;

typedef git_oid_shorten OID_Shorten;

typedef git_odb ODB;

#include <git2/odb_backend.h>
#include <git2/sys/odb_backend.h>

typedef struct ODBBackend {
	git_odb_backend backend;
	lua_State *L;
	int read;
	int read_prefix;
	int read_header;
	int write;
	int exists;
	int free;
	int ref_count;
} ODBBackend;

static void ODBBackend_ref(ODBBackend *backend) {
	backend->ref_count++;
}

static void ODBBackend_unref(ODBBackend *backend) {
	lua_State *L = backend->L;
	assert(backend->ref_count > 0);
	if((--backend->ref_count) == 0) {
		luaL_unref(L, LUA_REGISTRYINDEX, backend->read);
		luaL_unref(L, LUA_REGISTRYINDEX, backend->read_prefix);
		luaL_unref(L, LUA_REGISTRYINDEX, backend->read_header);
		luaL_unref(L, LUA_REGISTRYINDEX, backend->write);
		luaL_unref(L, LUA_REGISTRYINDEX, backend->exists);
		luaL_unref(L, LUA_REGISTRYINDEX, backend->free);
		free(backend);
	}
}

static int odb_backend_read_cb(void **data_p, size_t *len_p, git_otype *type_p, git_odb_backend *backend, const git_oid *oid)
{
	ODBBackend *lua_backend = (ODBBackend *)backend;
	lua_State *L = lua_backend->L;
	const char *ldata;
	size_t len;
	int err;

	/* get Lua callback function. */
	lua_rawgeti(L, LUA_REGISTRYINDEX, lua_backend->read);

	obj_type_OID_push(L, *((OID *)oid));
	/* call Lua function. */
	lua_call(L, 1, 2);
	ldata = lua_tolstring(L, -2, &len);
	if(ldata) {
		char *data;
		/* parse otype value. */
		int arg_type = lua_type(L, -1);
		if(arg_type == LUA_TNUMBER) {
			*type_p = lua_tointeger(L, -1);
		} else if(arg_type == LUA_TSTRING) {
			*type_p = git_object_string2type(lua_tostring(L, -1));
		}
		*len_p = len;
		/* allocate buffer for data. */
		data = malloc(len);
		*data_p = data;
		if(data == NULL) {
			return GIT_EBUFS; //GIT_ENOMEM;
		}
		/* copy data. */
		memcpy(data, ldata, len);
		err = GIT_OK;
	} else if(lua_isnil(L, -2)) {
		*data_p = NULL;
		/* backend returned an error. */
		err = lua_tointeger(L, -1);
	} else {
		*data_p = NULL;
		/* bad return value from lua backend. */
		err = GIT_EAMBIGUOUS; //GIT_EOBJTYPE;
	}

	return err;
}

static int odb_backend_read_prefix_cb(git_oid *out_oid, void **data_p, size_t *len_p, git_otype *type_p, git_odb_backend *backend, const git_oid *short_oid, size_t len)
{
	*data_p = NULL;
	if(len >= GIT_OID_HEXSZ) {
		int rc = odb_backend_read_cb(data_p, len_p, type_p, backend, short_oid);
		if(rc == GIT_OK) {
			git_oid_cpy(out_oid, short_oid);
		}
		return rc;
	}
	return GIT_EAMBIGUOUS; //GIT_ENOTIMPLEMENTED;
}

static int odb_backend_read_header_cb(size_t *len_p, git_otype *type_p, git_odb_backend *backend, const git_oid *oid)
{
	ODBBackend *lua_backend = (ODBBackend *)backend;
	lua_State *L = lua_backend->L;
	int err;
	int arg_type;

	/* get Lua callback function. */
	lua_rawgeti(L, LUA_REGISTRYINDEX, lua_backend->read_header);

	obj_type_OID_push(L, *((OID *)oid));
	/* call Lua function. */
	lua_call(L, 1, 2);

	arg_type = lua_type(L, -2);
	if(arg_type == LUA_TSTRING || arg_type == LUA_TNUMBER) {
		/* parse data length. */
		*len_p = lua_tonumber(L, -2);
		/* parse otype value. */
		lua_type(L, -1);
		if(arg_type == LUA_TNUMBER) {
			*type_p = lua_tointeger(L, -1);
		} else if(arg_type == LUA_TSTRING) {
			*type_p = git_object_string2type(lua_tostring(L, -1));
		}
		err = GIT_OK;
	} else if(arg_type == LUA_TNIL) {
		/* backend returned an error. */
		err = lua_tointeger(L, -1);
	} else {
		/* bad return value from lua backend. */
		err = GIT_EAMBIGUOUS; //GIT_EOBJTYPE;
	}

	return err;
}

static int odb_backend_write_cb(git_odb_backend *backend, const git_oid *oid, const void *data, size_t len, git_otype type)
{
	ODBBackend *lua_backend = (ODBBackend *)backend;
	lua_State *L = lua_backend->L;

	/* get Lua callback function. */
	lua_rawgeti(L, LUA_REGISTRYINDEX, lua_backend->write);

  /* push oid */
	obj_type_OID_push(L, *((OID *)oid));
	/* push data onto stack. */
	lua_pushlstring(L, data, len);
	/* push otype */
	lua_pushstring(L, git_object_type2string(type));

	/* call Lua function. */
	lua_call(L, 3, 1);
	return lua_tointeger(L, -1);
}

static int odb_backend_exists_cb(git_odb_backend *backend, const git_oid *oid)
{
	ODBBackend *lua_backend = (ODBBackend *)backend;
	lua_State *L = lua_backend->L;

	/* get Lua callback function. */
	lua_rawgeti(L, LUA_REGISTRYINDEX, lua_backend->exists);

	obj_type_OID_push(L, *((OID *)oid));
	/* call Lua function. */
	lua_call(L, 1, 1);
	return lua_tointeger(L, -1);
}

static void odb_backend_free_cb(git_odb_backend *backend)
{
	ODBBackend *lua_backend = (ODBBackend *)backend;
	lua_State *L = lua_backend->L;

	/* get Lua callback function. */
	lua_rawgeti(L, LUA_REGISTRYINDEX, lua_backend->free);

	/* call Lua function. */
	lua_call(L, 0, 0);

	ODBBackend_unref(lua_backend);
}


typedef git_index Index;

typedef git_index_entry IndexEntry;

#include <git2/sys/index.h>
typedef git_index_reuc_entry IndexEntryUnmerged;

typedef git_object Object;


typedef git_blob Blob;

typedef git_signature Signature;


typedef git_commit Commit;


typedef git_tree Tree;

typedef git_tree_entry TreeEntry;


typedef git_tag Tag;

typedef git_revwalk RevWalk;

typedef git_reference Reference;



/* method: version */
static int git2__version__func(lua_State *L) {
	int major, minor, patch;
	git_libgit2_version(&(major), &(minor), &(patch));

	/* return version as a table: { major, minor, patch } */
	lua_createtable(L, 3, 0);
	lua_pushinteger(L, major);
	lua_rawseti(L, -2, 1);
	lua_pushinteger(L, minor);
	lua_rawseti(L, -2, 2);
	lua_pushinteger(L, patch);
	lua_rawseti(L, -2, 3);

  return 1;
}

/* method: init */
static int CheckoutOptions__init__meth(lua_State *L) {
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  CheckoutOptions * this_idx1;
	this_idx1 = calloc(1, sizeof(CheckoutOptions));
	git_checkout_options_init(this_idx1, GIT_STATUS_OPTIONS_VERSION);

  obj_type_CheckoutOptions_push(L, this_idx1, this_flags_idx1);
  return 1;
}

/* method: _priv */
static int CheckoutOptions__delete__meth(lua_State *L) {
  int this_flags_idx1 = 0;
  CheckoutOptions * this_idx1;
  this_idx1 = obj_type_CheckoutOptions_delete(L,1,&(this_flags_idx1));
  if(!(this_flags_idx1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
	free(this_idx1);

  return 0;
}

/* method: set_checkout_strategy */
static int CheckoutOptions__set_checkout_strategy__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  unsigned int checkout_strategy_idx2;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
  checkout_strategy_idx2 = luaL_checkinteger(L,2);
		this_idx1->checkout_strategy = checkout_strategy_idx2;

  return 0;
}

/* method: set_disable_filters */
static int CheckoutOptions__set_disable_filters__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  int disable_filters_idx2;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
  disable_filters_idx2 = luaL_checkinteger(L,2);
		this_idx1->disable_filters = disable_filters_idx2;

  return 0;
}

/* method: set_dir_mode */
static int CheckoutOptions__set_dir_mode__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  unsigned int dir_mode_idx2;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
  dir_mode_idx2 = luaL_checkinteger(L,2);
		this_idx1->dir_mode = dir_mode_idx2;

  return 0;
}

/* method: set_file_mode */
static int CheckoutOptions__set_file_mode__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  unsigned int file_mode_idx2;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
  file_mode_idx2 = luaL_checkinteger(L,2);
		this_idx1->file_mode = file_mode_idx2;

  return 0;
}

/* method: set_file_open_flags */
static int CheckoutOptions__set_file_open_flags__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  int file_open_flags_idx2;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
  file_open_flags_idx2 = luaL_checkinteger(L,2);
		this_idx1->file_open_flags = file_open_flags_idx2;

  return 0;
}

/* method: set_notify_flags */
static int CheckoutOptions__set_notify_flags__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  unsigned int notify_flags_idx2;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
  notify_flags_idx2 = luaL_checkinteger(L,2);
		this_idx1->notify_flags = notify_flags_idx2;

  return 0;
}

/* method: paths */
static int CheckoutOptions__paths__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  StrArray paths_idx1_store;
  StrArray * paths_idx1 = &(paths_idx1_store);
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
		paths_idx1 = &this_idx1->paths;

  obj_type_StrArray_push(L, paths_idx1);
  return 1;
}

/* method: set_paths */
static int CheckoutOptions__set_paths__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  StrArray * paths_idx2;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
  paths_idx2 = obj_type_StrArray_check(L,2);
		this_idx1->paths.strings = paths_idx2->strings;
		this_idx1->paths.count = paths_idx2->count;

  return 0;
}

/* method: set_baseline */
static int CheckoutOptions__set_baseline__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  Tree * baseline_idx2;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
  baseline_idx2 = obj_type_Tree_check(L,2);
		this_idx1->baseline = baseline_idx2;

  return 0;
}

/* method: set_baseline_index */
static int CheckoutOptions__set_baseline_index__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  Index * baseline_index_idx2;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
  baseline_index_idx2 = obj_type_Index_check(L,2);
		this_idx1->baseline_index = baseline_index_idx2;

  return 0;
}

/* method: set_target_directory */
static int CheckoutOptions__set_target_directory__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  size_t target_directory_len_idx2;
  const char * target_directory_idx2;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
  target_directory_idx2 = luaL_checklstring(L,2,&(target_directory_len_idx2));
		this_idx1->target_directory = target_directory_idx2;

  return 0;
}

/* method: set_ancestor_label */
static int CheckoutOptions__set_ancestor_label__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  size_t ancestor_label_len_idx2;
  const char * ancestor_label_idx2;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
  ancestor_label_idx2 = luaL_checklstring(L,2,&(ancestor_label_len_idx2));
		this_idx1->ancestor_label = ancestor_label_idx2;

  return 0;
}

/* method: set_our_label */
static int CheckoutOptions__set_our_label__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  size_t our_label_len_idx2;
  const char * our_label_idx2;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
  our_label_idx2 = luaL_checklstring(L,2,&(our_label_len_idx2));
		this_idx1->our_label = our_label_idx2;

  return 0;
}

/* method: set_their_label */
static int CheckoutOptions__set_their_label__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  size_t their_label_len_idx2;
  const char * their_label_idx2;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
  their_label_idx2 = luaL_checklstring(L,2,&(their_label_len_idx2));
		this_idx1->their_label = their_label_idx2;

  return 0;
}

/* method: version */
static int CheckoutOptions__version__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  unsigned int field_idx1 = 0;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
	field_idx1 = this_idx1->version;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: checkout_strategy */
static int CheckoutOptions__checkout_strategy__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  unsigned int field_idx1 = 0;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
	field_idx1 = this_idx1->checkout_strategy;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: disable_filters */
static int CheckoutOptions__disable_filters__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  int field_idx1 = 0;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
	field_idx1 = this_idx1->disable_filters;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: dir_mode */
static int CheckoutOptions__dir_mode__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  unsigned int field_idx1 = 0;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
	field_idx1 = this_idx1->dir_mode;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: file_mode */
static int CheckoutOptions__file_mode__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  unsigned int field_idx1 = 0;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
	field_idx1 = this_idx1->file_mode;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: file_open_flags */
static int CheckoutOptions__file_open_flags__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  int field_idx1 = 0;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
	field_idx1 = this_idx1->file_open_flags;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: notify_flags */
static int CheckoutOptions__notify_flags__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  unsigned int field_idx1 = 0;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
	field_idx1 = this_idx1->notify_flags;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: baseline */
static int CheckoutOptions__baseline__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  Tree * field_idx1;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
	field_idx1 = this_idx1->baseline;

  obj_type_Tree_push(L, field_idx1, 0);
  return 1;
}

/* method: baseline_index */
static int CheckoutOptions__baseline_index__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  Index * field_idx1;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
	field_idx1 = this_idx1->baseline_index;

  obj_type_Index_push(L, field_idx1, 0);
  return 1;
}

/* method: target_directory */
static int CheckoutOptions__target_directory__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  const char * field_idx1 = NULL;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
	field_idx1 = this_idx1->target_directory;

  lua_pushstring(L, field_idx1);
  return 1;
}

/* method: ancestor_label */
static int CheckoutOptions__ancestor_label__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  const char * field_idx1 = NULL;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
	field_idx1 = this_idx1->ancestor_label;

  lua_pushstring(L, field_idx1);
  return 1;
}

/* method: our_label */
static int CheckoutOptions__our_label__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  const char * field_idx1 = NULL;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
	field_idx1 = this_idx1->our_label;

  lua_pushstring(L, field_idx1);
  return 1;
}

/* method: their_label */
static int CheckoutOptions__their_label__meth(lua_State *L) {
  CheckoutOptions * this_idx1;
  const char * field_idx1 = NULL;
  this_idx1 = obj_type_CheckoutOptions_check(L,1);
	field_idx1 = this_idx1->their_label;

  lua_pushstring(L, field_idx1);
  return 1;
}

/* method: init */
static int DiffFindOptions__init__meth(lua_State *L) {
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  DiffFindOptions * this_idx1;
	this_idx1 = calloc(1, sizeof(DiffFindOptions));
	git_diff_find_options_init(this_idx1, GIT_DIFF_OPTIONS_VERSION);

  obj_type_DiffFindOptions_push(L, this_idx1, this_flags_idx1);
  return 1;
}

/* method: _priv */
static int DiffFindOptions__delete__meth(lua_State *L) {
  int this_flags_idx1 = 0;
  DiffFindOptions * this_idx1;
  this_idx1 = obj_type_DiffFindOptions_delete(L,1,&(this_flags_idx1));
  if(!(this_flags_idx1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
	if(this_idx1->metric != NULL) {
		free((void *)this_idx1->metric);
		this_idx1->metric = NULL;
	}
	free(this_idx1);

  return 0;
}

/* method: set_flags */
static int DiffFindOptions__set_flags__meth(lua_State *L) {
  DiffFindOptions * this_idx1;
  unsigned int flags_idx2;
  this_idx1 = obj_type_DiffFindOptions_check(L,1);
  flags_idx2 = luaL_checkinteger(L,2);
		this_idx1->flags = flags_idx2;

  return 0;
}

/* method: set_rename_threshold */
static int DiffFindOptions__set_rename_threshold__meth(lua_State *L) {
  DiffFindOptions * this_idx1;
  uint16_t rename_threshold_idx2;
  this_idx1 = obj_type_DiffFindOptions_check(L,1);
  rename_threshold_idx2 = luaL_checkinteger(L,2);
		this_idx1->rename_threshold = rename_threshold_idx2;

  return 0;
}

/* method: set_rename_from_rewrite_threshold */
static int DiffFindOptions__set_rename_from_rewrite_threshold__meth(lua_State *L) {
  DiffFindOptions * this_idx1;
  uint16_t rename_from_rewrite_threshold_idx2;
  this_idx1 = obj_type_DiffFindOptions_check(L,1);
  rename_from_rewrite_threshold_idx2 = luaL_checkinteger(L,2);
		this_idx1->rename_from_rewrite_threshold = rename_from_rewrite_threshold_idx2;

  return 0;
}

/* method: set_copy_threshold */
static int DiffFindOptions__set_copy_threshold__meth(lua_State *L) {
  DiffFindOptions * this_idx1;
  uint16_t copy_threshold_idx2;
  this_idx1 = obj_type_DiffFindOptions_check(L,1);
  copy_threshold_idx2 = luaL_checkinteger(L,2);
		this_idx1->copy_threshold = copy_threshold_idx2;

  return 0;
}

/* method: set_break_rewrite_threshold */
static int DiffFindOptions__set_break_rewrite_threshold__meth(lua_State *L) {
  DiffFindOptions * this_idx1;
  uint16_t break_rewrite_threshold_idx2;
  this_idx1 = obj_type_DiffFindOptions_check(L,1);
  break_rewrite_threshold_idx2 = luaL_checkinteger(L,2);
		this_idx1->break_rewrite_threshold = break_rewrite_threshold_idx2;

  return 0;
}

/* method: set_rename_limit */
static int DiffFindOptions__set_rename_limit__meth(lua_State *L) {
  DiffFindOptions * this_idx1;
  size_t rename_limit_idx2;
  this_idx1 = obj_type_DiffFindOptions_check(L,1);
  rename_limit_idx2 = luaL_checkinteger(L,2);
		this_idx1->rename_limit = rename_limit_idx2;

  return 0;
}

/* method: version */
static int DiffFindOptions__version__meth(lua_State *L) {
  DiffFindOptions * this_idx1;
  unsigned int field_idx1 = 0;
  this_idx1 = obj_type_DiffFindOptions_check(L,1);
	field_idx1 = this_idx1->version;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: flags */
static int DiffFindOptions__flags__meth(lua_State *L) {
  DiffFindOptions * this_idx1;
  unsigned int field_idx1 = 0;
  this_idx1 = obj_type_DiffFindOptions_check(L,1);
	field_idx1 = this_idx1->flags;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: rename_threshold */
static int DiffFindOptions__rename_threshold__meth(lua_State *L) {
  DiffFindOptions * this_idx1;
  uint16_t field_idx1 = 0;
  this_idx1 = obj_type_DiffFindOptions_check(L,1);
	field_idx1 = this_idx1->rename_threshold;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: rename_from_rewrite_threshold */
static int DiffFindOptions__rename_from_rewrite_threshold__meth(lua_State *L) {
  DiffFindOptions * this_idx1;
  uint16_t field_idx1 = 0;
  this_idx1 = obj_type_DiffFindOptions_check(L,1);
	field_idx1 = this_idx1->rename_from_rewrite_threshold;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: copy_threshold */
static int DiffFindOptions__copy_threshold__meth(lua_State *L) {
  DiffFindOptions * this_idx1;
  uint16_t field_idx1 = 0;
  this_idx1 = obj_type_DiffFindOptions_check(L,1);
	field_idx1 = this_idx1->copy_threshold;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: break_rewrite_threshold */
static int DiffFindOptions__break_rewrite_threshold__meth(lua_State *L) {
  DiffFindOptions * this_idx1;
  uint16_t field_idx1 = 0;
  this_idx1 = obj_type_DiffFindOptions_check(L,1);
	field_idx1 = this_idx1->break_rewrite_threshold;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: rename_limit */
static int DiffFindOptions__rename_limit__meth(lua_State *L) {
  DiffFindOptions * this_idx1;
  size_t field_idx1 = 0;
  this_idx1 = obj_type_DiffFindOptions_check(L,1);
	field_idx1 = this_idx1->rename_limit;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: init */
static int DiffOptions__init__meth(lua_State *L) {
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  DiffOptions * this_idx1;
	this_idx1 = calloc(1, sizeof(DiffOptions));
	git_diff_options_init(this_idx1, GIT_DIFF_OPTIONS_VERSION);

  obj_type_DiffOptions_push(L, this_idx1, this_flags_idx1);
  return 1;
}

/* method: _priv */
static int DiffOptions__delete__meth(lua_State *L) {
  int this_flags_idx1 = 0;
  DiffOptions * this_idx1;
  this_idx1 = obj_type_DiffOptions_delete(L,1,&(this_flags_idx1));
  if(!(this_flags_idx1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
	if(this_idx1->old_prefix != NULL) {
		free((void *)this_idx1->old_prefix);
		this_idx1->old_prefix = NULL;
	}
	if(this_idx1->new_prefix != NULL) {
		free((void *)this_idx1->new_prefix);
		this_idx1->new_prefix = NULL;
	}
	free(this_idx1);

  return 0;
}

/* method: set_flags */
static int DiffOptions__set_flags__meth(lua_State *L) {
  DiffOptions * this_idx1;
  unsigned int flags_idx2;
  this_idx1 = obj_type_DiffOptions_check(L,1);
  flags_idx2 = luaL_checkinteger(L,2);
		this_idx1->flags = flags_idx2;

  return 0;
}

/* method: set_ignore_submodules */
static int DiffOptions__set_ignore_submodules__meth(lua_State *L) {
  DiffOptions * this_idx1;
  unsigned int ignore_submodules_idx2;
  this_idx1 = obj_type_DiffOptions_check(L,1);
  ignore_submodules_idx2 = luaL_checkinteger(L,2);
		this_idx1->ignore_submodules = ignore_submodules_idx2;

  return 0;
}

/* method: pathspec */
static int DiffOptions__pathspec__meth(lua_State *L) {
  DiffOptions * this_idx1;
  StrArray pathspec_idx1_store;
  StrArray * pathspec_idx1 = &(pathspec_idx1_store);
  this_idx1 = obj_type_DiffOptions_check(L,1);
		pathspec_idx1 = &this_idx1->pathspec;

  obj_type_StrArray_push(L, pathspec_idx1);
  return 1;
}

/* method: set_pathspec */
static int DiffOptions__set_pathspec__meth(lua_State *L) {
  DiffOptions * this_idx1;
  StrArray * pathspec_idx2;
  this_idx1 = obj_type_DiffOptions_check(L,1);
  pathspec_idx2 = obj_type_StrArray_check(L,2);
		this_idx1->pathspec.strings = pathspec_idx2->strings;
		this_idx1->pathspec.count = pathspec_idx2->count;

  return 0;
}

/* method: set_context_lines */
static int DiffOptions__set_context_lines__meth(lua_State *L) {
  DiffOptions * this_idx1;
  uint32_t context_lines_idx2;
  this_idx1 = obj_type_DiffOptions_check(L,1);
  context_lines_idx2 = luaL_checkinteger(L,2);
		this_idx1->context_lines = context_lines_idx2;

  return 0;
}

/* method: set_interhunk_lines */
static int DiffOptions__set_interhunk_lines__meth(lua_State *L) {
  DiffOptions * this_idx1;
  uint32_t interhunk_lines_idx2;
  this_idx1 = obj_type_DiffOptions_check(L,1);
  interhunk_lines_idx2 = luaL_checkinteger(L,2);
		this_idx1->interhunk_lines = interhunk_lines_idx2;

  return 0;
}

/* method: set_oid_type */
static int DiffOptions__set_oid_type__meth(lua_State *L) {
  DiffOptions * this_idx1;
  unsigned int oid_type_idx2;
  this_idx1 = obj_type_DiffOptions_check(L,1);
  oid_type_idx2 = luaL_checkinteger(L,2);
		this_idx1->oid_type = oid_type_idx2;

  return 0;
}

/* method: set_id_abbrev */
static int DiffOptions__set_id_abbrev__meth(lua_State *L) {
  DiffOptions * this_idx1;
  uint16_t id_abbrev_idx2;
  this_idx1 = obj_type_DiffOptions_check(L,1);
  id_abbrev_idx2 = luaL_checkinteger(L,2);
		this_idx1->id_abbrev = id_abbrev_idx2;

  return 0;
}

/* method: set_max_size */
static int DiffOptions__set_max_size__meth(lua_State *L) {
  DiffOptions * this_idx1;
  int64_t max_size_idx2;
  this_idx1 = obj_type_DiffOptions_check(L,1);
  max_size_idx2 = luaL_checkinteger(L,2);
		this_idx1->max_size = max_size_idx2;

  return 0;
}

/* method: set_old_prefix */
static int DiffOptions__set_old_prefix__meth(lua_State *L) {
  DiffOptions * this_idx1;
  size_t old_prefix_len_idx2;
  const char * old_prefix_idx2;
  this_idx1 = obj_type_DiffOptions_check(L,1);
  old_prefix_idx2 = luaL_checklstring(L,2,&(old_prefix_len_idx2));
		this_idx1->old_prefix = old_prefix_idx2;

  return 0;
}

/* method: set_new_prefix */
static int DiffOptions__set_new_prefix__meth(lua_State *L) {
  DiffOptions * this_idx1;
  size_t new_prefix_len_idx2;
  const char * new_prefix_idx2;
  this_idx1 = obj_type_DiffOptions_check(L,1);
  new_prefix_idx2 = luaL_checklstring(L,2,&(new_prefix_len_idx2));
		this_idx1->new_prefix = new_prefix_idx2;

  return 0;
}

/* method: version */
static int DiffOptions__version__meth(lua_State *L) {
  DiffOptions * this_idx1;
  unsigned int field_idx1 = 0;
  this_idx1 = obj_type_DiffOptions_check(L,1);
	field_idx1 = this_idx1->version;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: flags */
static int DiffOptions__flags__meth(lua_State *L) {
  DiffOptions * this_idx1;
  unsigned int field_idx1 = 0;
  this_idx1 = obj_type_DiffOptions_check(L,1);
	field_idx1 = this_idx1->flags;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: ignore_submodules */
static int DiffOptions__ignore_submodules__meth(lua_State *L) {
  DiffOptions * this_idx1;
  unsigned int field_idx1 = 0;
  this_idx1 = obj_type_DiffOptions_check(L,1);
	field_idx1 = this_idx1->ignore_submodules;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: context_lines */
static int DiffOptions__context_lines__meth(lua_State *L) {
  DiffOptions * this_idx1;
  uint32_t field_idx1 = 0;
  this_idx1 = obj_type_DiffOptions_check(L,1);
	field_idx1 = this_idx1->context_lines;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: interhunk_lines */
static int DiffOptions__interhunk_lines__meth(lua_State *L) {
  DiffOptions * this_idx1;
  uint32_t field_idx1 = 0;
  this_idx1 = obj_type_DiffOptions_check(L,1);
	field_idx1 = this_idx1->interhunk_lines;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: oid_type */
static int DiffOptions__oid_type__meth(lua_State *L) {
  DiffOptions * this_idx1;
  unsigned int field_idx1 = 0;
  this_idx1 = obj_type_DiffOptions_check(L,1);
	field_idx1 = this_idx1->oid_type;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: id_abbrev */
static int DiffOptions__id_abbrev__meth(lua_State *L) {
  DiffOptions * this_idx1;
  uint16_t field_idx1 = 0;
  this_idx1 = obj_type_DiffOptions_check(L,1);
	field_idx1 = this_idx1->id_abbrev;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: max_size */
static int DiffOptions__max_size__meth(lua_State *L) {
  DiffOptions * this_idx1;
  int64_t field_idx1 = 0;
  this_idx1 = obj_type_DiffOptions_check(L,1);
	field_idx1 = this_idx1->max_size;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: old_prefix */
static int DiffOptions__old_prefix__meth(lua_State *L) {
  DiffOptions * this_idx1;
  const char * field_idx1 = NULL;
  this_idx1 = obj_type_DiffOptions_check(L,1);
	field_idx1 = this_idx1->old_prefix;

  lua_pushstring(L, field_idx1);
  return 1;
}

/* method: new_prefix */
static int DiffOptions__new_prefix__meth(lua_State *L) {
  DiffOptions * this_idx1;
  const char * field_idx1 = NULL;
  this_idx1 = obj_type_DiffOptions_check(L,1);
	field_idx1 = this_idx1->new_prefix;

  lua_pushstring(L, field_idx1);
  return 1;
}

/* method: get */
static int DiffDelta__get__meth(lua_State *L) {
  Diff * diff_idx1;
  size_t idx_idx2;
  const DiffDelta * rc_git_diff_get_delta_idx1;
  int this_flags_idx2 = OBJ_UDATA_FLAG_OWN;
  DiffDelta * this_idx2;
  diff_idx1 = obj_type_Diff_check(L,1);
  idx_idx2 = luaL_checkinteger(L,2);
  rc_git_diff_get_delta_idx1 = git_diff_get_delta(diff_idx1, idx_idx2);
  obj_type_DiffDelta_push(L, rc_git_diff_get_delta_idx1, 0);
  obj_type_DiffDelta_push(L, this_idx2, this_flags_idx2);
  return 2;
}

/* method: old_file */
static int DiffDelta__old_file__meth(lua_State *L) {
  DiffDelta * this_idx1;
  DiffFile * old_file_idx1;
  this_idx1 = obj_type_DiffDelta_check(L,1);
		old_file_idx1 = &this_idx1->old_file;

  obj_type_DiffFile_push(L, old_file_idx1, 0);
  return 1;
}

/* method: new_file */
static int DiffDelta__new_file__meth(lua_State *L) {
  DiffDelta * this_idx1;
  DiffFile * new_file_idx1;
  this_idx1 = obj_type_DiffDelta_check(L,1);
		new_file_idx1 = &this_idx1->new_file;

  obj_type_DiffFile_push(L, new_file_idx1, 0);
  return 1;
}

/* method: status */
static int DiffDelta__status__meth(lua_State *L) {
  DiffDelta * this_idx1;
  unsigned int field_idx1 = 0;
  this_idx1 = obj_type_DiffDelta_check(L,1);
	field_idx1 = this_idx1->status;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: flags */
static int DiffDelta__flags__meth(lua_State *L) {
  DiffDelta * this_idx1;
  uint32_t field_idx1 = 0;
  this_idx1 = obj_type_DiffDelta_check(L,1);
	field_idx1 = this_idx1->flags;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: similarity */
static int DiffDelta__similarity__meth(lua_State *L) {
  DiffDelta * this_idx1;
  uint16_t field_idx1 = 0;
  this_idx1 = obj_type_DiffDelta_check(L,1);
	field_idx1 = this_idx1->similarity;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: nfiles */
static int DiffDelta__nfiles__meth(lua_State *L) {
  DiffDelta * this_idx1;
  uint16_t field_idx1 = 0;
  this_idx1 = obj_type_DiffDelta_check(L,1);
	field_idx1 = this_idx1->nfiles;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: get */
static int DiffStats__get__meth(lua_State *L) {
  Diff * diff_idx1;
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  DiffStats * this_idx1;
  GitError err_idx2 = GIT_OK;
  diff_idx1 = obj_type_Diff_check(L,1);
  err_idx2 = git_diff_get_stats(&(this_idx1), diff_idx1);
  if(!(GIT_OK != err_idx2)) {
    obj_type_DiffStats_push(L, this_idx1, this_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: insertions */
static int DiffStats__insertions__meth(lua_State *L) {
  DiffStats * this_idx1;
  size_t size_idx1 = 0;
  this_idx1 = obj_type_DiffStats_check(L,1);
  size_idx1 = git_diff_stats_insertions(this_idx1);
  lua_pushinteger(L, size_idx1);
  return 1;
}

/* method: deletions */
static int DiffStats__deletions__meth(lua_State *L) {
  DiffStats * this_idx1;
  size_t size_idx1 = 0;
  this_idx1 = obj_type_DiffStats_check(L,1);
  size_idx1 = git_diff_stats_deletions(this_idx1);
  lua_pushinteger(L, size_idx1);
  return 1;
}

/* method: files_changed */
static int DiffStats__files_changed__meth(lua_State *L) {
  DiffStats * this_idx1;
  size_t size_idx1 = 0;
  this_idx1 = obj_type_DiffStats_check(L,1);
  size_idx1 = git_diff_stats_files_changed(this_idx1);
  lua_pushinteger(L, size_idx1);
  return 1;
}

/* method: _priv */
static int DiffFile__delete__meth(lua_State *L) {
  int this_flags_idx1 = 0;
  DiffFile * this_idx1;
  this_idx1 = obj_type_DiffFile_delete(L,1,&(this_flags_idx1));
  if(!(this_flags_idx1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
	if(this_idx1->path != NULL) {
		free((void *)this_idx1->path);
		this_idx1->path = NULL;
	}
	free(this_idx1);

  return 0;
}

/* method: id */
static int DiffFile__id__meth(lua_State *L) {
  DiffFile * this_idx1;
  OID field_idx1;
  this_idx1 = obj_type_DiffFile_check(L,1);
	field_idx1 = this_idx1->id;

  obj_type_OID_push(L, field_idx1);
  return 1;
}

/* method: path */
static int DiffFile__path__meth(lua_State *L) {
  DiffFile * this_idx1;
  const char * field_idx1 = NULL;
  this_idx1 = obj_type_DiffFile_check(L,1);
	field_idx1 = this_idx1->path;

  lua_pushstring(L, field_idx1);
  return 1;
}

/* method: size */
static int DiffFile__size__meth(lua_State *L) {
  DiffFile * this_idx1;
  uint64_t field_idx1 = 0;
  this_idx1 = obj_type_DiffFile_check(L,1);
	field_idx1 = this_idx1->size;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: flags */
static int DiffFile__flags__meth(lua_State *L) {
  DiffFile * this_idx1;
  uint32_t field_idx1 = 0;
  this_idx1 = obj_type_DiffFile_check(L,1);
	field_idx1 = this_idx1->flags;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: mode */
static int DiffFile__mode__meth(lua_State *L) {
  DiffFile * this_idx1;
  uint16_t field_idx1 = 0;
  this_idx1 = obj_type_DiffFile_check(L,1);
	field_idx1 = this_idx1->mode;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: id_abbrev */
static int DiffFile__id_abbrev__meth(lua_State *L) {
  DiffFile * this_idx1;
  uint16_t field_idx1 = 0;
  this_idx1 = obj_type_DiffFile_check(L,1);
	field_idx1 = this_idx1->id_abbrev;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: index_to_workdir */
static int Diff__index_to_workdir__meth(lua_State *L) {
  Repository * repo_idx1;
  Index * index_idx2;
  DiffOptions * opts_idx3;
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  Diff * this_idx1;
  GitError err_idx2 = GIT_OK;
  repo_idx1 = obj_type_Repository_check(L,1);
  index_idx2 = obj_type_Index_check(L,2);
  opts_idx3 = obj_type_DiffOptions_check(L,3);
  err_idx2 = git_diff_index_to_workdir(&(this_idx1), repo_idx1, index_idx2, opts_idx3);
  if(!(GIT_OK != err_idx2)) {
    obj_type_Diff_push(L, this_idx1, this_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: tree_to_index */
static int Diff__tree_to_index__meth(lua_State *L) {
  Repository * repo_idx1;
  Tree * tree_idx2;
  Index * index_idx3;
  DiffOptions * opts_idx4;
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  Diff * this_idx1;
  GitError err_idx2 = GIT_OK;
  repo_idx1 = obj_type_Repository_check(L,1);
  tree_idx2 = obj_type_Tree_check(L,2);
  index_idx3 = obj_type_Index_check(L,3);
  opts_idx4 = obj_type_DiffOptions_check(L,4);
  err_idx2 = git_diff_tree_to_index(&(this_idx1), repo_idx1, tree_idx2, index_idx3, opts_idx4);
  if(!(GIT_OK != err_idx2)) {
    obj_type_Diff_push(L, this_idx1, this_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: tree_to_workdir_with_index */
static int Diff__tree_to_workdir_with_index__meth(lua_State *L) {
  Repository * repo_idx1;
  Tree * tree_idx2;
  DiffOptions * opts_idx3;
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  Diff * this_idx1;
  GitError err_idx2 = GIT_OK;
  repo_idx1 = obj_type_Repository_check(L,1);
  tree_idx2 = obj_type_Tree_check(L,2);
  opts_idx3 = obj_type_DiffOptions_check(L,3);
  err_idx2 = git_diff_tree_to_workdir_with_index(&(this_idx1), repo_idx1, tree_idx2, opts_idx3);
  if(!(GIT_OK != err_idx2)) {
    obj_type_Diff_push(L, this_idx1, this_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: tree_to_tree */
static int Diff__tree_to_tree__meth(lua_State *L) {
  Repository * repo_idx1;
  Tree * tree_idx2;
  DiffOptions * opts_idx3;
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  Diff * this_idx1;
  GitError err_idx2 = GIT_OK;
  repo_idx1 = obj_type_Repository_check(L,1);
  tree_idx2 = obj_type_Tree_check(L,2);
  opts_idx3 = obj_type_DiffOptions_check(L,3);
  err_idx2 = git_diff_tree_to_tree(&(this_idx1), repo_idx1, tree_idx2, tree_idx2, opts_idx3);
  if(!(GIT_OK != err_idx2)) {
    obj_type_Diff_push(L, this_idx1, this_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: _priv */
static int Diff__delete__meth(lua_State *L) {
  int this_flags_idx1 = 0;
  Diff * this_idx1;
  this_idx1 = obj_type_Diff_delete(L,1,&(this_flags_idx1));
  if(!(this_flags_idx1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
  git_diff_free(this_idx1);
  return 0;
}

/* method: num */
static int Diff__num__meth(lua_State *L) {
  Diff * this_idx1;
  size_t count_idx1 = 0;
  this_idx1 = obj_type_Diff_check(L,1);
  count_idx1 = git_diff_num_deltas(this_idx1);
  lua_pushinteger(L, count_idx1);
  return 1;
}

/* method: find_similar */
static int Diff__find_similar__meth(lua_State *L) {
  Diff * this_idx1;
  DiffFindOptions * opts_idx2;
  GitError err_idx1 = GIT_OK;
  this_idx1 = obj_type_Diff_check(L,1);
  opts_idx2 = obj_type_DiffFindOptions_check(L,2);
  err_idx1 = git_diff_find_similar(this_idx1, opts_idx2);
  /* check for error. */
  if((GIT_OK != err_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, err_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: init */
static int StatusOptions__init__meth(lua_State *L) {
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  StatusOptions * this_idx1;
	this_idx1 = calloc(1, sizeof(StatusOptions));
	git_status_options_init(this_idx1, GIT_STATUS_OPTIONS_VERSION);

  obj_type_StatusOptions_push(L, this_idx1, this_flags_idx1);
  return 1;
}

/* method: _priv */
static int StatusOptions__delete__meth(lua_State *L) {
  int this_flags_idx1 = 0;
  StatusOptions * this_idx1;
  this_idx1 = obj_type_StatusOptions_delete(L,1,&(this_flags_idx1));
  if(!(this_flags_idx1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
	free(this_idx1);

  return 0;
}

/* method: set_show */
static int StatusOptions__set_show__meth(lua_State *L) {
  StatusOptions * this_idx1;
  unsigned int show_idx2;
  this_idx1 = obj_type_StatusOptions_check(L,1);
  show_idx2 = luaL_checkinteger(L,2);
		this_idx1->show = show_idx2;

  return 0;
}

/* method: set_flags */
static int StatusOptions__set_flags__meth(lua_State *L) {
  StatusOptions * this_idx1;
  unsigned int flags_idx2;
  this_idx1 = obj_type_StatusOptions_check(L,1);
  flags_idx2 = luaL_checkinteger(L,2);
		this_idx1->flags = flags_idx2;

  return 0;
}

/* method: pathspec */
static int StatusOptions__pathspec__meth(lua_State *L) {
  StatusOptions * this_idx1;
  StrArray pathspec_idx1_store;
  StrArray * pathspec_idx1 = &(pathspec_idx1_store);
  this_idx1 = obj_type_StatusOptions_check(L,1);
		pathspec_idx1 = &this_idx1->pathspec;

  obj_type_StrArray_push(L, pathspec_idx1);
  return 1;
}

/* method: set_pathspec */
static int StatusOptions__set_pathspec__meth(lua_State *L) {
  StatusOptions * this_idx1;
  StrArray * pathspec_idx2;
  this_idx1 = obj_type_StatusOptions_check(L,1);
  pathspec_idx2 = obj_type_StrArray_check(L,2);
		this_idx1->pathspec.strings = pathspec_idx2->strings;
		this_idx1->pathspec.count = pathspec_idx2->count;

  return 0;
}

/* method: set_baseline */
static int StatusOptions__set_baseline__meth(lua_State *L) {
  StatusOptions * this_idx1;
  Tree * baseline_idx2;
  this_idx1 = obj_type_StatusOptions_check(L,1);
  baseline_idx2 = obj_type_Tree_check(L,2);
		this_idx1->baseline = baseline_idx2;

  return 0;
}

/* method: set_rename_threshold */
static int StatusOptions__set_rename_threshold__meth(lua_State *L) {
  StatusOptions * this_idx1;
  uint16_t rename_threshold_idx2;
  this_idx1 = obj_type_StatusOptions_check(L,1);
  rename_threshold_idx2 = luaL_checkinteger(L,2);
		this_idx1->rename_threshold = rename_threshold_idx2;

  return 0;
}

/* method: version */
static int StatusOptions__version__meth(lua_State *L) {
  StatusOptions * this_idx1;
  unsigned int field_idx1 = 0;
  this_idx1 = obj_type_StatusOptions_check(L,1);
	field_idx1 = this_idx1->version;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: show */
static int StatusOptions__show__meth(lua_State *L) {
  StatusOptions * this_idx1;
  unsigned int field_idx1 = 0;
  this_idx1 = obj_type_StatusOptions_check(L,1);
	field_idx1 = this_idx1->show;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: flags */
static int StatusOptions__flags__meth(lua_State *L) {
  StatusOptions * this_idx1;
  unsigned int field_idx1 = 0;
  this_idx1 = obj_type_StatusOptions_check(L,1);
	field_idx1 = this_idx1->flags;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: baseline */
static int StatusOptions__baseline__meth(lua_State *L) {
  StatusOptions * this_idx1;
  Tree * field_idx1;
  this_idx1 = obj_type_StatusOptions_check(L,1);
	field_idx1 = this_idx1->baseline;

  obj_type_Tree_push(L, field_idx1, 0);
  return 1;
}

/* method: rename_threshold */
static int StatusOptions__rename_threshold__meth(lua_State *L) {
  StatusOptions * this_idx1;
  uint16_t field_idx1 = 0;
  this_idx1 = obj_type_StatusOptions_check(L,1);
	field_idx1 = this_idx1->rename_threshold;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: new */
static int StatusList__new__meth(lua_State *L) {
  Repository * repo_idx1;
  const StatusOptions * opts_idx2;
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  StatusList * this_idx1;
  GitError err_idx2 = GIT_OK;
  repo_idx1 = obj_type_Repository_check(L,1);
  opts_idx2 = obj_type_StatusOptions_check(L,2);
  err_idx2 = git_status_list_new(&(this_idx1), repo_idx1, opts_idx2);
  if(!(GIT_OK != err_idx2)) {
    obj_type_StatusList_push(L, this_idx1, this_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: _priv */
static int StatusList__delete__meth(lua_State *L) {
  int this_flags_idx1 = 0;
  StatusList * this_idx1;
  this_idx1 = obj_type_StatusList_delete(L,1,&(this_flags_idx1));
  if(!(this_flags_idx1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
  git_status_list_free(this_idx1);
  return 0;
}

/* method: entrycount */
static int StatusList__entrycount__meth(lua_State *L) {
  StatusList * this_idx1;
  size_t count_idx1 = 0;
  this_idx1 = obj_type_StatusList_check(L,1);
  count_idx1 = git_status_list_entrycount(this_idx1);
  lua_pushinteger(L, count_idx1);
  return 1;
}

/* method: byindex */
static int StatusEntry__byindex__meth(lua_State *L) {
  StatusList * statuslist_idx1;
  size_t idx_idx2;
  StatusEntry * rc_git_status_byindex_idx1;
  int this_flags_idx2 = OBJ_UDATA_FLAG_OWN;
  StatusEntry * this_idx2;
  statuslist_idx1 = obj_type_StatusList_check(L,1);
  idx_idx2 = luaL_checkinteger(L,2);
  rc_git_status_byindex_idx1 = git_status_byindex(statuslist_idx1, idx_idx2);
  obj_type_StatusEntry_push(L, rc_git_status_byindex_idx1, 0);
  obj_type_StatusEntry_push(L, this_idx2, this_flags_idx2);
  return 2;
}

/* method: status */
static int StatusEntry__status__meth(lua_State *L) {
  StatusEntry * this_idx1;
  unsigned int field_idx1 = 0;
  this_idx1 = obj_type_StatusEntry_check(L,1);
	field_idx1 = this_idx1->status;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: head_to_index */
static int StatusEntry__head_to_index__meth(lua_State *L) {
  StatusEntry * this_idx1;
  DiffDelta * field_idx1;
  this_idx1 = obj_type_StatusEntry_check(L,1);
	field_idx1 = this_idx1->head_to_index;

  obj_type_DiffDelta_push(L, field_idx1, 0);
  return 1;
}

/* method: index_to_workdir */
static int StatusEntry__index_to_workdir__meth(lua_State *L) {
  StatusEntry * this_idx1;
  DiffDelta * field_idx1;
  this_idx1 = obj_type_StatusEntry_check(L,1);
	field_idx1 = this_idx1->index_to_workdir;

  obj_type_DiffDelta_push(L, field_idx1, 0);
  return 1;
}

/* method: new */
static int StrArray__new__meth(lua_State *L) {
  size_t n_idx1;
  StrArray this_idx1_store;
  StrArray * this_idx1 = &(this_idx1_store);
  n_idx1 = luaL_checkinteger(L,1);
	StrArray array;
	array.strings = (char **)calloc(n_idx1, sizeof(char *));
	array.count = n_idx1;
	this_idx1 = &array;

  obj_type_StrArray_push(L, this_idx1);
  return 1;
}

/* method: free */
static int StrArray__free__meth(lua_State *L) {
  StrArray * this_idx1;
  this_idx1 = obj_type_StrArray_delete(L,1);
	if(this_idx1->strings != 0) {
		git_strarray_dispose(this_idx1);
		this_idx1->strings = NULL;
	}

  return 0;
}

/* method: str */
static int StrArray__str__meth(lua_State *L) {
  StrArray * this_idx1;
  size_t n_idx2;
  const char * str_idx1 = NULL;
  this_idx1 = obj_type_StrArray_check(L,1);
  n_idx2 = luaL_checkinteger(L,2);
	if(n_idx2 < this_idx1->count) {
		str_idx1 = this_idx1->strings[n_idx2];
	}

  lua_pushstring(L, str_idx1);
  return 1;
}

/* method: set_str */
static int StrArray__set_str__meth(lua_State *L) {
  StrArray * this_idx1;
  size_t n_idx2;
  size_t str_len_idx3;
  const char * str_idx3;
  this_idx1 = obj_type_StrArray_check(L,1);
  n_idx2 = luaL_checkinteger(L,2);
  str_idx3 = luaL_checklstring(L,3,&(str_len_idx3));
	if(n_idx2 < this_idx1->count) {
		if(this_idx1->strings[n_idx2] != NULL) {
			free(this_idx1->strings[n_idx2]);
		}
		this_idx1->strings[n_idx2] = strdup(str_idx3);
	}

  return 0;
}

/* method: get_array */
static int StrArray__get_array__meth(lua_State *L) {
  StrArray * this_idx1;
	size_t n;

  this_idx1 = obj_type_StrArray_check(L,1);
	lua_createtable(L, this_idx1->count, 0);
	for(n = 0; n < this_idx1->count; n++) {
		lua_pushstring(L, this_idx1->strings[n]);
		lua_rawseti(L, -2, n+1);
	}

  return 1;
}

/* method: count */
static int StrArray__count__meth(lua_State *L) {
  StrArray * this_idx1;
  size_t field_idx1 = 0;
  this_idx1 = obj_type_StrArray_check(L,1);
	field_idx1 = this_idx1->count;

  lua_pushinteger(L, field_idx1);
  return 1;
}

static void error_code__GitError__push(lua_State *L, GitError err) {
  const char *err_str = NULL;
	const git_error *giterr;
	switch(err) {
	case GIT_ERROR:
		giterr = giterr_last();
		err_str = giterr->message;
		break;
	case GIT_ENOTFOUND:
		err_str = "Requested object could not be found";
		break;
	case GIT_EEXISTS:
		err_str = "Object exists preventing operation";
		break;
	case GIT_EAMBIGUOUS:
		err_str = "More than one object matches";
		break;
	case GIT_EBUFS:
		err_str = "Output buffer too short to hold data";
		break;
	case GIT_EUSER:
		err_str = "a special error that is never generated by libgit2 code";
		break;
	case GIT_EBAREREPO:
		err_str = "Operation not allowed on bare repository";
		break;
	case GIT_EUNBORNBRANCH:
		err_str = "HEAD refers to branch with no commits";
		break;
	case GIT_EUNMERGED:
		err_str = "Merge in progress prevented operation";
		break;
	case GIT_ENONFASTFORWARD:
		err_str = "Reference was not fast-forwardable";
		break;
	case GIT_EINVALIDSPEC:
		err_str = "Name/ref spec was not in a valid format";
		break;
	case GIT_ECONFLICT:
		err_str = "Checkout conflicts prevented operation";
		break;
	case GIT_ELOCKED:
		err_str = "Lock file prevented operation";
		break;
	case GIT_EMODIFIED:
		err_str = "Reference value does not match expected";
		break;
	case GIT_EAUTH:
		err_str = "Authentication error";
		break;
	case GIT_ECERTIFICATE:
		err_str = "Server certificate is invalid";
		break;
	case GIT_EAPPLIED:
		err_str = "Patch/merge has already been applied";
		break;
	case GIT_EPEEL:
		err_str = "The requested peel operation is not possible";
		break;
	case GIT_EEOF:
		err_str = "Unexpected EOF";
		break;
	case GIT_EINVALID:
		err_str = "Invalid operation or input";
		break;
	case GIT_EUNCOMMITTED:
		err_str = "Uncommitted changes in index prevented operation";
		break;
	case GIT_EDIRECTORY:
		err_str = "The operation is not valid for a directory";
		break;
	case GIT_EMERGECONFLICT:
		err_str = "A merge conflict exists and cannot continue";
		break;
	case GIT_PASSTHROUGH:
		err_str = "A user-configured callback refused to act";
		break;
	case GIT_ITEROVER:
		err_str = "Signals end of iteration with iterator";
		break;
	case GIT_RETRY:
		err_str = "Internal only";
		break;
	case GIT_EMISMATCH:
		err_str = "Hashsum mismatch in object";
		break;
	case GIT_EINDEXDIRTY:
		err_str = "Unsaved changes in the index would be overwritten";
		break;
	case GIT_EAPPLYFAIL:
		err_str = "Patch application failed";
		break;
	case GIT_EOWNER:
		err_str = "The object is not owned by the current user";
		break;
	case GIT_TIMEOUT:
		err_str = "The operation timed out";
		break;
	case GIT_OK:
	default:
		break;
	}

	if(err_str) {
		lua_pushstring(L, err_str);
	} else {
		lua_pushnil(L);
	}
}

/* method: open */
static int Repository__open__meth(lua_State *L) {
  size_t path_len_idx1;
  const char * path_idx1;
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  Repository * this_idx1;
  GitError err_idx2 = GIT_OK;
  path_idx1 = luaL_checklstring(L,1,&(path_len_idx1));
  err_idx2 = git_repository_open(&(this_idx1), path_idx1);
  if(!(GIT_OK != err_idx2)) {
    obj_type_Repository_push(L, this_idx1, this_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: init */
static int Repository__init__meth(lua_State *L) {
  size_t path_len_idx1;
  const char * path_idx1;
  unsigned int is_bare_idx2;
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  Repository * this_idx1;
  GitError err_idx2 = GIT_OK;
  path_idx1 = luaL_checklstring(L,1,&(path_len_idx1));
  is_bare_idx2 = luaL_checkinteger(L,2);
  err_idx2 = git_repository_init(&(this_idx1), path_idx1, is_bare_idx2);
  if(!(GIT_OK != err_idx2)) {
    obj_type_Repository_push(L, this_idx1, this_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: _priv */
static int Repository__delete__meth(lua_State *L) {
  int this_flags_idx1 = 0;
  Repository * this_idx1;
  this_idx1 = obj_type_Repository_delete(L,1,&(this_flags_idx1));
  if(!(this_flags_idx1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
  git_repository_free(this_idx1);
  return 0;
}

/* method: head */
static int Repository__head__meth(lua_State *L) {
  Repository * this_idx1;
  int head_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  Reference * head_idx1;
  GitError err_idx2 = GIT_OK;
  this_idx1 = obj_type_Repository_check(L,1);
  err_idx2 = git_repository_head(&(head_idx1), this_idx1);
  if(!(GIT_OK != err_idx2)) {
    obj_type_Reference_push(L, head_idx1, head_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: head_detached */
static int Repository__head_detached__meth(lua_State *L) {
  Repository * this_idx1;
  bool rc_git_repository_head_detached_idx1 = 0;
  this_idx1 = obj_type_Repository_check(L,1);
  rc_git_repository_head_detached_idx1 = git_repository_head_detached(this_idx1);
  lua_pushboolean(L, rc_git_repository_head_detached_idx1);
  return 1;
}

/* method: set_head_detached */
static int Repository__set_head_detached__meth(lua_State *L) {
  Repository * this_idx1;
  OID * committish_idx2;
  GitError err_idx1 = GIT_OK;
  this_idx1 = obj_type_Repository_check(L,1);
  committish_idx2 = lua_touserdata(L,2);
  err_idx1 = git_repository_set_head_detached(this_idx1, committish_idx2);
  /* check for error. */
  if((GIT_OK != err_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, err_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: head_unborn */
static int Repository__head_unborn__meth(lua_State *L) {
  Repository * this_idx1;
  bool rc_git_repository_head_unborn_idx1 = 0;
  this_idx1 = obj_type_Repository_check(L,1);
  rc_git_repository_head_unborn_idx1 = git_repository_head_unborn(this_idx1);
  lua_pushboolean(L, rc_git_repository_head_unborn_idx1);
  return 1;
}

/* method: is_empty */
static int Repository__is_empty__meth(lua_State *L) {
  Repository * this_idx1;
  bool rc_git_repository_is_empty_idx1 = 0;
  this_idx1 = obj_type_Repository_check(L,1);
  rc_git_repository_is_empty_idx1 = git_repository_is_empty(this_idx1);
  lua_pushboolean(L, rc_git_repository_is_empty_idx1);
  return 1;
}

/* method: is_bare */
static int Repository__is_bare__meth(lua_State *L) {
  Repository * this_idx1;
  bool rc_git_repository_is_bare_idx1 = 0;
  this_idx1 = obj_type_Repository_check(L,1);
  rc_git_repository_is_bare_idx1 = git_repository_is_bare(this_idx1);
  lua_pushboolean(L, rc_git_repository_is_bare_idx1);
  return 1;
}

/* method: path */
static int Repository__path__meth(lua_State *L) {
  Repository * this_idx1;
  const char * rc_git_repository_path_idx1 = NULL;
  this_idx1 = obj_type_Repository_check(L,1);
  rc_git_repository_path_idx1 = git_repository_path(this_idx1);
  lua_pushstring(L, rc_git_repository_path_idx1);
  return 1;
}

/* method: workdir */
static int Repository__workdir__meth(lua_State *L) {
  Repository * this_idx1;
  const char * rc_git_repository_workdir_idx1 = NULL;
  this_idx1 = obj_type_Repository_check(L,1);
  rc_git_repository_workdir_idx1 = git_repository_workdir(this_idx1);
  lua_pushstring(L, rc_git_repository_workdir_idx1);
  return 1;
}

/* method: set_workdir */
static int Repository__set_workdir__meth(lua_State *L) {
  Repository * this_idx1;
  size_t workdir_len_idx2;
  const char * workdir_idx2;
  int update_gitlink_idx3;
  GitError rc_git_repository_set_workdir_idx1 = GIT_OK;
  this_idx1 = obj_type_Repository_check(L,1);
  workdir_idx2 = luaL_checklstring(L,2,&(workdir_len_idx2));
  update_gitlink_idx3 = luaL_checkinteger(L,3);
  rc_git_repository_set_workdir_idx1 = git_repository_set_workdir(this_idx1, workdir_idx2, update_gitlink_idx3);
  /* check for error. */
  if((GIT_OK != rc_git_repository_set_workdir_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, rc_git_repository_set_workdir_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: config */
static int Repository__config__meth(lua_State *L) {
  Repository * this_idx1;
  int config_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  Config * config_idx1;
  GitError err_idx2 = GIT_OK;
  this_idx1 = obj_type_Repository_check(L,1);
  err_idx2 = git_repository_config(&(config_idx1), this_idx1);
  if(!(GIT_OK != err_idx2)) {
    obj_type_Config_push(L, config_idx1, config_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: set_config */
static int Repository__set_config__meth(lua_State *L) {
  Repository * this_idx1;
  Config * config_idx2;
  this_idx1 = obj_type_Repository_check(L,1);
  config_idx2 = obj_type_Config_check(L,2);
  git_repository_set_config(this_idx1, config_idx2);
  return 0;
}

/* method: odb */
static int Repository__odb__meth(lua_State *L) {
  Repository * this_idx1;
  int odb_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  ODB * odb_idx1;
  GitError err_idx2 = GIT_OK;
  this_idx1 = obj_type_Repository_check(L,1);
  err_idx2 = git_repository_odb(&(odb_idx1), this_idx1);
  if(!(GIT_OK != err_idx2)) {
    obj_type_ODB_push(L, odb_idx1, odb_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: set_odb */
static int Repository__set_odb__meth(lua_State *L) {
  Repository * this_idx1;
  ODB * odb_idx2;
  this_idx1 = obj_type_Repository_check(L,1);
  odb_idx2 = obj_type_ODB_check(L,2);
  git_repository_set_odb(this_idx1, odb_idx2);
  return 0;
}

/* method: index */
static int Repository__index__meth(lua_State *L) {
  Repository * this_idx1;
  int index_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  Index * index_idx1;
  GitError err_idx2 = GIT_OK;
  this_idx1 = obj_type_Repository_check(L,1);
  err_idx2 = git_repository_index(&(index_idx1), this_idx1);
  if(!(GIT_OK != err_idx2)) {
    obj_type_Index_push(L, index_idx1, index_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: set_index */
static int Repository__set_index__meth(lua_State *L) {
  Repository * this_idx1;
  Index * index_idx2;
  this_idx1 = obj_type_Repository_check(L,1);
  index_idx2 = obj_type_Index_check(L,2);
  git_repository_set_index(this_idx1, index_idx2);
  return 0;
}

/* method: checkout_tree */
static int Repository__checkout_tree__meth(lua_State *L) {
  Repository * this_idx1;
  const Object * treeish_idx2;
  const CheckoutOptions * opts_idx3;
  GitError err_idx1 = GIT_OK;
  this_idx1 = obj_type_Repository_check(L,1);
  treeish_idx2 = obj_type_Object_check(L,2);
  opts_idx3 = obj_type_CheckoutOptions_check(L,3);
  err_idx1 = git_checkout_tree(this_idx1, treeish_idx2, opts_idx3);
  /* check for error. */
  if((GIT_OK != err_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, err_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: checkout_head */
static int Repository__checkout_head__meth(lua_State *L) {
  Repository * this_idx1;
  const CheckoutOptions * opts_idx2;
  GitError err_idx1 = GIT_OK;
  this_idx1 = obj_type_Repository_check(L,1);
  opts_idx2 = obj_type_CheckoutOptions_check(L,2);
  err_idx1 = git_checkout_head(this_idx1, opts_idx2);
  /* check for error. */
  if((GIT_OK != err_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, err_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: checkout_index */
static int Repository__checkout_index__meth(lua_State *L) {
  Repository * this_idx1;
  Index * index_idx2;
  const CheckoutOptions * opts_idx3;
  GitError err_idx1 = GIT_OK;
  this_idx1 = obj_type_Repository_check(L,1);
  index_idx2 = obj_type_Index_check(L,2);
  opts_idx3 = obj_type_CheckoutOptions_check(L,3);
  err_idx1 = git_checkout_index(this_idx1, index_idx2, opts_idx3);
  /* check for error. */
  if((GIT_OK != err_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, err_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: new */
static int Config__new__meth(lua_State *L) {
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  Config * this_idx1;
  GitError err_idx2 = GIT_OK;
  err_idx2 = git_config_new(&(this_idx1));
  if(!(GIT_OK != err_idx2)) {
    obj_type_Config_push(L, this_idx1, this_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: open */
static int Config__open__meth(lua_State *L) {
  size_t path_len_idx1;
  const char * path_idx1;
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  Config * this_idx1;
  GitError err_idx2 = GIT_OK;
  path_idx1 = luaL_checklstring(L,1,&(path_len_idx1));
  err_idx2 = git_config_open_ondisk(&(this_idx1), path_idx1);
  if(!(GIT_OK != err_idx2)) {
    obj_type_Config_push(L, this_idx1, this_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: free */
static int Config__free__meth(lua_State *L) {
  int this_flags_idx1 = 0;
  Config * this_idx1;
  this_idx1 = obj_type_Config_delete(L,1,&(this_flags_idx1));
  if(!(this_flags_idx1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
  git_config_free(this_idx1);
  return 0;
}

/* method: add_file_ondisk */
static int Config__add_file_ondisk__meth(lua_State *L) {
  Config * this_idx1;
  size_t path_len_idx2;
  const char * path_idx2;
  git_config_level_t level_idx3;
  Repository * repo_idx4;
  int force_idx5;
  GitError err_idx1 = GIT_OK;
  this_idx1 = obj_type_Config_check(L,1);
  path_idx2 = luaL_checklstring(L,2,&(path_len_idx2));
  level_idx3 = luaL_checkinteger(L,3);
  repo_idx4 = obj_type_Repository_check(L,4);
  force_idx5 = luaL_checkinteger(L,5);
  err_idx1 = git_config_add_file_ondisk(this_idx1, path_idx2, level_idx3, repo_idx4, force_idx5);
  /* check for error. */
  if((GIT_OK != err_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, err_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: get_int32 */
static int Config__get_int32__meth(lua_State *L) {
  Config * this_idx1;
  size_t name_len_idx2;
  const char * name_idx2;
  int32_t out_idx1 = 0;
  GitError err_idx2 = GIT_OK;
  this_idx1 = obj_type_Config_check(L,1);
  name_idx2 = luaL_checklstring(L,2,&(name_len_idx2));
  err_idx2 = git_config_get_int32(&(out_idx1), this_idx1, name_idx2);
  if(!(GIT_OK != err_idx2)) {
    lua_pushinteger(L, out_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: set_int32 */
static int Config__set_int32__meth(lua_State *L) {
  Config * this_idx1;
  size_t name_len_idx2;
  const char * name_idx2;
  int32_t value_idx3;
  GitError err_idx1 = GIT_OK;
  this_idx1 = obj_type_Config_check(L,1);
  name_idx2 = luaL_checklstring(L,2,&(name_len_idx2));
  value_idx3 = luaL_checkinteger(L,3);
  err_idx1 = git_config_set_int32(this_idx1, name_idx2, value_idx3);
  /* check for error. */
  if((GIT_OK != err_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, err_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: get_int64 */
static int Config__get_int64__meth(lua_State *L) {
  Config * this_idx1;
  size_t name_len_idx2;
  const char * name_idx2;
  int64_t out_idx1 = 0;
  GitError err_idx2 = GIT_OK;
  this_idx1 = obj_type_Config_check(L,1);
  name_idx2 = luaL_checklstring(L,2,&(name_len_idx2));
  err_idx2 = git_config_get_int64(&(out_idx1), this_idx1, name_idx2);
  if(!(GIT_OK != err_idx2)) {
    lua_pushinteger(L, out_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: set_int64 */
static int Config__set_int64__meth(lua_State *L) {
  Config * this_idx1;
  size_t name_len_idx2;
  const char * name_idx2;
  int64_t value_idx3;
  GitError err_idx1 = GIT_OK;
  this_idx1 = obj_type_Config_check(L,1);
  name_idx2 = luaL_checklstring(L,2,&(name_len_idx2));
  value_idx3 = luaL_checkinteger(L,3);
  err_idx1 = git_config_set_int64(this_idx1, name_idx2, value_idx3);
  /* check for error. */
  if((GIT_OK != err_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, err_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: get_bool */
static int Config__get_bool__meth(lua_State *L) {
  Config * this_idx1;
  size_t name_len_idx2;
  const char * name_idx2;
  bool out_idx1 = 0;
  GitError err_idx2 = GIT_OK;
  int out_int_idx3 = 0;
  this_idx1 = obj_type_Config_check(L,1);
  name_idx2 = luaL_checklstring(L,2,&(name_len_idx2));
  err_idx2 = git_config_get_bool(&(out_int_idx3), this_idx1, name_idx2);
	out_idx1 = out_int_idx3;

  if(!(GIT_OK != err_idx2)) {
    lua_pushboolean(L, out_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: set_bool */
static int Config__set_bool__meth(lua_State *L) {
  Config * this_idx1;
  bool value_idx2;
  size_t name_len_idx3;
  const char * name_idx3;
  GitError err_idx1 = GIT_OK;
  int value_int_idx2 = 0;
  this_idx1 = obj_type_Config_check(L,1);
  value_idx2 = lua_toboolean(L,2);
  name_idx3 = luaL_checklstring(L,3,&(name_len_idx3));
	value_int_idx2 = value_idx2 ? 1 : 0;

  err_idx1 = git_config_set_bool(this_idx1, name_idx3, value_int_idx2);
  /* check for error. */
  if((GIT_OK != err_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, err_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: get_string */
static int Config__get_string__meth(lua_State *L) {
  Config * this_idx1;
  size_t name_len_idx2;
  const char * name_idx2;
  const char * out_idx1 = NULL;
  GitError err_idx2 = GIT_OK;
  this_idx1 = obj_type_Config_check(L,1);
  name_idx2 = luaL_checklstring(L,2,&(name_len_idx2));
  err_idx2 = git_config_get_string(&(out_idx1), this_idx1, name_idx2);
  if(!(GIT_OK != err_idx2)) {
    lua_pushstring(L, out_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: set_string */
static int Config__set_string__meth(lua_State *L) {
  Config * this_idx1;
  size_t name_len_idx2;
  const char * name_idx2;
  size_t value_len_idx3;
  const char * value_idx3;
  GitError err_idx1 = GIT_OK;
  this_idx1 = obj_type_Config_check(L,1);
  name_idx2 = luaL_checklstring(L,2,&(name_len_idx2));
  value_idx3 = luaL_checklstring(L,3,&(value_len_idx3));
  err_idx1 = git_config_set_string(this_idx1, name_idx2, value_idx3);
  /* check for error. */
  if((GIT_OK != err_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, err_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: delete_entry */
static int Config__delete_entry__meth(lua_State *L) {
  Config * this_idx1;
  size_t name_len_idx2;
  const char * name_idx2;
  GitError err_idx1 = GIT_OK;
  this_idx1 = obj_type_Config_check(L,1);
  name_idx2 = luaL_checklstring(L,2,&(name_len_idx2));
  err_idx1 = git_config_delete_entry(this_idx1, name_idx2);
  /* check for error. */
  if((GIT_OK != err_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, err_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: delete_multivar */
static int Config__delete_multivar__meth(lua_State *L) {
  Config * this_idx1;
  size_t name_len_idx2;
  const char * name_idx2;
  size_t regexp_len_idx3;
  const char * regexp_idx3;
  GitError err_idx1 = GIT_OK;
  this_idx1 = obj_type_Config_check(L,1);
  name_idx2 = luaL_checklstring(L,2,&(name_len_idx2));
  regexp_idx3 = luaL_checklstring(L,3,&(regexp_len_idx3));
  err_idx1 = git_config_delete_multivar(this_idx1, name_idx2, regexp_idx3);
  /* check for error. */
  if((GIT_OK != err_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, err_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: free */
static int OdbObject__free__meth(lua_State *L) {
  int this_flags_idx1 = 0;
  OdbObject * this_idx1;
  this_idx1 = obj_type_OdbObject_delete(L,1,&(this_flags_idx1));
  if(!(this_flags_idx1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
  git_odb_object_free(this_idx1);
  return 0;
}

/* method: id */
static int OdbObject__id__meth(lua_State *L) {
  OdbObject * this_idx1;
  OID id_idx1;
  this_idx1 = obj_type_OdbObject_check(L,1);
  id_idx1 = *git_odb_object_id(this_idx1);
  obj_type_OID_push(L, id_idx1);
  return 1;
}

/* method: data */
static int OdbObject__data__meth(lua_State *L) {
  OdbObject * this_idx1;
  size_t data_len_idx1 = 0;
  const char * data_idx1 = NULL;
  this_idx1 = obj_type_OdbObject_check(L,1);
  data_idx1 = git_odb_object_data(this_idx1);
  data_len_idx1 = git_odb_object_size(this_idx1);
  if(data_idx1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, data_idx1,data_len_idx1);
  return 1;
}

/* method: size */
static int OdbObject__size__meth(lua_State *L) {
  OdbObject * this_idx1;
  size_t rc_git_odb_object_size_idx1 = 0;
  this_idx1 = obj_type_OdbObject_check(L,1);
  rc_git_odb_object_size_idx1 = git_odb_object_size(this_idx1);
  lua_pushinteger(L, rc_git_odb_object_size_idx1);
  return 1;
}

/* method: type */
static int OdbObject__type__meth(lua_State *L) {
  OdbObject * this_idx1;
  git_otype otype_idx1;
  const char * type_idx2 = NULL;
  this_idx1 = obj_type_OdbObject_check(L,1);
  otype_idx1 = git_odb_object_type(this_idx1);
  type_idx2 = git_object_type2string(otype_idx1);
  lua_pushstring(L, type_idx2);
  return 1;
}

/* method: hex */
static int OID__hex__meth(lua_State *L) {
  size_t str_len_idx1;
  const char * str_idx1;
  OID this_idx1;
  GitError err_idx2 = GIT_OK;
  str_idx1 = luaL_checklstring(L,1,&(str_len_idx1));
  err_idx2 = git_oid_fromstrn(&(this_idx1), str_idx1, str_len_idx1);
  if(!(GIT_OK != err_idx2)) {
    obj_type_OID_push(L, this_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: raw */
static int OID__raw__meth(lua_State *L) {
  size_t raw_len_idx1;
  const unsigned char * raw_idx1;
  OID this_idx1;
  raw_idx1 = (unsigned char *)luaL_checklstring(L,1,&(raw_len_idx1));
		if(raw_len_idx1 < GIT_OID_RAWSZ) {
			lua_pushnil(L);
			lua_pushliteral(L, "Invalid RAW OID");
			return 2;
		}
		
  git_oid_fromraw(&(this_idx1), raw_idx1);
  obj_type_OID_push(L, this_idx1);
  return 1;
}

/* method: pathfmt */
static int OID__pathfmt__meth(lua_State *L) {
  OID this_idx1;
  const char * ret_idx1 = NULL;
  this_idx1 = obj_type_OID_check(L,1);
	char buf[GIT_OID_HEXSZ+1+1];
  git_oid_pathfmt(buf, &(this_idx1));
	buf[GIT_OID_HEXSZ] = 0;
	ret_idx1 = buf;

  lua_pushstring(L, ret_idx1);
  return 1;
}

/* method: fmt */
static int OID__fmt__meth(lua_State *L) {
  OID this_idx1;
  const char * ret_idx1 = NULL;
  this_idx1 = obj_type_OID_check(L,1);
	char buf[GIT_OID_HEXSZ+1];
  git_oid_fmt(buf, &(this_idx1));
	buf[GIT_OID_HEXSZ] = 0;
	ret_idx1 = buf;

  lua_pushstring(L, ret_idx1);
  return 1;
}

/* method: __tostring */
static int OID____str____meth(lua_State *L) {
  OID this_idx1;
  const char * ret_idx1 = NULL;
  this_idx1 = obj_type_OID_check(L,1);
	char buf[GIT_OID_HEXSZ+1];
  git_oid_fmt(buf, &(this_idx1));
	buf[GIT_OID_HEXSZ] = 0;
	ret_idx1 = buf;

  lua_pushstring(L, ret_idx1);
  return 1;
}

/* method: __eq */
static int OID____eq____meth(lua_State *L) {
  OID this_idx1;
  OID id_idx2;
  int ret_idx1 = 0;
  this_idx1 = obj_type_OID_check(L,1);
  id_idx2 = obj_type_OID_check(L,2);
  ret_idx1 = git_oid_cmp(&(this_idx1), &(id_idx2));
  lua_pushinteger(L, ret_idx1);
  return 1;
}

/* method: new */
static int OID_Shorten__new__meth(lua_State *L) {
  size_t min_length_idx1;
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  OID_Shorten * this_idx1;
  min_length_idx1 = luaL_checkinteger(L,1);
  this_idx1 = git_oid_shorten_new(min_length_idx1);
  obj_type_OID_Shorten_push(L, this_idx1, this_flags_idx1);
  return 1;
}

/* method: _priv */
static int OID_Shorten__delete__meth(lua_State *L) {
  int this_flags_idx1 = 0;
  OID_Shorten * this_idx1;
  this_idx1 = obj_type_OID_Shorten_delete(L,1,&(this_flags_idx1));
  if(!(this_flags_idx1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
  git_oid_shorten_free(this_idx1);
  return 0;
}

/* method: add */
static int OID_Shorten__add__meth(lua_State *L) {
  OID_Shorten * this_idx1;
  size_t text_oid_len_idx2;
  const char * text_oid_idx2;
  GitError rc_idx1 = GIT_OK;
  this_idx1 = obj_type_OID_Shorten_check(L,1);
  text_oid_idx2 = luaL_checklstring(L,2,&(text_oid_len_idx2));
  rc_idx1 = git_oid_shorten_add(this_idx1, text_oid_idx2);
		if(rc_idx1 >= 0) {
			lua_pushinteger(L, rc_idx1);
			return 1;
		}
		
  /* check for error. */
  if((GIT_OK != rc_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, rc_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: new */
static int ODB__new__meth(lua_State *L) {
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  ODB * this_idx1;
  GitError err_idx2 = GIT_OK;
  err_idx2 = git_odb_new(&(this_idx1));
  if(!(GIT_OK != err_idx2)) {
    obj_type_ODB_push(L, this_idx1, this_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: open */
static int ODB__open__meth(lua_State *L) {
  size_t object_dir_len_idx1;
  const char * object_dir_idx1;
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  ODB * this_idx1;
  GitError err_idx2 = GIT_OK;
  object_dir_idx1 = luaL_checklstring(L,1,&(object_dir_len_idx1));
  err_idx2 = git_odb_open(&(this_idx1), object_dir_idx1);
  if(!(GIT_OK != err_idx2)) {
    obj_type_ODB_push(L, this_idx1, this_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: free */
static int ODB__free__meth(lua_State *L) {
  int this_flags_idx1 = 0;
  ODB * this_idx1;
  this_idx1 = obj_type_ODB_delete(L,1,&(this_flags_idx1));
  if(!(this_flags_idx1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
  git_odb_free(this_idx1);
  return 0;
}

/* method: add_backend */
static int ODB__add_backend__meth(lua_State *L) {
  ODB * this_idx1;
  ODBBackend * backend_idx2;
  int priority_idx3;
  GitError err_idx1 = GIT_OK;
  this_idx1 = obj_type_ODB_check(L,1);
  backend_idx2 = obj_type_ODBBackend_check(L,2);
  priority_idx3 = luaL_checkinteger(L,3);
	err_idx1 = git_odb_add_backend(this_idx1, &(backend_idx2->backend), priority_idx3);
	ODBBackend_ref(backend_idx2);

  /* check for error. */
  if((GIT_OK != err_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, err_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: add_alternate */
static int ODB__add_alternate__meth(lua_State *L) {
  ODB * this_idx1;
  ODBBackend * backend_idx2;
  int priority_idx3;
  GitError err_idx1 = GIT_OK;
  this_idx1 = obj_type_ODB_check(L,1);
  backend_idx2 = obj_type_ODBBackend_check(L,2);
  priority_idx3 = luaL_checkinteger(L,3);
	err_idx1 = git_odb_add_alternate(this_idx1, &(backend_idx2->backend), priority_idx3);
	ODBBackend_ref(backend_idx2);

  /* check for error. */
  if((GIT_OK != err_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, err_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: read */
static int ODB__read__meth(lua_State *L) {
  ODB * this_idx1;
  OID id_idx2;
  int out_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  OdbObject * out_idx1;
  GitError rc_git_odb_read_idx2 = GIT_OK;
  this_idx1 = obj_type_ODB_check(L,1);
  id_idx2 = obj_type_OID_check(L,2);
  rc_git_odb_read_idx2 = git_odb_read(&(out_idx1), this_idx1, &(id_idx2));
  if(!(GIT_OK != rc_git_odb_read_idx2)) {
    obj_type_OdbObject_push(L, out_idx1, out_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, rc_git_odb_read_idx2);
  return 2;
}

/* method: read_prefix */
static int ODB__read_prefix__meth(lua_State *L) {
  ODB * this_idx1;
  OID short_id_idx2;
  unsigned int len_idx3;
  int out_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  OdbObject * out_idx1;
  GitError rc_git_odb_read_prefix_idx2 = GIT_OK;
  this_idx1 = obj_type_ODB_check(L,1);
  short_id_idx2 = obj_type_OID_check(L,2);
  len_idx3 = luaL_checkinteger(L,3);
  rc_git_odb_read_prefix_idx2 = git_odb_read_prefix(&(out_idx1), this_idx1, &(short_id_idx2), len_idx3);
  if(!(GIT_OK != rc_git_odb_read_prefix_idx2)) {
    obj_type_OdbObject_push(L, out_idx1, out_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, rc_git_odb_read_prefix_idx2);
  return 2;
}

/* method: read_header */
static int ODB__read_header__meth(lua_State *L) {
  ODB * this_idx1;
  OID id_idx2;
  size_t len_p_idx1 = 0;
  const char * type_idx2 = NULL;
  GitError err_idx3 = GIT_OK;
  git_otype otype_idx4;
  this_idx1 = obj_type_ODB_check(L,1);
  id_idx2 = obj_type_OID_check(L,2);
  err_idx3 = git_odb_read_header(&(len_p_idx1), &(otype_idx4), this_idx1, &(id_idx2));
  type_idx2 = git_object_type2string(otype_idx4);
  if(!(GIT_OK != err_idx3)) {
    lua_pushinteger(L, len_p_idx1);
  } else {
    lua_pushnil(L);
  }
  if(!(GIT_OK != err_idx3)) {
    lua_pushstring(L, type_idx2);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx3);
  return 3;
}

/* method: exists */
static int ODB__exists__meth(lua_State *L) {
  ODB * this_idx1;
  OID id_idx2;
  GitError err_idx1 = GIT_OK;
  this_idx1 = obj_type_ODB_check(L,1);
  id_idx2 = obj_type_OID_check(L,2);
  err_idx1 = git_odb_exists(this_idx1, &(id_idx2));
  /* check for error. */
  if((GIT_OK != err_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, err_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: write */
static int ODB__write__meth(lua_State *L) {
  ODB * this_idx1;
  size_t data_len_idx2;
  const char * data_idx2;
  size_t type_len_idx3;
  const char * type_idx3;
  OID id_idx1;
  GitError rc_git_odb_write_idx2 = GIT_OK;
  git_otype otype_idx3;
  this_idx1 = obj_type_ODB_check(L,1);
  data_idx2 = luaL_checklstring(L,2,&(data_len_idx2));
  type_idx3 = luaL_checklstring(L,3,&(type_len_idx3));
  otype_idx3 = git_object_string2type(type_idx3);
  rc_git_odb_write_idx2 = git_odb_write(&(id_idx1), this_idx1, data_idx2, data_len_idx2, otype_idx3);
  if(!(GIT_OK != rc_git_odb_write_idx2)) {
    obj_type_OID_push(L, id_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, rc_git_odb_write_idx2);
  return 2;
}

/* method: hash */
static int ODB__hash__func(lua_State *L) {
  size_t data_len_idx1;
  const char * data_idx1;
  git_otype otype_idx2;
  OID id_idx1;
  GitError err_idx2 = GIT_OK;
  data_idx1 = luaL_checklstring(L,1,&(data_len_idx1));
  otype_idx2 = luaL_checkinteger(L,2);
  err_idx2 = git_odb_hash(&(id_idx1), data_idx1, data_len_idx1, otype_idx2);
  if(!(GIT_OK != err_idx2)) {
    obj_type_OID_push(L, id_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: hashfile */
static int ODB__hashfile__func(lua_State *L) {
  size_t path_len_idx1;
  const char * path_idx1;
  git_otype otype_idx2;
  OID id_idx1;
  GitError err_idx2 = GIT_OK;
  path_idx1 = luaL_checklstring(L,1,&(path_len_idx1));
  otype_idx2 = luaL_checkinteger(L,2);
  err_idx2 = git_odb_hashfile(&(id_idx1), path_idx1, otype_idx2);
  if(!(GIT_OK != err_idx2)) {
    obj_type_OID_push(L, id_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: new */
static int ODBBackend__new__meth(lua_State *L) {
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  ODBBackend * this_idx1;
	luaL_checktype(L, 1, LUA_TTABLE);
	lua_settop(L, 1);
	/* create backend object. */
	this_idx1 = calloc(1, sizeof(ODBBackend));
	this_idx1->ref_count = 1;
	this_idx1->L = L;
	/* get each callback from table. */
#define REF_CB(_name) \
	lua_getfield(L, 1, "on_" #_name); \
	this_idx1->_name = luaL_ref(L, LUA_REGISTRYINDEX); \
	this_idx1->backend._name = odb_backend_ ## _name ## _cb;

	REF_CB(read)
	REF_CB(read_prefix)
	REF_CB(read_header)
	REF_CB(write)
	REF_CB(exists)
	REF_CB(free)
#undef REF_CB

#ifdef GIT_ODB_BACKEND_VERSION
  this_idx1->backend.version = GIT_ODB_BACKEND_VERSION;
#endif

  obj_type_ODBBackend_push(L, this_idx1, this_flags_idx1);
  return 1;
}

/* method: _priv */
static int ODBBackend__delete__meth(lua_State *L) {
  int this_flags_idx1 = 0;
  ODBBackend * this_idx1;
  this_idx1 = obj_type_ODBBackend_delete(L,1,&(this_flags_idx1));
  if(!(this_flags_idx1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
	ODBBackend_unref(this_idx1);

  return 0;
}

/* method: bare */
static int Index__bare__meth(lua_State *L) {
  size_t index_path_len_idx1;
  const char * index_path_idx1;
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  Index * this_idx1;
  GitError err_idx2 = GIT_OK;
  index_path_idx1 = luaL_checklstring(L,1,&(index_path_len_idx1));
  err_idx2 = git_index_open(&(this_idx1), index_path_idx1);
  if(!(GIT_OK != err_idx2)) {
    obj_type_Index_push(L, this_idx1, this_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: _priv */
static int Index__delete__meth(lua_State *L) {
  int this_flags_idx1 = 0;
  Index * this_idx1;
  this_idx1 = obj_type_Index_delete(L,1,&(this_flags_idx1));
  if(!(this_flags_idx1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
  git_index_free(this_idx1);
  return 0;
}

/* method: clear */
static int Index__clear__meth(lua_State *L) {
  Index * this_idx1;
  this_idx1 = obj_type_Index_check(L,1);
  git_index_clear(this_idx1);
  return 0;
}

/* method: read */
static int Index__read__meth(lua_State *L) {
  Index * this_idx1;
  int force_idx2;
  GitError rc_git_index_read_idx1 = GIT_OK;
  this_idx1 = obj_type_Index_check(L,1);
  force_idx2 = luaL_checkinteger(L,2);
  rc_git_index_read_idx1 = git_index_read(this_idx1, force_idx2);
  /* check for error. */
  if((GIT_OK != rc_git_index_read_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, rc_git_index_read_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: write */
static int Index__write__meth(lua_State *L) {
  Index * this_idx1;
  GitError rc_git_index_write_idx1 = GIT_OK;
  this_idx1 = obj_type_Index_check(L,1);
  rc_git_index_write_idx1 = git_index_write(this_idx1);
  /* check for error. */
  if((GIT_OK != rc_git_index_write_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, rc_git_index_write_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: find */
static int Index__find__meth(lua_State *L) {
  Index * this_idx1;
  size_t * at_pos_idx2;
  size_t path_len_idx3;
  const char * path_idx3;
  int rc_git_index_find_idx1 = 0;
  this_idx1 = obj_type_Index_check(L,1);
  at_pos_idx2 = lua_touserdata(L,2);
  path_idx3 = luaL_checklstring(L,3,&(path_len_idx3));
  rc_git_index_find_idx1 = git_index_find(at_pos_idx2, this_idx1, path_idx3);
  lua_pushinteger(L, rc_git_index_find_idx1);
  return 1;
}

/* method: add_bypath */
static int Index__add_bypath__meth(lua_State *L) {
  Index * this_idx1;
  size_t path_len_idx2;
  const char * path_idx2;
  GitError rc_git_index_add_bypath_idx1 = GIT_OK;
  this_idx1 = obj_type_Index_check(L,1);
  path_idx2 = luaL_checklstring(L,2,&(path_len_idx2));
  rc_git_index_add_bypath_idx1 = git_index_add_bypath(this_idx1, path_idx2);
  /* check for error. */
  if((GIT_OK != rc_git_index_add_bypath_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, rc_git_index_add_bypath_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: add */
static int Index__add__meth(lua_State *L) {
  Index * this_idx1;
  IndexEntry * source_entry_idx2;
  GitError rc_git_index_add_idx1 = GIT_OK;
  this_idx1 = obj_type_Index_check(L,1);
  source_entry_idx2 = obj_type_IndexEntry_check(L,2);
  rc_git_index_add_idx1 = git_index_add(this_idx1, source_entry_idx2);
  /* check for error. */
  if((GIT_OK != rc_git_index_add_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, rc_git_index_add_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: add_all */
static int Index__add_all__meth(lua_State *L) {
  Index * this_idx1;
  const StrArray * pathspec_idx2;
  unsigned int flags_idx3;
  void * callback_idx4;
  void * payload_idx5;
  GitError rc_git_index_add_all_idx1 = GIT_OK;
  this_idx1 = obj_type_Index_check(L,1);
  pathspec_idx2 = obj_type_StrArray_check(L,2);
  flags_idx3 = luaL_checkinteger(L,3);
  callback_idx4 = lua_touserdata(L,4);
  payload_idx5 = lua_touserdata(L,5);
  rc_git_index_add_all_idx1 = git_index_add_all(this_idx1, pathspec_idx2, flags_idx3, callback_idx4, payload_idx5);
  /* check for error. */
  if((GIT_OK != rc_git_index_add_all_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, rc_git_index_add_all_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: remove */
static int Index__remove__meth(lua_State *L) {
  Index * this_idx1;
  size_t path_len_idx2;
  const char * path_idx2;
  int stage_idx3;
  GitError rc_git_index_remove_idx1 = GIT_OK;
  this_idx1 = obj_type_Index_check(L,1);
  path_idx2 = luaL_checklstring(L,2,&(path_len_idx2));
  stage_idx3 = luaL_checkinteger(L,3);
  rc_git_index_remove_idx1 = git_index_remove(this_idx1, path_idx2, stage_idx3);
  /* check for error. */
  if((GIT_OK != rc_git_index_remove_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, rc_git_index_remove_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: remove_all */
static int Index__remove_all__meth(lua_State *L) {
  Index * this_idx1;
  const StrArray * pathspec_idx2;
  void * callback_idx3;
  void * payload_idx4;
  GitError rc_git_index_remove_all_idx1 = GIT_OK;
  this_idx1 = obj_type_Index_check(L,1);
  pathspec_idx2 = obj_type_StrArray_check(L,2);
  callback_idx3 = lua_touserdata(L,3);
  payload_idx4 = lua_touserdata(L,4);
  rc_git_index_remove_all_idx1 = git_index_remove_all(this_idx1, pathspec_idx2, callback_idx3, payload_idx4);
  /* check for error. */
  if((GIT_OK != rc_git_index_remove_all_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, rc_git_index_remove_all_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: get_byindex */
static int Index__get_byindex__meth(lua_State *L) {
  Index * this_idx1;
  size_t n_idx2;
  const IndexEntry * rc_git_index_get_byindex_idx1;
  this_idx1 = obj_type_Index_check(L,1);
  n_idx2 = luaL_checkinteger(L,2);
  rc_git_index_get_byindex_idx1 = git_index_get_byindex(this_idx1, n_idx2);
  obj_type_IndexEntry_push(L, rc_git_index_get_byindex_idx1, 0);
  return 1;
}

/* method: get_bypath */
static int Index__get_bypath__meth(lua_State *L) {
  Index * this_idx1;
  size_t path_len_idx2;
  const char * path_idx2;
  int stage_idx3;
  const IndexEntry * rc_git_index_get_bypath_idx1;
  this_idx1 = obj_type_Index_check(L,1);
  path_idx2 = luaL_checklstring(L,2,&(path_len_idx2));
  stage_idx3 = luaL_checkinteger(L,3);
  rc_git_index_get_bypath_idx1 = git_index_get_bypath(this_idx1, path_idx2, stage_idx3);
  obj_type_IndexEntry_push(L, rc_git_index_get_bypath_idx1, 0);
  return 1;
}

/* method: entrycount */
static int Index__entrycount__meth(lua_State *L) {
  Index * this_idx1;
  unsigned int rc_git_index_entrycount_idx1 = 0;
  this_idx1 = obj_type_Index_check(L,1);
  rc_git_index_entrycount_idx1 = git_index_entrycount(this_idx1);
  lua_pushinteger(L, rc_git_index_entrycount_idx1);
  return 1;
}

/* method: reuc_entrycount */
static int Index__reuc_entrycount__meth(lua_State *L) {
  Index * this_idx1;
  unsigned int rc_git_index_reuc_entrycount_idx1 = 0;
  this_idx1 = obj_type_Index_check(L,1);
  rc_git_index_reuc_entrycount_idx1 = git_index_reuc_entrycount(this_idx1);
  lua_pushinteger(L, rc_git_index_reuc_entrycount_idx1);
  return 1;
}

/* method: reuc_get_bypath */
static int Index__reuc_get_bypath__meth(lua_State *L) {
  Index * this_idx1;
  size_t path_len_idx2;
  const char * path_idx2;
  const IndexEntryUnmerged * rc_git_index_reuc_get_bypath_idx1;
  this_idx1 = obj_type_Index_check(L,1);
  path_idx2 = luaL_checklstring(L,2,&(path_len_idx2));
  rc_git_index_reuc_get_bypath_idx1 = git_index_reuc_get_bypath(this_idx1, path_idx2);
  obj_type_IndexEntryUnmerged_push(L, rc_git_index_reuc_get_bypath_idx1, 0);
  return 1;
}

/* method: reuc_get_byindex */
static int Index__reuc_get_byindex__meth(lua_State *L) {
  Index * this_idx1;
  int n_idx2;
  const IndexEntryUnmerged * rc_git_index_reuc_get_byindex_idx1;
  this_idx1 = obj_type_Index_check(L,1);
  n_idx2 = luaL_checkinteger(L,2);
  rc_git_index_reuc_get_byindex_idx1 = git_index_reuc_get_byindex(this_idx1, n_idx2);
  obj_type_IndexEntryUnmerged_push(L, rc_git_index_reuc_get_byindex_idx1, 0);
  return 1;
}

/* method: read_tree */
static int Index__read_tree__meth(lua_State *L) {
  Index * this_idx1;
  Tree * tree_idx2;
  GitError rc_git_index_read_tree_idx1 = GIT_OK;
  this_idx1 = obj_type_Index_check(L,1);
  tree_idx2 = obj_type_Tree_check(L,2);
  rc_git_index_read_tree_idx1 = git_index_read_tree(this_idx1, tree_idx2);
  /* check for error. */
  if((GIT_OK != rc_git_index_read_tree_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, rc_git_index_read_tree_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: new */
static int IndexEntry__new__meth(lua_State *L) {
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  IndexEntry * this_idx1;
	this_idx1 = calloc(1, sizeof(IndexEntry));

  obj_type_IndexEntry_push(L, this_idx1, this_flags_idx1);
  return 1;
}

/* method: _priv */
static int IndexEntry__delete__meth(lua_State *L) {
  int this_flags_idx1 = 0;
  IndexEntry * this_idx1;
  this_idx1 = obj_type_IndexEntry_delete(L,1,&(this_flags_idx1));
  if(!(this_flags_idx1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
	if(this_idx1->path != NULL) {
		free((void *)this_idx1->path);
	}
	free(this_idx1);

  return 0;
}

/* method: stage */
static int IndexEntry__stage__meth(lua_State *L) {
  IndexEntry * this_idx1;
  int rc_git_index_entry_stage_idx1 = 0;
  this_idx1 = obj_type_IndexEntry_check(L,1);
  rc_git_index_entry_stage_idx1 = git_index_entry_stage(this_idx1);
  lua_pushinteger(L, rc_git_index_entry_stage_idx1);
  return 1;
}

/* method: ctime */
static int IndexEntry__ctime__meth(lua_State *L) {
  IndexEntry * this_idx1;
  git_time_t secs_idx1 = 0;
  unsigned int nanosecs_idx2 = 0;
  this_idx1 = obj_type_IndexEntry_check(L,1);
	secs_idx1 = this_idx1->ctime.seconds;
	nanosecs_idx2 = this_idx1->ctime.nanoseconds;

  lua_pushinteger(L, secs_idx1);
  lua_pushinteger(L, nanosecs_idx2);
  return 2;
}

/* method: set_ctime */
static int IndexEntry__set_ctime__meth(lua_State *L) {
  IndexEntry * this_idx1;
  git_time_t secs_idx2;
  unsigned int nanosecs_idx3;
  this_idx1 = obj_type_IndexEntry_check(L,1);
  secs_idx2 = luaL_checkinteger(L,2);
  nanosecs_idx3 = luaL_checkinteger(L,3);
	this_idx1->ctime.seconds = secs_idx2;
	this_idx1->ctime.nanoseconds = nanosecs_idx3;

  return 0;
}

/* method: mtime */
static int IndexEntry__mtime__meth(lua_State *L) {
  IndexEntry * this_idx1;
  git_time_t secs_idx1 = 0;
  unsigned int nanosecs_idx2 = 0;
  this_idx1 = obj_type_IndexEntry_check(L,1);
	secs_idx1 = this_idx1->mtime.seconds;
	nanosecs_idx2 = this_idx1->mtime.nanoseconds;

  lua_pushinteger(L, secs_idx1);
  lua_pushinteger(L, nanosecs_idx2);
  return 2;
}

/* method: set_mtime */
static int IndexEntry__set_mtime__meth(lua_State *L) {
  IndexEntry * this_idx1;
  git_time_t secs_idx2;
  unsigned int nanosecs_idx3;
  this_idx1 = obj_type_IndexEntry_check(L,1);
  secs_idx2 = luaL_checkinteger(L,2);
  nanosecs_idx3 = luaL_checkinteger(L,3);
	this_idx1->mtime.seconds = secs_idx2;
	this_idx1->mtime.nanoseconds = nanosecs_idx3;

  return 0;
}

/* method: set_mode */
static int IndexEntry__set_mode__meth(lua_State *L) {
  IndexEntry * this_idx1;
  unsigned int mode_idx2;
  this_idx1 = obj_type_IndexEntry_check(L,1);
  mode_idx2 = luaL_checkinteger(L,2);
		this_idx1->mode = mode_idx2;

  return 0;
}

/* method: set_id */
static int IndexEntry__set_id__meth(lua_State *L) {
  IndexEntry * this_idx1;
  OID id_idx2;
  this_idx1 = obj_type_IndexEntry_check(L,1);
  id_idx2 = obj_type_OID_check(L,2);
	git_oid_cpy(&this_idx1->id, &id_idx2);

  return 0;
}

/* method: path */
static int IndexEntry__path__meth(lua_State *L) {
  IndexEntry * this_idx1;
  const char * ret_idx1 = NULL;
  this_idx1 = obj_type_IndexEntry_check(L,1);
ret_idx1 = this_idx1->path;
  lua_pushstring(L, ret_idx1);
  return 1;
}

/* method: set_path */
static int IndexEntry__set_path__meth(lua_State *L) {
  IndexEntry * this_idx1;
  size_t val_len_idx2;
  const char * val_idx2;
  this_idx1 = obj_type_IndexEntry_check(L,1);
  val_idx2 = luaL_checklstring(L,2,&(val_len_idx2));
	if(this_idx1->path != NULL) {
		free((void *)this_idx1->path);
	}
    char * path_buf = malloc(val_len_idx2);
	strncpy(path_buf, val_idx2, val_len_idx2);
	path_buf[val_len_idx2] = 0;
	this_idx1->path = path_buf;

  return 0;
}

/* method: dev */
static int IndexEntry__dev__meth(lua_State *L) {
  IndexEntry * this_idx1;
  unsigned int field_idx1 = 0;
  this_idx1 = obj_type_IndexEntry_check(L,1);
	field_idx1 = this_idx1->dev;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: ino */
static int IndexEntry__ino__meth(lua_State *L) {
  IndexEntry * this_idx1;
  unsigned int field_idx1 = 0;
  this_idx1 = obj_type_IndexEntry_check(L,1);
	field_idx1 = this_idx1->ino;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: mode */
static int IndexEntry__mode__meth(lua_State *L) {
  IndexEntry * this_idx1;
  unsigned int field_idx1 = 0;
  this_idx1 = obj_type_IndexEntry_check(L,1);
	field_idx1 = this_idx1->mode;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: uid */
static int IndexEntry__uid__meth(lua_State *L) {
  IndexEntry * this_idx1;
  unsigned int field_idx1 = 0;
  this_idx1 = obj_type_IndexEntry_check(L,1);
	field_idx1 = this_idx1->uid;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: gid */
static int IndexEntry__gid__meth(lua_State *L) {
  IndexEntry * this_idx1;
  unsigned int field_idx1 = 0;
  this_idx1 = obj_type_IndexEntry_check(L,1);
	field_idx1 = this_idx1->gid;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: file_size */
static int IndexEntry__file_size__meth(lua_State *L) {
  IndexEntry * this_idx1;
  off_t field_idx1 = 0;
  this_idx1 = obj_type_IndexEntry_check(L,1);
	field_idx1 = this_idx1->file_size;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: id */
static int IndexEntry__id__meth(lua_State *L) {
  IndexEntry * this_idx1;
  OID field_idx1;
  this_idx1 = obj_type_IndexEntry_check(L,1);
	field_idx1 = this_idx1->id;

  obj_type_OID_push(L, field_idx1);
  return 1;
}

/* method: flags */
static int IndexEntry__flags__meth(lua_State *L) {
  IndexEntry * this_idx1;
  unsigned int field_idx1 = 0;
  this_idx1 = obj_type_IndexEntry_check(L,1);
	field_idx1 = this_idx1->flags;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: flags_extended */
static int IndexEntry__flags_extended__meth(lua_State *L) {
  IndexEntry * this_idx1;
  unsigned int field_idx1 = 0;
  this_idx1 = obj_type_IndexEntry_check(L,1);
	field_idx1 = this_idx1->flags_extended;

  lua_pushinteger(L, field_idx1);
  return 1;
}

/* method: mode */
static int IndexEntryUnmerged__mode__meth(lua_State *L) {
  IndexEntryUnmerged * this_idx1;
  int idx_idx2;
  unsigned int mode_idx1 = 0;
  this_idx1 = obj_type_IndexEntryUnmerged_check(L,1);
  idx_idx2 = luaL_checkinteger(L,2);
	if(idx_idx2 < 0 || idx_idx2 >=3) {
		return luaL_argerror(L, 2, "Index out-of-bounds (0-2)");
	}
	mode_idx1 = this_idx1->mode[idx_idx2];

  lua_pushinteger(L, mode_idx1);
  return 1;
}

/* method: oid */
static int IndexEntryUnmerged__oid__meth(lua_State *L) {
  IndexEntryUnmerged * this_idx1;
  int idx_idx2;
  OID oid_idx1;
  this_idx1 = obj_type_IndexEntryUnmerged_check(L,1);
  idx_idx2 = luaL_checkinteger(L,2);
	if(idx_idx2 < 0 || idx_idx2 >=3) {
		return luaL_argerror(L, 2, "Index out-of-bounds (0-2)");
	}
	oid_idx1 = this_idx1->oid[idx_idx2];

  obj_type_OID_push(L, oid_idx1);
  return 1;
}

/* method: path */
static int IndexEntryUnmerged__path__meth(lua_State *L) {
  IndexEntryUnmerged * this_idx1;
  const char * ret_idx1 = NULL;
  this_idx1 = obj_type_IndexEntryUnmerged_check(L,1);
ret_idx1 = this_idx1->path;
  lua_pushstring(L, ret_idx1);
  return 1;
}

/* method: revparse_single */
static int Object__revparse_single__meth(lua_State *L) {
  Repository * repo_idx1;
  size_t spec_len_idx2;
  const char * spec_idx2;
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  Object * this_idx1;
  GitError err_idx2 = GIT_OK;
  repo_idx1 = obj_type_Repository_check(L,1);
  spec_idx2 = luaL_checklstring(L,2,&(spec_len_idx2));
  err_idx2 = git_revparse_single(&(this_idx1), repo_idx1, spec_idx2);
  if(!(GIT_OK != err_idx2)) {
    obj_type_Object_push(L, this_idx1, this_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: free */
static int Object__free__meth(lua_State *L) {
  int this_flags_idx1 = 0;
  Object * this_idx1;
  this_idx1 = obj_type_Object_delete(L,1,&(this_flags_idx1));
  if(!(this_flags_idx1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
  git_object_free(this_idx1);
  return 0;
}

/* method: id */
static int Object__id__meth(lua_State *L) {
  Object * this_idx1;
  OID id_idx1;
  this_idx1 = obj_type_Object_check(L,1);
  id_idx1 = *git_object_id(this_idx1);
  obj_type_OID_push(L, id_idx1);
  return 1;
}

/* method: type */
static int Object__type__meth(lua_State *L) {
  Object * this_idx1;
  git_otype otype_idx1;
  const char * type_idx2 = NULL;
  this_idx1 = obj_type_Object_check(L,1);
  otype_idx1 = git_object_type(this_idx1);
  type_idx2 = git_object_type2string(otype_idx1);
  lua_pushstring(L, type_idx2);
  return 1;
}

/* method: owner */
static int Object__owner__meth(lua_State *L) {
  Object * this_idx1;
  Repository * rc_git_object_owner_idx1;
  this_idx1 = obj_type_Object_check(L,1);
  rc_git_object_owner_idx1 = git_object_owner(this_idx1);
  obj_type_Repository_push(L, rc_git_object_owner_idx1, 0);
  return 1;
}

/* method: type2string */
static int Object__type2string__func(lua_State *L) {
  git_otype otype_idx1;
  const char * rc_git_object_type2string_idx1 = NULL;
  otype_idx1 = luaL_checkinteger(L,1);
  rc_git_object_type2string_idx1 = git_object_type2string(otype_idx1);
  lua_pushstring(L, rc_git_object_type2string_idx1);
  return 1;
}

/* method: string2type */
static int Object__string2type__func(lua_State *L) {
  size_t str_len_idx1;
  const char * str_idx1;
  git_otype rc_git_object_string2type_idx1;
  str_idx1 = luaL_checklstring(L,1,&(str_len_idx1));
  rc_git_object_string2type_idx1 = git_object_string2type(str_idx1);
  lua_pushinteger(L, rc_git_object_string2type_idx1);
  return 1;
}

static void dyn_caster_Object(void **obj, obj_type **type) {
  Object * base_obj = (Object *)*obj;
  switch(git_object_type(base_obj)) {
  case GIT_OBJ_COMMIT:
    *type = &(obj_type_Commit);
    break;
  case GIT_OBJ_BLOB:
    *type = &(obj_type_Blob);
    break;
  case GIT_OBJ_TREE:
    *type = &(obj_type_Tree);
    break;
  case GIT_OBJ_TAG:
    *type = &(obj_type_Tag);
    break;
  default:
    break;
  }
}

/* method: lookup */
static int Blob__lookup__meth(lua_State *L) {
  Repository * repo_idx1;
  OID id_idx2;
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  Blob * this_idx1;
  GitError err_idx2 = GIT_OK;
  repo_idx1 = obj_type_Repository_check(L,1);
  id_idx2 = obj_type_OID_check(L,2);
  err_idx2 = git_blob_lookup(&(this_idx1), repo_idx1, &(id_idx2));
  if(!(GIT_OK != err_idx2)) {
    obj_type_Blob_push(L, this_idx1, this_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: from_disk */
static int Blob__from_disk__func(lua_State *L) {
  Repository * repo_idx1;
  size_t path_len_idx2;
  const char * path_idx2;
  OID written_id_idx1;
  GitError err_idx2 = GIT_OK;
  repo_idx1 = obj_type_Repository_check(L,1);
  path_idx2 = luaL_checklstring(L,2,&(path_len_idx2));
  err_idx2 = git_blob_create_from_disk(&(written_id_idx1), repo_idx1, path_idx2);
  if(!(GIT_OK != err_idx2)) {
    obj_type_OID_push(L, written_id_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: from_buffer */
static int Blob__from_buffer__func(lua_State *L) {
  Repository * repo_idx1;
  size_t buffer_len_idx2;
  const char * buffer_idx2;
  OID written_id_idx1;
  GitError err_idx2 = GIT_OK;
  repo_idx1 = obj_type_Repository_check(L,1);
  buffer_idx2 = luaL_checklstring(L,2,&(buffer_len_idx2));
  err_idx2 = git_blob_create_from_buffer(&(written_id_idx1), repo_idx1, buffer_idx2, buffer_len_idx2);
  if(!(GIT_OK != err_idx2)) {
    obj_type_OID_push(L, written_id_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: rawcontent */
static int Blob__rawcontent__meth(lua_State *L) {
  Blob * this_idx1;
  size_t buff_len_idx1 = 0;
  const char * buff_idx1 = NULL;
  this_idx1 = obj_type_Blob_check(L,1);
  buff_idx1 = git_blob_rawcontent(this_idx1);
  buff_len_idx1 = git_blob_rawsize(this_idx1);
  if(buff_idx1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, buff_idx1,buff_len_idx1);
  return 1;
}

/* method: rawsize */
static int Blob__rawsize__meth(lua_State *L) {
  Blob * this_idx1;
  int rc_git_blob_rawsize_idx1 = 0;
  this_idx1 = obj_type_Blob_check(L,1);
  rc_git_blob_rawsize_idx1 = git_blob_rawsize(this_idx1);
  lua_pushinteger(L, rc_git_blob_rawsize_idx1);
  return 1;
}

/* method: new */
static int Signature__new__meth(lua_State *L) {
  size_t name_len_idx1;
  const char * name_idx1;
  size_t email_len_idx2;
  const char * email_idx2;
  git_time_t time_idx3;
  int offset_idx4;
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  Signature * this_idx1;
  GitError rc_git_signature_new_idx2 = GIT_OK;
  name_idx1 = luaL_checklstring(L,1,&(name_len_idx1));
  email_idx2 = luaL_checklstring(L,2,&(email_len_idx2));
  time_idx3 = luaL_checkinteger(L,3);
  offset_idx4 = luaL_checkinteger(L,4);
  rc_git_signature_new_idx2 = git_signature_new(&(this_idx1), name_idx1, email_idx2, time_idx3, offset_idx4);
  if(!(GIT_OK != rc_git_signature_new_idx2)) {
    obj_type_Signature_push(L, this_idx1, this_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, rc_git_signature_new_idx2);
  return 2;
}

/* method: now */
static int Signature__now__meth(lua_State *L) {
  size_t name_len_idx1;
  const char * name_idx1;
  size_t email_len_idx2;
  const char * email_idx2;
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  Signature * this_idx1;
  GitError rc_git_signature_now_idx2 = GIT_OK;
  name_idx1 = luaL_checklstring(L,1,&(name_len_idx1));
  email_idx2 = luaL_checklstring(L,2,&(email_len_idx2));
  rc_git_signature_now_idx2 = git_signature_now(&(this_idx1), name_idx1, email_idx2);
  if(!(GIT_OK != rc_git_signature_now_idx2)) {
    obj_type_Signature_push(L, this_idx1, this_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, rc_git_signature_now_idx2);
  return 2;
}

/* method: _priv */
static int Signature__delete__meth(lua_State *L) {
  int this_flags_idx1 = 0;
  Signature * this_idx1;
  this_idx1 = obj_type_Signature_delete(L,1,&(this_flags_idx1));
  if(!(this_flags_idx1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
  git_signature_free(this_idx1);
  return 0;
}

/* method: name */
static int Signature__name__meth(lua_State *L) {
  Signature * this_idx1;
  const char * name_idx1 = NULL;
  this_idx1 = obj_type_Signature_check(L,1);
name_idx1 = this_idx1->name;
  lua_pushstring(L, name_idx1);
  return 1;
}

/* method: email */
static int Signature__email__meth(lua_State *L) {
  Signature * this_idx1;
  const char * email_idx1 = NULL;
  this_idx1 = obj_type_Signature_check(L,1);
email_idx1 = this_idx1->email;
  lua_pushstring(L, email_idx1);
  return 1;
}

/* method: when */
static int Signature__when__meth(lua_State *L) {
  Signature * this_idx1;
  git_time_t time_idx1 = 0;
  int offset_idx2 = 0;
  this_idx1 = obj_type_Signature_check(L,1);
time_idx1 = this_idx1->when.time; offset_idx2 = this_idx1->when.offset;
  lua_pushinteger(L, time_idx1);
  lua_pushinteger(L, offset_idx2);
  return 2;
}

/* method: lookup */
static int Commit__lookup__meth(lua_State *L) {
  Repository * repo_idx1;
  OID id_idx2;
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  Commit * this_idx1;
  GitError err_idx2 = GIT_OK;
  repo_idx1 = obj_type_Repository_check(L,1);
  id_idx2 = obj_type_OID_check(L,2);
  err_idx2 = git_commit_lookup(&(this_idx1), repo_idx1, &(id_idx2));
  if(!(GIT_OK != err_idx2)) {
    obj_type_Commit_push(L, this_idx1, this_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: create */
static int Commit__create__func(lua_State *L) {
  OID oid_idx1;
  Repository * repo_idx2;
  size_t update_ref_len_idx3;
  const char * update_ref_idx3;
  Signature * author_idx4;
  Signature * committer_idx5;
  size_t message_encoding_len_idx6;
  const char * message_encoding_idx6;
  size_t message_len_idx7;
  const char * message_idx7;
  Tree * tree_idx8;
  Commit * parent_idx9;
  GitError err_idx1 = GIT_OK;
	int parent_count = 0;
#if LIBGIT2_VER_MAJOR == 1 && LIBGIT2_VER_MINOR == 8
	git_commit **parents;
#else
	const git_commit **parents;
#endif
	int n;

  oid_idx1 = obj_type_OID_check(L,1);
  repo_idx2 = obj_type_Repository_check(L,2);
  update_ref_idx3 = luaL_checklstring(L,3,&(update_ref_len_idx3));
  author_idx4 = obj_type_Signature_check(L,4);
  committer_idx5 = obj_type_Signature_check(L,5);
  message_encoding_idx6 = luaL_checklstring(L,6,&(message_encoding_len_idx6));
  message_idx7 = luaL_checklstring(L,7,&(message_len_idx7));
  tree_idx8 = obj_type_Tree_check(L,8);
  parent_idx9 = obj_type_Commit_check(L,9);
	/* count parents. */
	parent_count = lua_gettop(L) - 9 + 1;
	/* valid parents.  The first parent commit is already validated. */
	for(n = 1; n < parent_count; n++) {
		obj_type_Commit_check(L, 9 + n);
	}
	/* now it is safe to allocate oid array. */
	parents = malloc(parent_count * sizeof(git_commit *));

	/* copy oids from all parents into oid array. */
	parents[0] = parent_idx9;
	for(n = 1; n < parent_count; n++) {
		parents[n] = obj_type_Commit_check(L, 9 + n);
	}

	err_idx1 = git_commit_create(&(oid_idx1), repo_idx2, update_ref_idx3,
		author_idx4, committer_idx5, message_encoding_idx6, message_idx7,
		tree_idx8, parent_count, parents);
	/* free parent oid array. */
	free(parents);

  /* check for error. */
  if((GIT_OK != err_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, err_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: id */
static int Commit__id__meth(lua_State *L) {
  Commit * this_idx1;
  OID id_idx1;
  this_idx1 = obj_type_Commit_check(L,1);
  id_idx1 = *git_commit_id(this_idx1);
  obj_type_OID_push(L, id_idx1);
  return 1;
}

/* method: message_encoding */
static int Commit__message_encoding__meth(lua_State *L) {
  Commit * this_idx1;
  const char * rc_git_commit_message_encoding_idx1 = NULL;
  this_idx1 = obj_type_Commit_check(L,1);
  rc_git_commit_message_encoding_idx1 = git_commit_message_encoding(this_idx1);
  lua_pushstring(L, rc_git_commit_message_encoding_idx1);
  return 1;
}

/* method: message */
static int Commit__message__meth(lua_State *L) {
  Commit * this_idx1;
  const char * rc_git_commit_message_idx1 = NULL;
  this_idx1 = obj_type_Commit_check(L,1);
  rc_git_commit_message_idx1 = git_commit_message(this_idx1);
  lua_pushstring(L, rc_git_commit_message_idx1);
  return 1;
}

/* method: time */
static int Commit__time__meth(lua_State *L) {
  Commit * this_idx1;
  git_time_t rc_git_commit_time_idx1 = 0;
  this_idx1 = obj_type_Commit_check(L,1);
  rc_git_commit_time_idx1 = git_commit_time(this_idx1);
  lua_pushinteger(L, rc_git_commit_time_idx1);
  return 1;
}

/* method: time_offset */
static int Commit__time_offset__meth(lua_State *L) {
  Commit * this_idx1;
  int rc_git_commit_time_offset_idx1 = 0;
  this_idx1 = obj_type_Commit_check(L,1);
  rc_git_commit_time_offset_idx1 = git_commit_time_offset(this_idx1);
  lua_pushinteger(L, rc_git_commit_time_offset_idx1);
  return 1;
}

/* method: committer */
static int Commit__committer__meth(lua_State *L) {
  Commit * this_idx1;
  const Signature * rc_git_commit_committer_idx1;
  this_idx1 = obj_type_Commit_check(L,1);
  rc_git_commit_committer_idx1 = git_commit_committer(this_idx1);
  obj_type_Signature_push(L, rc_git_commit_committer_idx1, 0);
  return 1;
}

/* method: author */
static int Commit__author__meth(lua_State *L) {
  Commit * this_idx1;
  const Signature * rc_git_commit_author_idx1;
  this_idx1 = obj_type_Commit_check(L,1);
  rc_git_commit_author_idx1 = git_commit_author(this_idx1);
  obj_type_Signature_push(L, rc_git_commit_author_idx1, 0);
  return 1;
}

/* method: tree */
static int Commit__tree__meth(lua_State *L) {
  Commit * this_idx1;
  int tree_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  Tree * tree_idx1;
  GitError rc_git_commit_tree_idx2 = GIT_OK;
  this_idx1 = obj_type_Commit_check(L,1);
  rc_git_commit_tree_idx2 = git_commit_tree(&(tree_idx1), this_idx1);
  if(!(GIT_OK != rc_git_commit_tree_idx2)) {
    obj_type_Tree_push(L, tree_idx1, tree_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, rc_git_commit_tree_idx2);
  return 2;
}

/* method: parentcount */
static int Commit__parentcount__meth(lua_State *L) {
  Commit * this_idx1;
  unsigned int rc_git_commit_parentcount_idx1 = 0;
  this_idx1 = obj_type_Commit_check(L,1);
  rc_git_commit_parentcount_idx1 = git_commit_parentcount(this_idx1);
  lua_pushinteger(L, rc_git_commit_parentcount_idx1);
  return 1;
}

/* method: parent */
static int Commit__parent__meth(lua_State *L) {
  Commit * this_idx1;
  unsigned int n_idx2;
  Commit * parent_idx1;
  GitError rc_git_commit_parent_idx2 = GIT_OK;
  this_idx1 = obj_type_Commit_check(L,1);
  n_idx2 = luaL_checkinteger(L,2);
  rc_git_commit_parent_idx2 = git_commit_parent(&(parent_idx1), this_idx1, n_idx2);
  if(!(GIT_OK != rc_git_commit_parent_idx2)) {
    obj_type_Commit_push(L, parent_idx1, 0);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, rc_git_commit_parent_idx2);
  return 2;
}

/* method: lookup */
static int Tree__lookup__meth(lua_State *L) {
  Repository * repo_idx1;
  OID id_idx2;
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  Tree * this_idx1;
  GitError err_idx2 = GIT_OK;
  repo_idx1 = obj_type_Repository_check(L,1);
  id_idx2 = obj_type_OID_check(L,2);
  err_idx2 = git_tree_lookup(&(this_idx1), repo_idx1, &(id_idx2));
  if(!(GIT_OK != err_idx2)) {
    obj_type_Tree_push(L, this_idx1, this_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: entrycount */
static int Tree__entrycount__meth(lua_State *L) {
  Tree * this_idx1;
  size_t rc_git_tree_entrycount_idx1 = 0;
  this_idx1 = obj_type_Tree_check(L,1);
  rc_git_tree_entrycount_idx1 = git_tree_entrycount(this_idx1);
  lua_pushinteger(L, rc_git_tree_entrycount_idx1);
  return 1;
}

/* method: entry_byname */
static int Tree__entry_byname__meth(lua_State *L) {
  Tree * this_idx1;
  size_t filename_len_idx2;
  const char * filename_idx2;
  const TreeEntry * rc_git_tree_entry_byname_idx1;
  this_idx1 = obj_type_Tree_check(L,1);
  filename_idx2 = luaL_checklstring(L,2,&(filename_len_idx2));
  rc_git_tree_entry_byname_idx1 = git_tree_entry_byname(this_idx1, filename_idx2);
  obj_type_TreeEntry_push(L, rc_git_tree_entry_byname_idx1, 0);
  return 1;
}

/* method: entry_byindex */
static int Tree__entry_byindex__meth(lua_State *L) {
  Tree * this_idx1;
  int index_idx2;
  const TreeEntry * rc_git_tree_entry_byindex_idx1;
  this_idx1 = obj_type_Tree_check(L,1);
  index_idx2 = luaL_checkinteger(L,2);
  rc_git_tree_entry_byindex_idx1 = git_tree_entry_byindex(this_idx1, index_idx2);
  obj_type_TreeEntry_push(L, rc_git_tree_entry_byindex_idx1, 0);
  return 1;
}

/* method: bypath */
static int TreeEntry__bypath__meth(lua_State *L) {
  const Tree * tree_idx1;
  size_t path_len_idx2;
  const char * path_idx2;
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  TreeEntry * this_idx1;
  GitError err_idx2 = GIT_OK;
  tree_idx1 = obj_type_Tree_check(L,1);
  path_idx2 = luaL_checklstring(L,2,&(path_len_idx2));
  err_idx2 = git_tree_entry_bypath(&(this_idx1), tree_idx1, path_idx2);
  if(!(GIT_OK != err_idx2)) {
    obj_type_TreeEntry_push(L, this_idx1, this_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: free */
static int TreeEntry__free__meth(lua_State *L) {
  int this_flags_idx1 = 0;
  TreeEntry * this_idx1;
  this_idx1 = obj_type_TreeEntry_delete(L,1,&(this_flags_idx1));
  if(!(this_flags_idx1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
  git_tree_entry_free(this_idx1);
  return 0;
}

/* method: name */
static int TreeEntry__name__meth(lua_State *L) {
  TreeEntry * this_idx1;
  const char * rc_git_tree_entry_name_idx1 = NULL;
  this_idx1 = obj_type_TreeEntry_check(L,1);
  rc_git_tree_entry_name_idx1 = git_tree_entry_name(this_idx1);
  lua_pushstring(L, rc_git_tree_entry_name_idx1);
  return 1;
}

/* method: filemode */
static int TreeEntry__filemode__meth(lua_State *L) {
  TreeEntry * this_idx1;
  unsigned int rc_git_tree_entry_filemode_idx1 = 0;
  this_idx1 = obj_type_TreeEntry_check(L,1);
  rc_git_tree_entry_filemode_idx1 = git_tree_entry_filemode(this_idx1);
  lua_pushinteger(L, rc_git_tree_entry_filemode_idx1);
  return 1;
}

/* method: id */
static int TreeEntry__id__meth(lua_State *L) {
  TreeEntry * this_idx1;
  OID id_idx1;
  this_idx1 = obj_type_TreeEntry_check(L,1);
id_idx1 = *(git_tree_entry_id(this_idx1));
  obj_type_OID_push(L, id_idx1);
  return 1;
}

/* method: object */
static int TreeEntry__object__meth(lua_State *L) {
  TreeEntry * this_idx1;
  Repository * repo_idx2;
  int obj_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  Object * obj_idx1;
  GitError rc_git_tree_entry_to_object_idx2 = GIT_OK;
  this_idx1 = obj_type_TreeEntry_check(L,1);
  repo_idx2 = obj_type_Repository_check(L,2);
  rc_git_tree_entry_to_object_idx2 = git_tree_entry_to_object(&(obj_idx1), repo_idx2, this_idx1);
  if(!(GIT_OK != rc_git_tree_entry_to_object_idx2)) {
    obj_type_Object_push(L, obj_idx1, obj_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, rc_git_tree_entry_to_object_idx2);
  return 2;
}

/* method: lookup */
static int Tag__lookup__meth(lua_State *L) {
  Repository * repo_idx1;
  OID id_idx2;
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  Tag * this_idx1;
  GitError err_idx2 = GIT_OK;
  repo_idx1 = obj_type_Repository_check(L,1);
  id_idx2 = obj_type_OID_check(L,2);
  err_idx2 = git_tag_lookup(&(this_idx1), repo_idx1, &(id_idx2));
  if(!(GIT_OK != err_idx2)) {
    obj_type_Tag_push(L, this_idx1, this_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: target */
static int Tag__target__meth(lua_State *L) {
  Tag * this_idx1;
  Object * out_idx1;
  GitError rc_git_tag_target_idx2 = GIT_OK;
  this_idx1 = obj_type_Tag_check(L,1);
  rc_git_tag_target_idx2 = git_tag_target(&(out_idx1), this_idx1);
  if(!(GIT_OK != rc_git_tag_target_idx2)) {
    obj_type_Object_push(L, out_idx1, 0);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, rc_git_tag_target_idx2);
  return 2;
}

/* method: name */
static int Tag__name__meth(lua_State *L) {
  Tag * this_idx1;
  const char * rc_git_tag_name_idx1 = NULL;
  this_idx1 = obj_type_Tag_check(L,1);
  rc_git_tag_name_idx1 = git_tag_name(this_idx1);
  lua_pushstring(L, rc_git_tag_name_idx1);
  return 1;
}

/* method: tagger */
static int Tag__tagger__meth(lua_State *L) {
  Tag * this_idx1;
  const Signature * rc_git_tag_tagger_idx1;
  this_idx1 = obj_type_Tag_check(L,1);
  rc_git_tag_tagger_idx1 = git_tag_tagger(this_idx1);
  obj_type_Signature_push(L, rc_git_tag_tagger_idx1, 0);
  return 1;
}

/* method: message */
static int Tag__message__meth(lua_State *L) {
  Tag * this_idx1;
  const char * rc_git_tag_message_idx1 = NULL;
  this_idx1 = obj_type_Tag_check(L,1);
  rc_git_tag_message_idx1 = git_tag_message(this_idx1);
  lua_pushstring(L, rc_git_tag_message_idx1);
  return 1;
}

/* method: new */
static int RevWalk__new__meth(lua_State *L) {
  Repository * repo_idx1;
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  RevWalk * this_idx1;
  GitError err_idx2 = GIT_OK;
  repo_idx1 = obj_type_Repository_check(L,1);
  err_idx2 = git_revwalk_new(&(this_idx1), repo_idx1);
  if(!(GIT_OK != err_idx2)) {
    obj_type_RevWalk_push(L, this_idx1, this_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: close */
static int RevWalk__close__meth(lua_State *L) {
  int this_flags_idx1 = 0;
  RevWalk * this_idx1;
  this_idx1 = obj_type_RevWalk_delete(L,1,&(this_flags_idx1));
  if(!(this_flags_idx1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
  git_revwalk_free(this_idx1);
  return 0;
}

/* method: reset */
static int RevWalk__reset__meth(lua_State *L) {
  RevWalk * this_idx1;
  this_idx1 = obj_type_RevWalk_check(L,1);
  git_revwalk_reset(this_idx1);
  return 0;
}

/* method: push */
static int RevWalk__push__meth(lua_State *L) {
  RevWalk * this_idx1;
  OID id_idx2;
  GitError rc_git_revwalk_push_idx1 = GIT_OK;
  this_idx1 = obj_type_RevWalk_check(L,1);
  id_idx2 = obj_type_OID_check(L,2);
  rc_git_revwalk_push_idx1 = git_revwalk_push(this_idx1, &(id_idx2));
  /* check for error. */
  if((GIT_OK != rc_git_revwalk_push_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, rc_git_revwalk_push_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: hide */
static int RevWalk__hide__meth(lua_State *L) {
  RevWalk * this_idx1;
  OID id_idx2;
  GitError rc_git_revwalk_hide_idx1 = GIT_OK;
  this_idx1 = obj_type_RevWalk_check(L,1);
  id_idx2 = obj_type_OID_check(L,2);
  rc_git_revwalk_hide_idx1 = git_revwalk_hide(this_idx1, &(id_idx2));
  /* check for error. */
  if((GIT_OK != rc_git_revwalk_hide_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, rc_git_revwalk_hide_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: next */
static int RevWalk__next__meth(lua_State *L) {
  RevWalk * this_idx1;
  OID id_idx1;
  GitError rc_git_revwalk_next_idx2 = GIT_OK;
  this_idx1 = obj_type_RevWalk_check(L,1);
  rc_git_revwalk_next_idx2 = git_revwalk_next(&(id_idx1), this_idx1);
  if(!(GIT_OK != rc_git_revwalk_next_idx2)) {
    obj_type_OID_push(L, id_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, rc_git_revwalk_next_idx2);
  return 2;
}

/* method: sorting */
static int RevWalk__sorting__meth(lua_State *L) {
  RevWalk * this_idx1;
  unsigned int sort_mode_idx2;
  this_idx1 = obj_type_RevWalk_check(L,1);
  sort_mode_idx2 = luaL_checkinteger(L,2);
  git_revwalk_sorting(this_idx1, sort_mode_idx2);
  return 0;
}

/* method: repository */
static int RevWalk__repository__meth(lua_State *L) {
  RevWalk * this_idx1;
  Repository * rc_git_revwalk_repository_idx1;
  this_idx1 = obj_type_RevWalk_check(L,1);
  rc_git_revwalk_repository_idx1 = git_revwalk_repository(this_idx1);
  obj_type_Repository_push(L, rc_git_revwalk_repository_idx1, 0);
  return 1;
}

/* method: lookup */
static int Reference__lookup__meth(lua_State *L) {
  Repository * repo_idx1;
  size_t name_len_idx2;
  const char * name_idx2;
  int this_flags_idx1 = OBJ_UDATA_FLAG_OWN;
  Reference * this_idx1;
  GitError err_idx2 = GIT_OK;
  repo_idx1 = obj_type_Repository_check(L,1);
  name_idx2 = luaL_checklstring(L,2,&(name_len_idx2));
  err_idx2 = git_reference_lookup(&(this_idx1), repo_idx1, name_idx2);
  if(!(GIT_OK != err_idx2)) {
    obj_type_Reference_push(L, this_idx1, this_flags_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}

/* method: target */
static int Reference__target__meth(lua_State *L) {
  Reference * this_idx1;
  OID rc_git_reference_target_idx1;
  this_idx1 = obj_type_Reference_check(L,1);
  rc_git_reference_target_idx1 = *git_reference_target(this_idx1);
  obj_type_OID_push(L, rc_git_reference_target_idx1);
  return 1;
}

/* method: set_target */
static int Reference__set_target__meth(lua_State *L) {
  Reference * this_idx1;
  OID oid_idx2;
  size_t log_message_len_idx3;
  const char * log_message_idx3;
  Reference * ref_out_idx1;
  GitError rc_git_reference_set_target_idx2 = GIT_OK;
  this_idx1 = obj_type_Reference_check(L,1);
  oid_idx2 = obj_type_OID_check(L,2);
  log_message_idx3 = luaL_checklstring(L,3,&(log_message_len_idx3));
  rc_git_reference_set_target_idx2 = git_reference_set_target(&(ref_out_idx1), this_idx1, &(oid_idx2), log_message_idx3);
  if(!(GIT_OK != rc_git_reference_set_target_idx2)) {
    obj_type_Reference_push(L, ref_out_idx1, 0);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, rc_git_reference_set_target_idx2);
  return 2;
}

/* method: type */
static int Reference__type__meth(lua_State *L) {
  Reference * this_idx1;
  git_ref_t rc_git_reference_type_idx1;
  this_idx1 = obj_type_Reference_check(L,1);
  rc_git_reference_type_idx1 = git_reference_type(this_idx1);
  lua_pushinteger(L, rc_git_reference_type_idx1);
  return 1;
}

/* method: name */
static int Reference__name__meth(lua_State *L) {
  Reference * this_idx1;
  const char * rc_git_reference_name_idx1 = NULL;
  this_idx1 = obj_type_Reference_check(L,1);
  rc_git_reference_name_idx1 = git_reference_name(this_idx1);
  lua_pushstring(L, rc_git_reference_name_idx1);
  return 1;
}

/* method: resolve */
static int Reference__resolve__meth(lua_State *L) {
  Reference * this_idx1;
  Reference * resolved_ref_idx1;
  GitError rc_git_reference_resolve_idx2 = GIT_OK;
  this_idx1 = obj_type_Reference_check(L,1);
  rc_git_reference_resolve_idx2 = git_reference_resolve(&(resolved_ref_idx1), this_idx1);
  if(!(GIT_OK != rc_git_reference_resolve_idx2)) {
    obj_type_Reference_push(L, resolved_ref_idx1, 0);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, rc_git_reference_resolve_idx2);
  return 2;
}

/* method: owner */
static int Reference__owner__meth(lua_State *L) {
  Reference * this_idx1;
  Repository * rc_git_reference_owner_idx1;
  this_idx1 = obj_type_Reference_check(L,1);
  rc_git_reference_owner_idx1 = git_reference_owner(this_idx1);
  obj_type_Repository_push(L, rc_git_reference_owner_idx1, 0);
  return 1;
}

/* method: rename */
static int Reference__rename__meth(lua_State *L) {
  Reference * this_idx1;
  size_t new_name_len_idx2;
  const char * new_name_idx2;
  int force_idx3;
  size_t log_message_len_idx4;
  const char * log_message_idx4;
  Reference * ref_out_idx1;
  GitError rc_git_reference_rename_idx2 = GIT_OK;
  this_idx1 = obj_type_Reference_check(L,1);
  new_name_idx2 = luaL_checklstring(L,2,&(new_name_len_idx2));
  force_idx3 = luaL_checkinteger(L,3);
  log_message_idx4 = luaL_checklstring(L,4,&(log_message_len_idx4));
  rc_git_reference_rename_idx2 = git_reference_rename(&(ref_out_idx1), this_idx1, new_name_idx2, force_idx3, log_message_idx4);
  if(!(GIT_OK != rc_git_reference_rename_idx2)) {
    obj_type_Reference_push(L, ref_out_idx1, 0);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, rc_git_reference_rename_idx2);
  return 2;
}

/* method: delete */
static int Reference__delete__meth(lua_State *L) {
  Reference * this_idx1;
  GitError rc_git_reference_delete_idx1 = GIT_OK;
  this_idx1 = obj_type_Reference_check(L,1);
  rc_git_reference_delete_idx1 = git_reference_delete(this_idx1);
  /* check for error. */
  if((GIT_OK != rc_git_reference_delete_idx1)) {
    lua_pushnil(L);
      error_code__GitError__push(L, rc_git_reference_delete_idx1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: list */
static int Reference__list__func(lua_State *L) {
  Repository * repo_idx1;
  StrArray array_idx1_store;
  StrArray * array_idx1 = &(array_idx1_store);
  GitError err_idx2 = GIT_OK;
	git_strarray tmp_array = { .strings = NULL, .count = 0 };

  repo_idx1 = obj_type_Repository_check(L,1);
	/* push this onto stack now, just encase there is a out-of-memory error. */
	array_idx1 = obj_type_StrArray_push(L, &tmp_array);
	err_idx2 = git_reference_list(array_idx1, repo_idx1);
	if(err_idx2 == GIT_OK) {
		return 1; /* array is already on the stack. */
	} else {
		/* there is an error remove the temp array from stack. */
		lua_pop(L, 1);
		array_idx1 = NULL;
	}

  if(!(GIT_OK != err_idx2)) {
    obj_type_StrArray_push(L, array_idx1);
  } else {
    lua_pushnil(L);
  }
  error_code__GitError__push(L, err_idx2);
  return 2;
}



static const luaL_Reg obj_CheckoutOptions_pub_funcs[] = {
  {"init", CheckoutOptions__init__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_CheckoutOptions_methods[] = {
  {"set_checkout_strategy", CheckoutOptions__set_checkout_strategy__meth},
  {"set_disable_filters", CheckoutOptions__set_disable_filters__meth},
  {"set_dir_mode", CheckoutOptions__set_dir_mode__meth},
  {"set_file_mode", CheckoutOptions__set_file_mode__meth},
  {"set_file_open_flags", CheckoutOptions__set_file_open_flags__meth},
  {"set_notify_flags", CheckoutOptions__set_notify_flags__meth},
  {"paths", CheckoutOptions__paths__meth},
  {"set_paths", CheckoutOptions__set_paths__meth},
  {"set_baseline", CheckoutOptions__set_baseline__meth},
  {"set_baseline_index", CheckoutOptions__set_baseline_index__meth},
  {"set_target_directory", CheckoutOptions__set_target_directory__meth},
  {"set_ancestor_label", CheckoutOptions__set_ancestor_label__meth},
  {"set_our_label", CheckoutOptions__set_our_label__meth},
  {"set_their_label", CheckoutOptions__set_their_label__meth},
  {"version", CheckoutOptions__version__meth},
  {"checkout_strategy", CheckoutOptions__checkout_strategy__meth},
  {"disable_filters", CheckoutOptions__disable_filters__meth},
  {"dir_mode", CheckoutOptions__dir_mode__meth},
  {"file_mode", CheckoutOptions__file_mode__meth},
  {"file_open_flags", CheckoutOptions__file_open_flags__meth},
  {"notify_flags", CheckoutOptions__notify_flags__meth},
  {"baseline", CheckoutOptions__baseline__meth},
  {"baseline_index", CheckoutOptions__baseline_index__meth},
  {"target_directory", CheckoutOptions__target_directory__meth},
  {"ancestor_label", CheckoutOptions__ancestor_label__meth},
  {"our_label", CheckoutOptions__our_label__meth},
  {"their_label", CheckoutOptions__their_label__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_CheckoutOptions_metas[] = {
  {"__gc", CheckoutOptions__delete__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_CheckoutOptions_bases[] = {
  {-1, NULL}
};

static const obj_field obj_CheckoutOptions_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_CheckoutOptions_constants[] = {
  {"NONE", NULL, 1073741824, CONST_NUMBER},
  {"DONT_OVERWRITE_IGNORED", NULL, 524288, CONST_NUMBER},
  {"USE_THEIRS", NULL, 4096, CONST_NUMBER},
  {"UPDATE_SUBMODULES", NULL, 65536, CONST_NUMBER},
  {"CONFLICT_STYLE_DIFF3", NULL, 2097152, CONST_NUMBER},
  {"UPDATE_SUBMODULES_IF_CHANGED", NULL, 131072, CONST_NUMBER},
  {"DONT_UPDATE_INDEX", NULL, 256, CONST_NUMBER},
  {"VERSION", NULL, 1, CONST_NUMBER},
  {"REMOVE_UNTRACKED", NULL, 32, CONST_NUMBER},
  {"SKIP_UNMERGED", NULL, 1024, CONST_NUMBER},
  {"USE_OURS", NULL, 2048, CONST_NUMBER},
  {"DISABLE_PATHSPEC_MATCH", NULL, 8192, CONST_NUMBER},
  {"DONT_REMOVE_EXISTING", NULL, 4194304, CONST_NUMBER},
  {"NO_REFRESH", NULL, 512, CONST_NUMBER},
  {"ALLOW_CONFLICTS", NULL, 16, CONST_NUMBER},
  {"CONFLICT_STYLE_ZDIFF3", NULL, 33554432, CONST_NUMBER},
  {"SAFE", NULL, 0, CONST_NUMBER},
  {"DRY_RUN", NULL, 16777216, CONST_NUMBER},
  {"DONT_WRITE_INDEX", NULL, 8388608, CONST_NUMBER},
  {"CONFLICT_STYLE_MERGE", NULL, 1048576, CONST_NUMBER},
  {"UPDATE_ONLY", NULL, 128, CONST_NUMBER},
  {"RECREATE_MISSING", NULL, 4, CONST_NUMBER},
  {"SKIP_LOCKED_DIRECTORIES", NULL, 262144, CONST_NUMBER},
  {"REMOVE_IGNORED", NULL, 64, CONST_NUMBER},
  {"FORCE", NULL, 2, CONST_NUMBER},
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_CheckoutOptions_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_DiffFindOptions_pub_funcs[] = {
  {"init", DiffFindOptions__init__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_DiffFindOptions_methods[] = {
  {"set_flags", DiffFindOptions__set_flags__meth},
  {"set_rename_threshold", DiffFindOptions__set_rename_threshold__meth},
  {"set_rename_from_rewrite_threshold", DiffFindOptions__set_rename_from_rewrite_threshold__meth},
  {"set_copy_threshold", DiffFindOptions__set_copy_threshold__meth},
  {"set_break_rewrite_threshold", DiffFindOptions__set_break_rewrite_threshold__meth},
  {"set_rename_limit", DiffFindOptions__set_rename_limit__meth},
  {"version", DiffFindOptions__version__meth},
  {"flags", DiffFindOptions__flags__meth},
  {"rename_threshold", DiffFindOptions__rename_threshold__meth},
  {"rename_from_rewrite_threshold", DiffFindOptions__rename_from_rewrite_threshold__meth},
  {"copy_threshold", DiffFindOptions__copy_threshold__meth},
  {"break_rewrite_threshold", DiffFindOptions__break_rewrite_threshold__meth},
  {"rename_limit", DiffFindOptions__rename_limit__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_DiffFindOptions_metas[] = {
  {"__gc", DiffFindOptions__delete__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_DiffFindOptions_bases[] = {
  {-1, NULL}
};

static const obj_field obj_DiffFindOptions_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_DiffFindOptions_constants[] = {
  {"REWRITES", NULL, 16, CONST_NUMBER},
  {"REMOVE_UNMODIFIED", NULL, 65536, CONST_NUMBER},
  {"FOR_UNTRACKED", NULL, 64, CONST_NUMBER},
  {"EXACT_MATCH_ONLY", NULL, 16384, CONST_NUMBER},
  {"ALL", NULL, 255, CONST_NUMBER},
  {"COPIES_FROM_UNMODIFIED", NULL, 8, CONST_NUMBER},
  {"VERSION", NULL, 1, CONST_NUMBER},
  {"RENAMES_FROM_REWRITES", NULL, 2, CONST_NUMBER},
  {"BREAK_REWRITES", NULL, 32, CONST_NUMBER},
  {"DONT_IGNORE_WHITESPACE", NULL, 8192, CONST_NUMBER},
  {"AND_BREAK_REWRITES", NULL, 48, CONST_NUMBER},
  {"BY_CONFIG", NULL, 0, CONST_NUMBER},
  {"IGNORE_WHITESPACE", NULL, 4096, CONST_NUMBER},
  {"BREAK_REWRITES_FOR_RENAMES_ONLY", NULL, 32768, CONST_NUMBER},
  {"RENAMES", NULL, 1, CONST_NUMBER},
  {"COPIES", NULL, 4, CONST_NUMBER},
  {"IGNORE_LEADING_WHITESPACE", NULL, 0, CONST_NUMBER},
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_DiffFindOptions_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_DiffOptions_pub_funcs[] = {
  {"init", DiffOptions__init__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_DiffOptions_methods[] = {
  {"set_flags", DiffOptions__set_flags__meth},
  {"set_ignore_submodules", DiffOptions__set_ignore_submodules__meth},
  {"pathspec", DiffOptions__pathspec__meth},
  {"set_pathspec", DiffOptions__set_pathspec__meth},
  {"set_context_lines", DiffOptions__set_context_lines__meth},
  {"set_interhunk_lines", DiffOptions__set_interhunk_lines__meth},
  {"set_oid_type", DiffOptions__set_oid_type__meth},
  {"set_id_abbrev", DiffOptions__set_id_abbrev__meth},
  {"set_max_size", DiffOptions__set_max_size__meth},
  {"set_old_prefix", DiffOptions__set_old_prefix__meth},
  {"set_new_prefix", DiffOptions__set_new_prefix__meth},
  {"version", DiffOptions__version__meth},
  {"flags", DiffOptions__flags__meth},
  {"ignore_submodules", DiffOptions__ignore_submodules__meth},
  {"context_lines", DiffOptions__context_lines__meth},
  {"interhunk_lines", DiffOptions__interhunk_lines__meth},
  {"oid_type", DiffOptions__oid_type__meth},
  {"id_abbrev", DiffOptions__id_abbrev__meth},
  {"max_size", DiffOptions__max_size__meth},
  {"old_prefix", DiffOptions__old_prefix__meth},
  {"new_prefix", DiffOptions__new_prefix__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_DiffOptions_metas[] = {
  {"__gc", DiffOptions__delete__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_DiffOptions_bases[] = {
  {-1, NULL}
};

static const obj_field obj_DiffOptions_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_DiffOptions_constants[] = {
  {"SUBMODULE_IGNORE_UNTRACKED", NULL, 2, CONST_NUMBER},
  {"SHOW_UNMODIFIED", NULL, 67108864, CONST_NUMBER},
  {"SHOW_BINARY", NULL, 1073741824, CONST_NUMBER},
  {"VERSION", NULL, 1, CONST_NUMBER},
  {"INCLUDE_UNREADABLE_AS_UNTRACKED", NULL, 131072, CONST_NUMBER},
  {"REVERSE", NULL, 1, CONST_NUMBER},
  {"FORCE_TEXT", NULL, 1048576, CONST_NUMBER},
  {"FORCE_BINARY", NULL, 2097152, CONST_NUMBER},
  {"SUBMODULE_IGNORE_ALL", NULL, 4, CONST_NUMBER},
  {"IGNORE_FILEMODE", NULL, 256, CONST_NUMBER},
  {"INCLUDE_UNTRACKED", NULL, 8, CONST_NUMBER},
  {"PATIENCE", NULL, 268435456, CONST_NUMBER},
  {"IGNORE_CASE", NULL, 1024, CONST_NUMBER},
  {"SUBMODULE_IGNORE_DIRTY", NULL, 3, CONST_NUMBER},
  {"INCLUDE_IGNORED", NULL, 2, CONST_NUMBER},
  {"DISABLE_PATHSPEC_MATCH", NULL, 4096, CONST_NUMBER},
  {"UPDATE_INDEX", NULL, 32768, CONST_NUMBER},
  {"INCLUDE_CASECHANGE", NULL, 2048, CONST_NUMBER},
  {"INCLUDE_UNMODIFIED", NULL, 32, CONST_NUMBER},
  {"RECURSE_UNTRACKED_DIRS", NULL, 16, CONST_NUMBER},
  {"RECURSE_IGNORED_DIRS", NULL, 4, CONST_NUMBER},
  {"INCLUDE_TYPECHANGE", NULL, 64, CONST_NUMBER},
  {"SKIP_BINARY_CHECK", NULL, 8192, CONST_NUMBER},
  {"NORMAL", NULL, 0, CONST_NUMBER},
  {"IGNORE_WHITESPACE_EOL", NULL, 16777216, CONST_NUMBER},
  {"ENABLE_FAST_UNTRACKED_DIRS", NULL, 16384, CONST_NUMBER},
  {"IGNORE_SUBMODULES", NULL, 512, CONST_NUMBER},
  {"INCLUDE_UNREADABLE", NULL, 65536, CONST_NUMBER},
  {"INDENT_HEURISTIC", NULL, 262144, CONST_NUMBER},
  {"MINIMAL", NULL, 536870912, CONST_NUMBER},
  {"SUBMODULE_IGNORE_NONE", NULL, 1, CONST_NUMBER},
  {"IGNORE_BLANK_LINES", NULL, 524288, CONST_NUMBER},
  {"IGNORE_WHITESPACE", NULL, 4194304, CONST_NUMBER},
  {"INCLUDE_TYPECHANGE_TREES", NULL, 128, CONST_NUMBER},
  {"IGNORE_WHITESPACE_CHANGE", NULL, 8388608, CONST_NUMBER},
  {"SUBMODULE_IGNORE_UNSPECIFIED", NULL, -1, CONST_NUMBER},
  {"SHOW_UNTRACKED_CONTENT", NULL, 33554432, CONST_NUMBER},
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_DiffOptions_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_DiffDelta_pub_funcs[] = {
  {"get", DiffDelta__get__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_DiffDelta_methods[] = {
  {"old_file", DiffDelta__old_file__meth},
  {"new_file", DiffDelta__new_file__meth},
  {"status", DiffDelta__status__meth},
  {"flags", DiffDelta__flags__meth},
  {"similarity", DiffDelta__similarity__meth},
  {"nfiles", DiffDelta__nfiles__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_DiffDelta_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_DiffDelta_bases[] = {
  {-1, NULL}
};

static const obj_field obj_DiffDelta_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_DiffDelta_constants[] = {
  {"UNREADABLE", NULL, 9, CONST_NUMBER},
  {"UNMODIFIED", NULL, 0, CONST_NUMBER},
  {"RENAMED", NULL, 4, CONST_NUMBER},
  {"COPIED", NULL, 5, CONST_NUMBER},
  {"CONFLICTED", NULL, 10, CONST_NUMBER},
  {"DELETED", NULL, 2, CONST_NUMBER},
  {"MODIFIED", NULL, 3, CONST_NUMBER},
  {"IGNORED", NULL, 6, CONST_NUMBER},
  {"UNTRACKED", NULL, 7, CONST_NUMBER},
  {"TYPECHANGE", NULL, 8, CONST_NUMBER},
  {"ADDED", NULL, 1, CONST_NUMBER},
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_DiffDelta_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_DiffStats_pub_funcs[] = {
  {"get", DiffStats__get__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_DiffStats_methods[] = {
  {"insertions", DiffStats__insertions__meth},
  {"deletions", DiffStats__deletions__meth},
  {"files_changed", DiffStats__files_changed__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_DiffStats_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_DiffStats_bases[] = {
  {-1, NULL}
};

static const obj_field obj_DiffStats_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_DiffStats_constants[] = {
  {"NONE", NULL, 0, CONST_NUMBER},
  {"INCLUDE_SUMMARY", NULL, 8, CONST_NUMBER},
  {"SHORT", NULL, 2, CONST_NUMBER},
  {"NUMBER", NULL, 4, CONST_NUMBER},
  {"FULL", NULL, 1, CONST_NUMBER},
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_DiffStats_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_DiffFile_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_DiffFile_methods[] = {
  {"id", DiffFile__id__meth},
  {"path", DiffFile__path__meth},
  {"size", DiffFile__size__meth},
  {"flags", DiffFile__flags__meth},
  {"mode", DiffFile__mode__meth},
  {"id_abbrev", DiffFile__id_abbrev__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_DiffFile_metas[] = {
  {"__gc", DiffFile__delete__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_DiffFile_bases[] = {
  {-1, NULL}
};

static const obj_field obj_DiffFile_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_DiffFile_constants[] = {
  {"BINARY", NULL, 1, CONST_NUMBER},
  {"NOT_BINARY", NULL, 2, CONST_NUMBER},
  {"VALID_SIZE", NULL, 16, CONST_NUMBER},
  {"EXISTS", NULL, 8, CONST_NUMBER},
  {"VALID_ID", NULL, 4, CONST_NUMBER},
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_DiffFile_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_Diff_pub_funcs[] = {
  {"index_to_workdir", Diff__index_to_workdir__meth},
  {"tree_to_index", Diff__tree_to_index__meth},
  {"tree_to_workdir_with_index", Diff__tree_to_workdir_with_index__meth},
  {"tree_to_tree", Diff__tree_to_tree__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_Diff_methods[] = {
  {"num", Diff__num__meth},
  {"find_similar", Diff__find_similar__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_Diff_metas[] = {
  {"__gc", Diff__delete__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_Diff_bases[] = {
  {-1, NULL}
};

static const obj_field obj_Diff_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_Diff_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_Diff_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_StatusOptions_pub_funcs[] = {
  {"init", StatusOptions__init__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_StatusOptions_methods[] = {
  {"set_show", StatusOptions__set_show__meth},
  {"set_flags", StatusOptions__set_flags__meth},
  {"pathspec", StatusOptions__pathspec__meth},
  {"set_pathspec", StatusOptions__set_pathspec__meth},
  {"set_baseline", StatusOptions__set_baseline__meth},
  {"set_rename_threshold", StatusOptions__set_rename_threshold__meth},
  {"version", StatusOptions__version__meth},
  {"show", StatusOptions__show__meth},
  {"flags", StatusOptions__flags__meth},
  {"baseline", StatusOptions__baseline__meth},
  {"rename_threshold", StatusOptions__rename_threshold__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_StatusOptions_metas[] = {
  {"__gc", StatusOptions__delete__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_StatusOptions_bases[] = {
  {-1, NULL}
};

static const obj_field obj_StatusOptions_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_StatusOptions_constants[] = {
  {"SHOW_INDEX_ONLY", NULL, 1, CONST_NUMBER},
  {"SORT_CASE_SENSITIVELY", NULL, 512, CONST_NUMBER},
  {"DISABLE_PATHSPEC_MATCH", NULL, 32, CONST_NUMBER},
  {"UPDATE_INDEX", NULL, 8192, CONST_NUMBER},
  {"INCLUDE_UNMODIFIED", NULL, 4, CONST_NUMBER},
  {"VERSION", NULL, 1, CONST_NUMBER},
  {"SORT_CASE_INSENSITIVELY", NULL, 1024, CONST_NUMBER},
  {"SHOW_WORKDIR_ONLY", NULL, 2, CONST_NUMBER},
  {"INCLUDE_UNREADABLE_AS_UNTRACKED", NULL, 32768, CONST_NUMBER},
  {"RECURSE_IGNORED_DIRS", NULL, 64, CONST_NUMBER},
  {"INCLUDE_UNREADABLE", NULL, 16384, CONST_NUMBER},
  {"RENAMES_INDEX_TO_WORKDIR", NULL, 256, CONST_NUMBER},
  {"SHOW_INDEX_AND_WORKDIR", NULL, 0, CONST_NUMBER},
  {"INCLUDE_UNTRACKED", NULL, 1, CONST_NUMBER},
  {"INCLUDE_IGNORED", NULL, 2, CONST_NUMBER},
  {"RECURSE_UNTRACKED_DIRS", NULL, 16, CONST_NUMBER},
  {"RENAMES_HEAD_TO_INDEX", NULL, 128, CONST_NUMBER},
  {"RENAMES_FROM_REWRITES", NULL, 2048, CONST_NUMBER},
  {"NO_REFRESH", NULL, 4096, CONST_NUMBER},
  {"EXCLUDE_SUBMODULES", NULL, 8, CONST_NUMBER},
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_StatusOptions_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_StatusList_pub_funcs[] = {
  {"new", StatusList__new__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_StatusList_methods[] = {
  {"entrycount", StatusList__entrycount__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_StatusList_metas[] = {
  {"__gc", StatusList__delete__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_StatusList_bases[] = {
  {-1, NULL}
};

static const obj_field obj_StatusList_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_StatusList_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_StatusList_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_StatusEntry_pub_funcs[] = {
  {"byindex", StatusEntry__byindex__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_StatusEntry_methods[] = {
  {"status", StatusEntry__status__meth},
  {"head_to_index", StatusEntry__head_to_index__meth},
  {"index_to_workdir", StatusEntry__index_to_workdir__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_StatusEntry_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_StatusEntry_bases[] = {
  {-1, NULL}
};

static const obj_field obj_StatusEntry_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_StatusEntry_constants[] = {
  {"INDEX_DELETED", NULL, 4, CONST_NUMBER},
  {"INDEX_NEW", NULL, 1, CONST_NUMBER},
  {"INDEX_MODIFIED", NULL, 2, CONST_NUMBER},
  {"IGNORED", NULL, 16384, CONST_NUMBER},
  {"CONFLICTED", NULL, 32768, CONST_NUMBER},
  {"WT_UNREADABLE", NULL, 4096, CONST_NUMBER},
  {"INDEX_RENAMED", NULL, 8, CONST_NUMBER},
  {"WT_DELETED", NULL, 512, CONST_NUMBER},
  {"WT_TYPECHANGE", NULL, 1024, CONST_NUMBER},
  {"WT_RENAMED", NULL, 2048, CONST_NUMBER},
  {"WT_NEW", NULL, 128, CONST_NUMBER},
  {"WT_MODIFIED", NULL, 256, CONST_NUMBER},
  {"INDEX_TYPECHANGE", NULL, 16, CONST_NUMBER},
  {"CURRENT", NULL, 0, CONST_NUMBER},
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_StatusEntry_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_StrArray_pub_funcs[] = {
  {"new", StrArray__new__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_StrArray_methods[] = {
  {"free", StrArray__free__meth},
  {"str", StrArray__str__meth},
  {"set_str", StrArray__set_str__meth},
  {"get_array", StrArray__get_array__meth},
  {"count", StrArray__count__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_StrArray_metas[] = {
  {"__gc", StrArray__free__meth},
  {"__tostring", obj_simple_udata_default_tostring},
  {"__eq", obj_simple_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_StrArray_bases[] = {
  {-1, NULL}
};

static const obj_field obj_StrArray_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_StrArray_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_StrArray_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_Repository_pub_funcs[] = {
  {"open", Repository__open__meth},
  {"init", Repository__init__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_Repository_methods[] = {
  {"head", Repository__head__meth},
  {"head_detached", Repository__head_detached__meth},
  {"set_head_detached", Repository__set_head_detached__meth},
  {"head_unborn", Repository__head_unborn__meth},
  {"is_empty", Repository__is_empty__meth},
  {"is_bare", Repository__is_bare__meth},
  {"path", Repository__path__meth},
  {"workdir", Repository__workdir__meth},
  {"set_workdir", Repository__set_workdir__meth},
  {"config", Repository__config__meth},
  {"set_config", Repository__set_config__meth},
  {"odb", Repository__odb__meth},
  {"set_odb", Repository__set_odb__meth},
  {"index", Repository__index__meth},
  {"set_index", Repository__set_index__meth},
  {"checkout_tree", Repository__checkout_tree__meth},
  {"checkout_head", Repository__checkout_head__meth},
  {"checkout_index", Repository__checkout_index__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_Repository_metas[] = {
  {"__gc", Repository__delete__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_Repository_bases[] = {
  {-1, NULL}
};

static const obj_field obj_Repository_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_Repository_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_Repository_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_Config_pub_funcs[] = {
  {"new", Config__new__meth},
  {"open", Config__open__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_Config_methods[] = {
  {"free", Config__free__meth},
  {"add_file_ondisk", Config__add_file_ondisk__meth},
  {"get_int32", Config__get_int32__meth},
  {"set_int32", Config__set_int32__meth},
  {"get_int64", Config__get_int64__meth},
  {"set_int64", Config__set_int64__meth},
  {"get_bool", Config__get_bool__meth},
  {"set_bool", Config__set_bool__meth},
  {"get_string", Config__get_string__meth},
  {"set_string", Config__set_string__meth},
  {"delete_entry", Config__delete_entry__meth},
  {"delete_multivar", Config__delete_multivar__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_Config_metas[] = {
  {"__gc", Config__free__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_Config_bases[] = {
  {-1, NULL}
};

static const obj_field obj_Config_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_Config_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_Config_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_OdbObject_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_OdbObject_methods[] = {
  {"free", OdbObject__free__meth},
  {"id", OdbObject__id__meth},
  {"data", OdbObject__data__meth},
  {"size", OdbObject__size__meth},
  {"type", OdbObject__type__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_OdbObject_metas[] = {
  {"__gc", OdbObject__free__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_OdbObject_bases[] = {
  {-1, NULL}
};

static const obj_field obj_OdbObject_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_OdbObject_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_OdbObject_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_OID_pub_funcs[] = {
  {"hex", OID__hex__meth},
  {"raw", OID__raw__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_OID_methods[] = {
  {"pathfmt", OID__pathfmt__meth},
  {"fmt", OID__fmt__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_OID_metas[] = {
  {"__tostring", OID____str____meth},
  {"__eq", OID____eq____meth},
  {NULL, NULL}
};

static const obj_base obj_OID_bases[] = {
  {-1, NULL}
};

static const obj_field obj_OID_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_OID_constants[] = {
#ifdef GIT_OID_RAWSZ
  {"RAWSZ", NULL, GIT_OID_RAWSZ, CONST_NUMBER},
#endif
#ifdef GIT_OID_MINPREFIXLEN
  {"MINPREFIXLEN", NULL, GIT_OID_MINPREFIXLEN, CONST_NUMBER},
#endif
#ifdef GIT_OID_HEXSZ
  {"HEXSZ", NULL, GIT_OID_HEXSZ, CONST_NUMBER},
#endif
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_OID_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_OID_Shorten_pub_funcs[] = {
  {"new", OID_Shorten__new__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_OID_Shorten_methods[] = {
  {"add", OID_Shorten__add__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_OID_Shorten_metas[] = {
  {"__gc", OID_Shorten__delete__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_OID_Shorten_bases[] = {
  {-1, NULL}
};

static const obj_field obj_OID_Shorten_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_OID_Shorten_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_OID_Shorten_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_ODB_pub_funcs[] = {
  {"new", ODB__new__meth},
  {"open", ODB__open__meth},
  {"hash", ODB__hash__func},
  {"hashfile", ODB__hashfile__func},
  {NULL, NULL}
};

static const luaL_Reg obj_ODB_methods[] = {
  {"free", ODB__free__meth},
  {"add_backend", ODB__add_backend__meth},
  {"add_alternate", ODB__add_alternate__meth},
  {"read", ODB__read__meth},
  {"read_prefix", ODB__read_prefix__meth},
  {"read_header", ODB__read_header__meth},
  {"exists", ODB__exists__meth},
  {"write", ODB__write__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_ODB_metas[] = {
  {"__gc", ODB__free__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_ODB_bases[] = {
  {-1, NULL}
};

static const obj_field obj_ODB_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_ODB_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_ODB_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_ODBBackend_pub_funcs[] = {
  {"new", ODBBackend__new__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_ODBBackend_methods[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_ODBBackend_metas[] = {
  {"__gc", ODBBackend__delete__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_ODBBackend_bases[] = {
  {-1, NULL}
};

static const obj_field obj_ODBBackend_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_ODBBackend_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_ODBBackend_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_Index_pub_funcs[] = {
  {"bare", Index__bare__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_Index_methods[] = {
  {"clear", Index__clear__meth},
  {"read", Index__read__meth},
  {"write", Index__write__meth},
  {"find", Index__find__meth},
  {"add_bypath", Index__add_bypath__meth},
  {"add", Index__add__meth},
  {"add_all", Index__add_all__meth},
  {"remove", Index__remove__meth},
  {"remove_all", Index__remove_all__meth},
  {"get_byindex", Index__get_byindex__meth},
  {"get_bypath", Index__get_bypath__meth},
  {"entrycount", Index__entrycount__meth},
  {"reuc_entrycount", Index__reuc_entrycount__meth},
  {"reuc_get_bypath", Index__reuc_get_bypath__meth},
  {"reuc_get_byindex", Index__reuc_get_byindex__meth},
  {"read_tree", Index__read_tree__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_Index_metas[] = {
  {"__gc", Index__delete__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_Index_bases[] = {
  {-1, NULL}
};

static const obj_field obj_Index_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_Index_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_Index_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_IndexEntry_pub_funcs[] = {
  {"new", IndexEntry__new__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_IndexEntry_methods[] = {
  {"stage", IndexEntry__stage__meth},
  {"ctime", IndexEntry__ctime__meth},
  {"set_ctime", IndexEntry__set_ctime__meth},
  {"mtime", IndexEntry__mtime__meth},
  {"set_mtime", IndexEntry__set_mtime__meth},
  {"set_mode", IndexEntry__set_mode__meth},
  {"set_id", IndexEntry__set_id__meth},
  {"path", IndexEntry__path__meth},
  {"set_path", IndexEntry__set_path__meth},
  {"dev", IndexEntry__dev__meth},
  {"ino", IndexEntry__ino__meth},
  {"mode", IndexEntry__mode__meth},
  {"uid", IndexEntry__uid__meth},
  {"gid", IndexEntry__gid__meth},
  {"file_size", IndexEntry__file_size__meth},
  {"id", IndexEntry__id__meth},
  {"flags", IndexEntry__flags__meth},
  {"flags_extended", IndexEntry__flags_extended__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_IndexEntry_metas[] = {
  {"__gc", IndexEntry__delete__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_IndexEntry_bases[] = {
  {-1, NULL}
};

static const obj_field obj_IndexEntry_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_IndexEntry_constants[] = {
  {"EXTENDED", NULL, 16384, CONST_NUMBER},
  {"STAGEMASK", NULL, 12288, CONST_NUMBER},
  {"VALID", NULL, 32768, CONST_NUMBER},
  {"NAMEMASK", NULL, 4095, CONST_NUMBER},
  {"STAGESHIFT", NULL, 12, CONST_NUMBER},
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_IndexEntry_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_IndexEntryUnmerged_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_IndexEntryUnmerged_methods[] = {
  {"mode", IndexEntryUnmerged__mode__meth},
  {"oid", IndexEntryUnmerged__oid__meth},
  {"path", IndexEntryUnmerged__path__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_IndexEntryUnmerged_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_IndexEntryUnmerged_bases[] = {
  {-1, NULL}
};

static const obj_field obj_IndexEntryUnmerged_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_IndexEntryUnmerged_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_IndexEntryUnmerged_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_Object_pub_funcs[] = {
  {"revparse_single", Object__revparse_single__meth},
  {"type2string", Object__type2string__func},
  {"string2type", Object__string2type__func},
  {NULL, NULL}
};

static const luaL_Reg obj_Object_methods[] = {
  {"free", Object__free__meth},
  {"id", Object__id__meth},
  {"type", Object__type__meth},
  {"owner", Object__owner__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_Object_metas[] = {
  {"__gc", Object__free__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_Object_bases[] = {
  {-1, NULL}
};

static const obj_field obj_Object_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_Object_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_Object_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_Blob_pub_funcs[] = {
  {"lookup", Blob__lookup__meth},
  {"from_disk", Blob__from_disk__func},
  {"from_buffer", Blob__from_buffer__func},
  {NULL, NULL}
};

static const luaL_Reg obj_Blob_methods[] = {
  {"type", Object__type__meth},
  {"free", Object__free__meth},
  {"id", Object__id__meth},
  {"owner", Object__owner__meth},
  {"rawcontent", Blob__rawcontent__meth},
  {"rawsize", Blob__rawsize__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_Blob_metas[] = {
  {"__gc", Object__free__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_Blob_bases[] = {
  {21, NULL},
  {-1, NULL}
};

static const obj_field obj_Blob_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_Blob_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_Blob_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_Signature_pub_funcs[] = {
  {"new", Signature__new__meth},
  {"now", Signature__now__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_Signature_methods[] = {
  {"name", Signature__name__meth},
  {"email", Signature__email__meth},
  {"when", Signature__when__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_Signature_metas[] = {
  {"__gc", Signature__delete__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_Signature_bases[] = {
  {-1, NULL}
};

static const obj_field obj_Signature_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_Signature_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_Signature_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_Commit_pub_funcs[] = {
  {"lookup", Commit__lookup__meth},
  {"create", Commit__create__func},
  {NULL, NULL}
};

static const luaL_Reg obj_Commit_methods[] = {
  {"type", Object__type__meth},
  {"free", Object__free__meth},
  {"owner", Object__owner__meth},
  {"id", Commit__id__meth},
  {"message_encoding", Commit__message_encoding__meth},
  {"message", Commit__message__meth},
  {"time", Commit__time__meth},
  {"time_offset", Commit__time_offset__meth},
  {"committer", Commit__committer__meth},
  {"author", Commit__author__meth},
  {"tree", Commit__tree__meth},
  {"parentcount", Commit__parentcount__meth},
  {"parent", Commit__parent__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_Commit_metas[] = {
  {"__gc", Object__free__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_Commit_bases[] = {
  {21, NULL},
  {-1, NULL}
};

static const obj_field obj_Commit_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_Commit_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_Commit_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_Tree_pub_funcs[] = {
  {"lookup", Tree__lookup__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_Tree_methods[] = {
  {"type", Object__type__meth},
  {"free", Object__free__meth},
  {"id", Object__id__meth},
  {"owner", Object__owner__meth},
  {"entrycount", Tree__entrycount__meth},
  {"entry_byname", Tree__entry_byname__meth},
  {"entry_byindex", Tree__entry_byindex__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_Tree_metas[] = {
  {"__gc", Object__free__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_Tree_bases[] = {
  {21, NULL},
  {-1, NULL}
};

static const obj_field obj_Tree_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_Tree_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_Tree_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_TreeEntry_pub_funcs[] = {
  {"bypath", TreeEntry__bypath__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_TreeEntry_methods[] = {
  {"free", TreeEntry__free__meth},
  {"name", TreeEntry__name__meth},
  {"filemode", TreeEntry__filemode__meth},
  {"id", TreeEntry__id__meth},
  {"object", TreeEntry__object__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_TreeEntry_metas[] = {
  {"__gc", TreeEntry__free__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_TreeEntry_bases[] = {
  {-1, NULL}
};

static const obj_field obj_TreeEntry_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_TreeEntry_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_TreeEntry_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_Tag_pub_funcs[] = {
  {"lookup", Tag__lookup__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_Tag_methods[] = {
  {"type", Object__type__meth},
  {"free", Object__free__meth},
  {"id", Object__id__meth},
  {"owner", Object__owner__meth},
  {"target", Tag__target__meth},
  {"name", Tag__name__meth},
  {"tagger", Tag__tagger__meth},
  {"message", Tag__message__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_Tag_metas[] = {
  {"__gc", Object__free__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_Tag_bases[] = {
  {21, NULL},
  {-1, NULL}
};

static const obj_field obj_Tag_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_Tag_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_Tag_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_RevWalk_pub_funcs[] = {
  {"new", RevWalk__new__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_RevWalk_methods[] = {
  {"close", RevWalk__close__meth},
  {"reset", RevWalk__reset__meth},
  {"push", RevWalk__push__meth},
  {"hide", RevWalk__hide__meth},
  {"next", RevWalk__next__meth},
  {"sorting", RevWalk__sorting__meth},
  {"repository", RevWalk__repository__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_RevWalk_metas[] = {
  {"__gc", RevWalk__close__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_RevWalk_bases[] = {
  {-1, NULL}
};

static const obj_field obj_RevWalk_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_RevWalk_constants[] = {
  {"SORT_NONE", NULL, 0, CONST_NUMBER},
  {"SORT_TIME", NULL, 2, CONST_NUMBER},
  {"SORT_REVERSE", NULL, 4, CONST_NUMBER},
  {"SORT_TOPOLOGICAL", NULL, 1, CONST_NUMBER},
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_RevWalk_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_Reference_pub_funcs[] = {
  {"lookup", Reference__lookup__meth},
  {"list", Reference__list__func},
  {NULL, NULL}
};

static const luaL_Reg obj_Reference_methods[] = {
  {"target", Reference__target__meth},
  {"set_target", Reference__set_target__meth},
  {"type", Reference__type__meth},
  {"name", Reference__name__meth},
  {"resolve", Reference__resolve__meth},
  {"owner", Reference__owner__meth},
  {"rename", Reference__rename__meth},
  {"delete", Reference__delete__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_Reference_metas[] = {
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_Reference_bases[] = {
  {-1, NULL}
};

static const obj_field obj_Reference_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_Reference_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_Reference_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg git2_function[] = {
  {"version", git2__version__func},
  {NULL, NULL}
};

static const obj_const git2_constants[] = {
#ifdef GIT_REVWALKOVER
  {"REVWALKOVER", NULL, GIT_REVWALKOVER, CONST_NUMBER},
#endif
#ifdef GIT_ERROR
  {"ERROR", NULL, GIT_ERROR, CONST_NUMBER},
#endif
  {"REF_INVALID", NULL, 0, CONST_NUMBER},
#ifdef GIT_PASSTHROUGH
  {"PASSTHROUGH", NULL, GIT_PASSTHROUGH, CONST_NUMBER},
#endif
  {"REF_SYMBOLIC", NULL, 2, CONST_NUMBER},
#ifdef GIT_EAMBIGUOUS
  {"EAMBIGUOUS", NULL, GIT_EAMBIGUOUS, CONST_NUMBER},
#endif
#ifdef GIT_OK
  {"OK", NULL, GIT_OK, CONST_NUMBER},
#endif
  {"REF_PACKED", NULL, 4, CONST_NUMBER},
  {"REF_HAS_PEEL", NULL, 8, CONST_NUMBER},
#ifdef GIT_EBUFS
  {"EBUFS", NULL, GIT_EBUFS, CONST_NUMBER},
#endif
  {"REF_LISTALL", NULL, 7, CONST_NUMBER},
#ifdef GIT_EEXISTS
  {"EEXISTS", NULL, GIT_EEXISTS, CONST_NUMBER},
#endif
  {"REF_OID", NULL, 1, CONST_NUMBER},
#ifdef GIT_ENOTFOUND
  {"ENOTFOUND", NULL, GIT_ENOTFOUND, CONST_NUMBER},
#endif
  {NULL, NULL, 0.0 , 0}
};



static const reg_sub_module reg_sub_modules[] = {
  { &(obj_type_CheckoutOptions), REG_OBJECT, obj_CheckoutOptions_pub_funcs, obj_CheckoutOptions_methods, obj_CheckoutOptions_metas, obj_CheckoutOptions_bases, obj_CheckoutOptions_fields, obj_CheckoutOptions_constants, obj_CheckoutOptions_implements, 0},
  { &(obj_type_DiffFindOptions), REG_OBJECT, obj_DiffFindOptions_pub_funcs, obj_DiffFindOptions_methods, obj_DiffFindOptions_metas, obj_DiffFindOptions_bases, obj_DiffFindOptions_fields, obj_DiffFindOptions_constants, obj_DiffFindOptions_implements, 0},
  { &(obj_type_DiffOptions), REG_OBJECT, obj_DiffOptions_pub_funcs, obj_DiffOptions_methods, obj_DiffOptions_metas, obj_DiffOptions_bases, obj_DiffOptions_fields, obj_DiffOptions_constants, obj_DiffOptions_implements, 0},
  { &(obj_type_DiffDelta), REG_OBJECT, obj_DiffDelta_pub_funcs, obj_DiffDelta_methods, obj_DiffDelta_metas, obj_DiffDelta_bases, obj_DiffDelta_fields, obj_DiffDelta_constants, obj_DiffDelta_implements, 0},
  { &(obj_type_DiffStats), REG_OBJECT, obj_DiffStats_pub_funcs, obj_DiffStats_methods, obj_DiffStats_metas, obj_DiffStats_bases, obj_DiffStats_fields, obj_DiffStats_constants, obj_DiffStats_implements, 0},
  { &(obj_type_DiffFile), REG_OBJECT, obj_DiffFile_pub_funcs, obj_DiffFile_methods, obj_DiffFile_metas, obj_DiffFile_bases, obj_DiffFile_fields, obj_DiffFile_constants, obj_DiffFile_implements, 0},
  { &(obj_type_Diff), REG_OBJECT, obj_Diff_pub_funcs, obj_Diff_methods, obj_Diff_metas, obj_Diff_bases, obj_Diff_fields, obj_Diff_constants, obj_Diff_implements, 0},
  { &(obj_type_StatusOptions), REG_OBJECT, obj_StatusOptions_pub_funcs, obj_StatusOptions_methods, obj_StatusOptions_metas, obj_StatusOptions_bases, obj_StatusOptions_fields, obj_StatusOptions_constants, obj_StatusOptions_implements, 0},
  { &(obj_type_StatusList), REG_OBJECT, obj_StatusList_pub_funcs, obj_StatusList_methods, obj_StatusList_metas, obj_StatusList_bases, obj_StatusList_fields, obj_StatusList_constants, obj_StatusList_implements, 0},
  { &(obj_type_StatusEntry), REG_OBJECT, obj_StatusEntry_pub_funcs, obj_StatusEntry_methods, obj_StatusEntry_metas, obj_StatusEntry_bases, obj_StatusEntry_fields, obj_StatusEntry_constants, obj_StatusEntry_implements, 0},
  { &(obj_type_StrArray), REG_OBJECT, obj_StrArray_pub_funcs, obj_StrArray_methods, obj_StrArray_metas, obj_StrArray_bases, obj_StrArray_fields, obj_StrArray_constants, obj_StrArray_implements, 0},
  { &(obj_type_Repository), REG_OBJECT, obj_Repository_pub_funcs, obj_Repository_methods, obj_Repository_metas, obj_Repository_bases, obj_Repository_fields, obj_Repository_constants, obj_Repository_implements, 0},
  { &(obj_type_Config), REG_OBJECT, obj_Config_pub_funcs, obj_Config_methods, obj_Config_metas, obj_Config_bases, obj_Config_fields, obj_Config_constants, obj_Config_implements, 0},
  { &(obj_type_OdbObject), REG_OBJECT, obj_OdbObject_pub_funcs, obj_OdbObject_methods, obj_OdbObject_metas, obj_OdbObject_bases, obj_OdbObject_fields, obj_OdbObject_constants, obj_OdbObject_implements, 0},
  { &(obj_type_OID), REG_OBJECT, obj_OID_pub_funcs, obj_OID_methods, obj_OID_metas, obj_OID_bases, obj_OID_fields, obj_OID_constants, obj_OID_implements, 0},
  { &(obj_type_OID_Shorten), REG_OBJECT, obj_OID_Shorten_pub_funcs, obj_OID_Shorten_methods, obj_OID_Shorten_metas, obj_OID_Shorten_bases, obj_OID_Shorten_fields, obj_OID_Shorten_constants, obj_OID_Shorten_implements, 0},
  { &(obj_type_ODB), REG_OBJECT, obj_ODB_pub_funcs, obj_ODB_methods, obj_ODB_metas, obj_ODB_bases, obj_ODB_fields, obj_ODB_constants, obj_ODB_implements, 0},
  { &(obj_type_ODBBackend), REG_OBJECT, obj_ODBBackend_pub_funcs, obj_ODBBackend_methods, obj_ODBBackend_metas, obj_ODBBackend_bases, obj_ODBBackend_fields, obj_ODBBackend_constants, obj_ODBBackend_implements, 0},
  { &(obj_type_Index), REG_OBJECT, obj_Index_pub_funcs, obj_Index_methods, obj_Index_metas, obj_Index_bases, obj_Index_fields, obj_Index_constants, obj_Index_implements, 0},
  { &(obj_type_IndexEntry), REG_OBJECT, obj_IndexEntry_pub_funcs, obj_IndexEntry_methods, obj_IndexEntry_metas, obj_IndexEntry_bases, obj_IndexEntry_fields, obj_IndexEntry_constants, obj_IndexEntry_implements, 0},
  { &(obj_type_IndexEntryUnmerged), REG_OBJECT, obj_IndexEntryUnmerged_pub_funcs, obj_IndexEntryUnmerged_methods, obj_IndexEntryUnmerged_metas, obj_IndexEntryUnmerged_bases, obj_IndexEntryUnmerged_fields, obj_IndexEntryUnmerged_constants, obj_IndexEntryUnmerged_implements, 0},
  { &(obj_type_Object), REG_OBJECT, obj_Object_pub_funcs, obj_Object_methods, obj_Object_metas, obj_Object_bases, obj_Object_fields, obj_Object_constants, obj_Object_implements, 0},
  { &(obj_type_Blob), REG_OBJECT, obj_Blob_pub_funcs, obj_Blob_methods, obj_Blob_metas, obj_Blob_bases, obj_Blob_fields, obj_Blob_constants, obj_Blob_implements, 0},
  { &(obj_type_Signature), REG_OBJECT, obj_Signature_pub_funcs, obj_Signature_methods, obj_Signature_metas, obj_Signature_bases, obj_Signature_fields, obj_Signature_constants, obj_Signature_implements, 0},
  { &(obj_type_Commit), REG_OBJECT, obj_Commit_pub_funcs, obj_Commit_methods, obj_Commit_metas, obj_Commit_bases, obj_Commit_fields, obj_Commit_constants, obj_Commit_implements, 0},
  { &(obj_type_Tree), REG_OBJECT, obj_Tree_pub_funcs, obj_Tree_methods, obj_Tree_metas, obj_Tree_bases, obj_Tree_fields, obj_Tree_constants, obj_Tree_implements, 0},
  { &(obj_type_TreeEntry), REG_OBJECT, obj_TreeEntry_pub_funcs, obj_TreeEntry_methods, obj_TreeEntry_metas, obj_TreeEntry_bases, obj_TreeEntry_fields, obj_TreeEntry_constants, obj_TreeEntry_implements, 0},
  { &(obj_type_Tag), REG_OBJECT, obj_Tag_pub_funcs, obj_Tag_methods, obj_Tag_metas, obj_Tag_bases, obj_Tag_fields, obj_Tag_constants, obj_Tag_implements, 0},
  { &(obj_type_RevWalk), REG_OBJECT, obj_RevWalk_pub_funcs, obj_RevWalk_methods, obj_RevWalk_metas, obj_RevWalk_bases, obj_RevWalk_fields, obj_RevWalk_constants, obj_RevWalk_implements, 0},
  { &(obj_type_Reference), REG_OBJECT, obj_Reference_pub_funcs, obj_Reference_methods, obj_Reference_metas, obj_Reference_bases, obj_Reference_fields, obj_Reference_constants, obj_Reference_implements, 0},
  {NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0}
};





#if LUAJIT_FFI
static const ffi_export_symbol git2_ffi_export[] = {
  {NULL, { NULL } }
};
#endif




static const luaL_Reg submodule_libs[] = {
  {NULL, NULL}
};



static void create_object_instance_cache(lua_State *L) {
	lua_pushlightuserdata(L, obj_udata_weak_ref_key); /* key for weak table. */
	lua_rawget(L, LUA_REGISTRYINDEX);  /* check if weak table exists already. */
	if(!lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop weak table. */
		return;
	}
	lua_pop(L, 1); /* pop nil. */
	/* create weak table for object instance references. */
	lua_pushlightuserdata(L, obj_udata_weak_ref_key); /* key for weak table. */
	lua_newtable(L);               /* weak table. */
	lua_newtable(L);               /* metatable for weak table. */
	lua_pushliteral(L, "__mode");
	lua_pushliteral(L, "v");
	lua_rawset(L, -3);             /* metatable.__mode = 'v'  weak values. */
	lua_setmetatable(L, -2);       /* add metatable to weak table. */
	lua_rawset(L, LUA_REGISTRYINDEX);  /* create reference to weak table. */
}

LUA_NOBJ_API int luaopen_git2(lua_State *L) {
	const reg_sub_module *reg = reg_sub_modules;
	const luaL_Reg *submodules = submodule_libs;
	int priv_table = -1;

	/* register interfaces */
	obj_register_interfaces(L, obj_interfaces);

	/* private table to hold reference to object metatables. */
	lua_newtable(L);
	priv_table = lua_gettop(L);
	lua_pushlightuserdata(L, obj_udata_private_key);
	lua_pushvalue(L, priv_table);
	lua_rawset(L, LUA_REGISTRYINDEX);  /* store private table in registry. */

	/* create object cache. */
	create_object_instance_cache(L);

	/* module table. */
#if REG_MODULES_AS_GLOBALS
	luaL_register(L, "git2", git2_function);
#else
	lua_newtable(L);
	nobj_setfuncs(L, git2_function, 0);
#endif

	/* register module constants. */
	obj_type_register_constants(L, git2_constants, -1, 0);

	for(; submodules->func != NULL ; submodules++) {
		lua_pushcfunction(L, submodules->func);
		lua_pushstring(L, submodules->name);
		lua_call(L, 1, 0);
	}

	/* register objects */
	for(; reg->type != NULL ; reg++) {
		lua_newtable(L); /* create public API table for object. */
		lua_pushvalue(L, -1); /* dup. object's public API table. */
		lua_setfield(L, -3, reg->type->name); /* module["<object_name>"] = <object public API> */
#if REG_OBJECTS_AS_GLOBALS
		lua_pushvalue(L, -1);                 /* dup value. */
		lua_setglobal(L, reg->type->name);    /* global: <object_name> = <object public API> */
#endif
		obj_type_register(L, reg, priv_table);
	}

#if LUAJIT_FFI
	if(nobj_check_ffi_support(L)) {
		nobj_try_loading_ffi(L, "git2.nobj.ffi.lua", git2_ffi_lua_code,
			git2_ffi_export, priv_table);
	}
#endif

  git_libgit2_init();



	return 1;
}


