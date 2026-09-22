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

-- Remote operations (git_remote_*)
-- Functions for managing remote repositories

object "Remote" {
	c_source [[
#include <git2/sys/remote.h>
typedef git_remote Remote;
]],

	-- Destructor
	destructor {
		c_method_call "void" "git_remote_free" {}
	},

	-- Get the remote's name
	method "name" {
		c_method_call "const char *" "git_remote_name" {}
	},

	-- Get the remote's URL
	method "url" {
		c_method_call "const char *" "git_remote_url" {}
	},

	-- Get the remote's push URL
	method "pushurl" {
		c_method_call "const char *" "git_remote_pushurl" {}
	},

	-- Get the remote's owner repository
	method "owner" {
		c_method_call "!Repository *" "git_remote_owner" {}
	},

	-- Check if the remote is connected
	method "connected" {
		c_method_call "int" "git_remote_connected" {}
	},

	-- Disconnect from the remote
	method "disconnect" {
		c_method_call "GitError" "git_remote_disconnect" {}
	},

	-- List remote heads (ls)
	method "ls" {
		var_out{ "<any>", "heads" },
		var_out{ "GitError", "err" },
		c_source "pre" [[
		const git_remote_head **heads = NULL;
		size_t num_heads = 0;
	]],
		c_source[[
		${err} = git_remote_ls(&heads, &num_heads, ${this});
		if (${err} != GIT_OK) {
			lua_pushnil(L);
			return 2;
		}

		lua_newtable(L);
		for (size_t i = 0; i < num_heads; i++) {
			const git_remote_head *head = heads[i];

			lua_pushstring(L, head->name);
			lua_rawseti(L, -2, (int)(i + 1));
		}
		]],
	},

	-- Get the number of refspecs
	method "refspec_count" {
		c_method_call "size_t" "git_remote_refspec_count" {}
	},

	-- Get fetch refspecs as table
	method "fetchspec" {
		var_out{ "<any>", "refs" },
		var_out{ "GitError", "err" },
		c_source[[
		git_strarray refs = { NULL, 0 };
		${err} = git_remote_get_fetch_refspecs(&refs, ${this});
		if (${err} != GIT_OK) {
			lua_pushnil(L);
			return 2;
		}

		lua_newtable(L);
		for (unsigned int i = 0; i < refs.count; i++) {
			lua_pushstring(L, refs.strings[i]);
			lua_rawseti(L, -2, (int)(i + 1));
		}

		git_strarray_free(&refs);
		]],
	},

	-- Get push refspecs as table
	method "pushspec" {
		var_out{ "<any>", "refs" },
		var_out{ "GitError", "err" },
		c_source[[
		git_strarray refs = { NULL, 0 };
		${err} = git_remote_get_push_refspecs(&refs, ${this});
		if (${err} != GIT_OK) {
			lua_pushnil(L);
			return 2;
		}

		lua_newtable(L);
		for (unsigned int i = 0; i < refs.count; i++) {
			lua_pushstring(L, refs.strings[i]);
			lua_rawseti(L, -2, (int)(i + 1));
		}

		git_strarray_free(&refs);
		]],
	},

	-- Set URL in configuration
	method "set_url" {
		var_in{ "const char *", "url" },
		var_out{ "GitError", "err" },
		c_source[[
		git_repository *repo = git_remote_owner(${this});
		const char *name = git_remote_name((const Remote *)${this});
		${err} = git_remote_set_url(repo, name, ${url});
		]],
	},

	-- Set push URL in configuration
	method "set_pushurl" {
		var_in{ "const char *", "url" },
		var_out{ "GitError", "err" },
		c_source[[
		git_repository *repo = git_remote_owner(${this});
		const char *name = git_remote_name((const Remote *)${this});
		${err} = git_remote_set_pushurl(repo, name, ${url});
		]],
	},
}

-- Constants for direction
constants {
	DIRECTION_FETCH = 0,  -- GIT_DIRECTION_FETCH
	DIRECTION_PUSH  = 1,  -- GIT_DIRECTION_PUSH
}

-- Autotag constants
constants {
	AUTOTAG_UNSPECIFIED = 0,  -- GIT_REMOTE_DOWNLOAD_TAGS_UNSPECIFIED
	AUTOTAG_AUTO        = 1,  -- GIT_REMOTE_DOWNLOAD_TAGS_AUTO
	AUTOTAG_NONE        = 2,  -- GIT_REMOTE_DOWNLOAD_TAGS_NONE
	AUTOTAG_ALL         = 3,  -- GIT_REMOTE_DOWNLOAD_TAGS_ALL
}

-- Module-level functions for creating and listing remotes

c_function "remote_list" {
	var_in{ "Repository *", "repo" },
	var_out{ "<any>", "names" },
	var_out{ "GitError", "err" },
	c_source[[
	git_strarray names = { NULL, 0 };
	${err} = git_remote_list(&names, ${repo});
	if (${err} != GIT_OK) {
		lua_pushnil(L);
		return 2;
	}

	lua_newtable(L);
	for (unsigned int i = 0; i < names.count; i++) {
		lua_pushstring(L, names.strings[i]);
		lua_rawseti(L, -2, (int)(i + 1));
	}

	git_strarray_free(&names);
	]],
}

c_function "remote_new" {
	var_in{ "Repository *", "repo" },
	var_in{ "const char *", "name" },
	var_in{ "const char *", "url" },
	var_out{ "!Remote *", "remote" },
	var_out{ "GitError", "err" },
	c_source[[
	${err} = git_remote_create(&${remote}, ${repo}, ${name}, ${url});
	]],
}

c_function "remote_load" {
	var_in{ "Repository *", "repo" },
	var_in{ "const char *", "name" },
	var_out{ "!Remote *", "remote" },
	var_out{ "GitError", "err" },
	c_source[[
	${err} = git_remote_lookup(&${remote}, ${repo}, ${name});
	]],
}

c_function "remote_add" {
	var_in{ "Repository *", "repo" },
	var_in{ "const char *", "name" },
	var_in{ "const char *", "url" },
	var_in{ "const char *", "fetch" },
	var_out{ "!Remote *", "remote" },
	var_out{ "GitError", "err" },
	c_source[[
	const char *fetchspec = lua_isstring(L, 4) ? ${fetch} : NULL;
	${err} = git_remote_create_with_fetchspec(&${remote}, ${repo}, ${name}, ${url}, fetchspec);
	]],
}

c_function "remote_save" {
	-- In libgit2, remotes are saved automatically when created via git_remote_create* functions
	-- This function returns success immediately for API compatibility
	var_out{ "int", "result" },
	c_source[[
	result_idx1 = GIT_OK;
	]],
}

c_function "remote_connect" {
	var_in{ "Remote *", "remote" },
	var_in{ "int", "direction" },
	var_out{ "GitError", "err" },
	c_source[[
	${err} = git_remote_connect(${remote}, (git_direction)${direction},
	                              NULL, NULL, NULL);
	]],
}

c_function "remote_download" {
	var_in{ "Remote *", "remote" },
	var_out{ "GitError", "err" },
	c_source[[
	${err} = git_remote_download(${remote}, NULL, NULL);
	]],
}

c_function "remote_update_tips" {
	var_in{ "Remote *", "remote" },
	var_in{ "unsigned int", "update_flags" },
	var_in{ "int", "download_tags" },
	var_in{ "const char *", "reflog_message" },
	var_out{ "GitError", "err" },
	c_source[[
	git_remote_autotag_option_t tags = (git_remote_autotag_option_t)${download_tags};
	const char *msg = lua_isstring(L, 5) ? ${reflog_message} : NULL;
	${err} = git_remote_update_tips(${remote}, NULL, ${update_flags}, tags, msg);
	]],
}

c_function "remote_set_fetchspec" {
	var_in{ "Repository *", "repo" },
	var_in{ "const char *", "name" },
	var_in{ "const char *", "refspec" },
	var_out{ "GitError", "err" },
	c_source[[
	${err} = git_remote_add_fetch(${repo}, ${name}, ${refspec});
	]],
}

c_function "remote_set_pushspec" {
	var_in{ "Repository *", "repo" },
	var_in{ "const char *", "name" },
	var_in{ "const char *", "refspec" },
	var_out{ "GitError", "err" },
	c_source[[
	${err} = git_remote_add_push(${repo}, ${name}, ${refspec});
	]],
}

c_function "remote_valid_url" {
	var_in{ "const char *", "url" },
	var_out{ "int", "valid" },
	var_out{ "GitError", "err" },
	c_source[[
	${err} = git_remote_name_is_valid(&${valid}, ${url});
	]],
}

c_function "remote_supported_url" {
	var_in{ "const char *", "url" },
	var_out{ "int", "supported" },
	c_source[[
	/* Check for common supported URL protocols */
	const char *proto = ${url};
	const char *colon = strchr(proto, ':');
	int is_supported = 0;

	if (colon) {
		size_t proto_len = (size_t)(colon - proto);

		/* Check for supported protocols: file, ssh, git, https, http */
		if ((proto_len == 4 && strncmp(proto, "file", 4) == 0) ||
		    (proto_len == 3 && strncmp(proto, "ssh", 3) == 0) ||
		    (proto_len == 3 && strncmp(proto, "git", 3) == 0) ||
		    (proto_len == 5 && strncmp(proto, "https", 5) == 0) ||
		    (proto_len == 4 && strncmp(proto, "http", 4) == 0)) {
			is_supported = 1;
		}
	}

	supported_idx1 = is_supported;
	]],
}
