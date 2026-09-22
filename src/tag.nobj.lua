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

object "Tag" {
	c_source [[
typedef git_tag Tag;
]],
	extends "Object",
	constructor "lookup" {
		c_call { "GitError", "err" } "git_tag_lookup"
			{ "Tag *", "&this>1", "Repository *", "repo", "OID", "&id" },
	},
	destructor "free" {
		c_method_call "void" "git_tag_free" {},
	},
	method "id" {
		c_method_call { "OID", "*id" } "git_tag_id" {},
	},
	method "target" {
		c_call "GitError" "git_tag_target" { "Object *", "&out>1", "Tag *", "this" }
	},
	method "target_id" {
		c_method_call { "OID", "*id" } "git_tag_target_id" {},
	},
	method "target_type" {
		c_method_call { "git_otype", "(otype)" } "git_tag_target_type" {},
		c_call { "const char *", "type" } "git_object_type2string" { "git_otype", "otype" },
	},
	method "name" {
		c_method_call "const char *" "git_tag_name" {}
	},
	method "tagger" {
		c_method_call "const Signature *" "git_tag_tagger" {}
	},
	method "message" {
		c_method_call "const char *" "git_tag_message" {}
	},
	method "peel" {
		c_call "GitError" "git_tag_peel" { "Object *", "&out>1", "Tag *", "this" }
	},
	method "owner" {
		c_method_call "Repository *" "git_tag_owner" {}
	},
}

-- Tag creation and management functions (module-level)
c_function "tag_create" {
	var_in{ "Repository *", "repo" },
	var_in{ "const char *", "tag_name" },
	var_in{ "Object *", "target" },
	var_in{ "Signature *", "tagger" },
	var_in{ "const char *", "message" },
	var_in{ "int", "force" },
	var_out{ "OID", "oid_out" },
	var_out{ "GitError", "err" },
	c_source[[
	${err} = git_tag_create(&${oid_out}, ${repo}, ${tag_name}, ${target}, ${tagger}, ${message}, ${force});
]],
}

c_function "tag_create_frombuffer" {
	var_in{ "Repository *", "repo" },
	var_in{ "const char *", "buffer" },
	var_in{ "int", "force" },
	var_out{ "OID", "oid_out" },
	var_out{ "GitError", "err" },
	c_source[[
	${err} = git_tag_create_frombuffer(&${oid_out}, ${repo}, ${buffer}, ${force});
]],
}

c_function "tag_create_lightweight" {
	var_in{ "Repository *", "repo" },
	var_in{ "const char *", "tag_name" },
	var_in{ "Object *", "target" },
	var_in{ "int", "force" },
	var_out{ "OID", "oid_out" },
	var_out{ "GitError", "err" },
	c_source[[
	${err} = git_tag_create_lightweight(&${oid_out}, ${repo}, ${tag_name}, ${target}, ${force});
]],
}

c_function "tag_delete" {
	var_in{ "Repository *", "repo" },
	var_in{ "const char *", "tag_name" },
	var_out{ "GitError", "err" },
	c_source[[
	${err} = git_tag_delete(${repo}, ${tag_name});
]],
}

c_function "tag_list" {
	var_in{ "Repository *", "repo" },
	var_out{ "StrArray *", "array" },
	var_out{ "GitError", "err" },
	c_source "pre" [[
	git_strarray tmp_array = { .strings = NULL, .count = 0 };
]],
	c_source[[
	${array} = obj_type_StrArray_push(L, &tmp_array);
	${err} = git_tag_list(${array}, ${repo});
	if(${err} == GIT_OK) {
		return 1;
	} else {
		lua_pop(L, 1);
		${array} = NULL;
	}
]],
}

c_function "tag_list_match" {
	var_in{ "Repository *", "repo" },
	var_in{ "const char *", "pattern" },
	var_out{ "StrArray *", "array" },
	var_out{ "GitError", "err" },
	c_source "pre" [[
	git_strarray tmp_array = { .strings = NULL, .count = 0 };
]],
	c_source[[
	${array} = obj_type_StrArray_push(L, &tmp_array);
	${err} = git_tag_list_match(${array}, ${pattern}, ${repo});
	if(${err} == GIT_OK) {
		return 1;
	} else {
		lua_pop(L, 1);
		${array} = NULL;
	}
]],
}
