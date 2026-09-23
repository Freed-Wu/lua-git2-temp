-- src/tree.nobj.lua

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

object "Tree" {
    basetype "git_treewalk_mode" "integer",
    c_source [[
typedef git_tree Tree;
]],
    extends "Object",
    constructor "lookup" {
        c_call { "GitError", "err" } "git_tree_lookup"
            { "Tree *", "&this>1", "Repository *", "repo", "OID", "&id" },
    },
    constructor "lookup_prefix" {
        c_call { "GitError", "err" } "git_tree_lookup_prefix"
            { "Tree *", "&this>1", "Repository *", "repo", "OID", "&id", "size_t", "len" },
    },
    method "entrycount" {
        c_method_call "size_t"  "git_tree_entrycount" {}
    },
    method "entry_byname" {
        c_method_call "const TreeEntry *"  "git_tree_entry_byname" { "const char *", "filename" }
    },
    method "entry_byindex" {
        c_method_call "const TreeEntry *"  "git_tree_entry_byindex" { "size_t", "index" }
    },
    method "entry_byid" {
        c_method_call "const TreeEntry *"  "git_tree_entry_byid" { "OID", "&id" }
    },

    -- git_tree_walk - simplified version with PRE order
    c_source [[
  typedef struct {
    lua_State *L;
    int callback_ref;
  } walk_data;

  int walk_trampoline(const char *root, const git_tree_entry *entry, void *payload) {
    walk_data *data = (walk_data *)payload;
    lua_State *L = data->L;
    int type_val = lua_type(L, -1);
    if (type_val != LUA_TTABLE) {
      lua_pop(L, 1);
      return 0;
    }
    lua_rawgeti(L, LUA_REGISTRYINDEX, data->callback_ref);
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        return 0;
    }
    lua_pushstring(L, root);
    if (entry == NULL) {
        lua_pushnil(L);
    } else {
        obj_type_TreeEntry_push(L, entry, 0);
    }
    lua_pushlightuserdata(L, payload);
    if (lua_pcall(L, 3, 1, 0) != 0) {
        lua_pop(L, 1);
        return 0;
    }
    int ret = lua_tointeger(L, -1);
    lua_pop(L, 1);
    return ret;
  }
  ]],

  method "walk" {
        c_source [[

  walk_data data;
  data.L = L;
  lua_pushvalue(L, 2);
  data.callback_ref = luaL_ref(L, LUA_REGISTRYINDEX);
  int err = git_tree_walk(${this}, GIT_TREEWALK_PRE, walk_trampoline, &data);
  luaL_unref(L, LUA_REGISTRYINDEX, data.callback_ref);
  ${err} = err;
  return err;
]],
        var_in { "void *", "callback" },
        var_in { "void *", "payload" },
        var_out { "GitError", "err" },
    },

    -- git_tree_id
    method "id" {
        c_method_call "const OID *" "git_tree_id" {}
    },


    -- git_tree_dup (create a copy of the tree)
    method "dup" {
        c_call { "GitError", "err" } "git_tree_dup"
            { "Tree *", "&this>1", "Tree *", "source" },
    },

    -- git_tree_owner
    method "owner" {
        c_method_call "Repository *" "git_tree_owner" {}
    },
}
