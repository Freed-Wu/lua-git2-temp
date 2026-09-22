-- Copyright (c) 2010-2012 by Robert G. Jakabosky <bobby@sharedrealm.com>
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
-- THE SOFTWARE.

object "TreeBuilder" {
	c_source [[
typedef git_treebuilder TreeBuilder;

/* Payload struct for Lua filter callback */
typedef struct {
	lua_State *L;
	int ref;
} treebuilder_filter_payload;

/* C callback wrapper for Lua filter function */
static int treebuilder_filter_cb(const git_tree_entry *entry, void *payload) {
	treebuilder_filter_payload *p = (treebuilder_filter_payload *)payload;
	int result;
	git_tree_entry *dup_entry;

	/* Duplicate the entry so Lua owns it */
	if (git_tree_entry_dup(&dup_entry, entry) != 0) {
		return 0;
	}

	lua_rawgeti(p->L, LUA_REGISTRYINDEX, p->ref);
	obj_type_TreeEntry_push(p->L, dup_entry, OBJ_UDATA_FLAG_OWN);
	lua_call(p->L, 1, 1);
	result = lua_toboolean(p->L, -1);
	lua_pop(p->L, 1);
	return result;
}
]],
	constructor "create" {
		c_source [[
	Repository *repo_idx1;
	const git_tree *source_idx2 = NULL;
	GitError err_idx1 = GIT_OK;

	repo_idx1 = obj_type_Repository_check(L, 1);
	if (!lua_isnil(L, 2)) {
		source_idx2 = obj_type_Tree_check(L, 2);
	}

	err_idx1 = git_treebuilder_new(&(this_idx1), repo_idx1, source_idx2);
	if (GIT_OK != err_idx1) {
		lua_pushnil(L);
		error_code__GitError__push(L, err_idx1);
		return 2;
	}
	obj_type_TreeBuilder_push(L, this_idx1, OBJ_UDATA_FLAG_OWN);
	lua_pushnil(L);
	return 2;
]],
	},
	destructor {
		c_method_call "void" "git_treebuilder_free" {}
	},
	method "clear" {
		c_method_call "GitError" "git_treebuilder_clear" {}
	},
	method "entrycount" {
		c_method_call "size_t" "git_treebuilder_entrycount" {}
	},
	method "insert" {
		c_source [[
	size_t filename_len_idx2;
	const char * filename_idx2;
	OID id_idx3;
	unsigned int filemode_idx4;
	GitError err_idx1 = GIT_OK;
	filename_idx2 = luaL_checklstring(L, 2, &(filename_len_idx2));
	id_idx3 = obj_type_OID_check(L, 3);
	filemode_idx4 = luaL_checkinteger(L, 4);
	err_idx1 = git_treebuilder_insert(NULL, this_idx1, filename_idx2, &(id_idx3), filemode_idx4);
	if (GIT_OK != err_idx1) {
		lua_pushnil(L);
		error_code__GitError__push(L, err_idx1);
	} else {
		lua_pushboolean(L, 1);
		lua_pushnil(L);
	}
	return 2;
]],
	},
	method "remove" {
		c_method_call "GitError" "git_treebuilder_remove" { "const char *", "filename" }
	},
	method "get" {
		c_method_call "const TreeEntry *" "git_treebuilder_get" { "const char *", "filename" }
	},
	method "write" {
		c_source [[
	OID id_idx2;
	GitError err_idx1 = GIT_OK;
	err_idx1 = git_treebuilder_write(&(id_idx2), this_idx1);
	if (GIT_OK != err_idx1) {
		lua_pushnil(L);
		error_code__GitError__push(L, err_idx1);
	} else {
		obj_type_OID_push(L, id_idx2);
		lua_pushnil(L);
	}
	return 2;
]],
	},
	method "filter" {
		c_source [[
	GitError err_idx1 = GIT_OK;
	treebuilder_filter_payload payload;
	luaL_checktype(L, 2, LUA_TFUNCTION);
	lua_pushvalue(L, 2);
	payload.ref = luaL_ref(L, LUA_REGISTRYINDEX);
	payload.L = L;
	err_idx1 = git_treebuilder_filter(this_idx1, treebuilder_filter_cb, &payload);
	luaL_unref(L, LUA_REGISTRYINDEX, payload.ref);
	error_code__GitError__push(L, err_idx1);
	return 1;
]],
	},
}
