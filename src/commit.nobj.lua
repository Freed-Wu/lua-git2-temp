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

object "Commit" {
	c_source [[
typedef git_commit Commit;
]],
	extends "Object",
	constructor "lookup" {
		c_call {"GitError", "err"} "git_commit_lookup"
			{ "Commit *", "&this", "Repository *", "repo", "OID", "&id" },
	},
	c_function "create" {
		var_in{ "Repository *", "repo" },
		var_in{ "const char *", "update_ref" },
		var_in{ "Signature *", "author" },
		var_in{ "Signature *", "committer" },
		var_in{ "const char *", "message_encoding" },
		var_in{ "const char *", "message" },
		var_in{ "Tree *", "tree" },
		var_out{ "OID", "oid" },
		var_out{"GitError", "err"},
		c_source [[
	int parent_count = 0;
	int parent_start_idx = 8;
#if LIBGIT2_VER_MAJOR == 1 && LIBGIT2_VER_MINOR == 8
	git_commit **parents;
#else
	const git_commit **parents;
#endif
	int n;

	/* count parents from index 8 onwards */
	parent_count = lua_gettop(L) - parent_start_idx + 1;
	if (parent_count < 0) {
		parent_count = 0;
	}

	/* validate all parent commits */
	for(n = 0; n < parent_count; n++) {
		obj_type_Commit_check(L, parent_start_idx + n);
	}

	/* allocate parent array */
	if (parent_count > 0) {
		parents = malloc(parent_count * sizeof(git_commit *));
		for(n = 0; n < parent_count; n++) {
			parents[n] = obj_type_Commit_check(L, parent_start_idx + n);
		}
	} else {
		parents = NULL;
	}

	${err} = git_commit_create(&(${oid}), ${repo}, ${update_ref},
		${author}, ${committer}, ${message_encoding}, ${message},
		${tree}, parent_count, parents);

	/* free parent array */
	if (parents != NULL) {
		free(parents);
	}
]],
	},
	method "id" {
		c_method_call { "OID", "*id" }  "git_commit_id" {}
	},
	method "message_encoding" {
		c_method_call "const char *"  "git_commit_message_encoding" {}
	},
	method "message" {
		c_method_call "const char *"  "git_commit_message" {}
	},
	method "time" {
		c_method_call "git_time_t"  "git_commit_time" {}
	},
	method "time_offset" {
		c_method_call "int"  "git_commit_time_offset" {}
	},
	method "committer" {
		c_method_call "const Signature *"  "git_commit_committer" {}
	},
	method "author" {
		c_method_call "const Signature *"  "git_commit_author" {}
	},
	method "tree" {
		c_call "GitError" "git_commit_tree" { "!Tree *", "&tree>1", "Commit *", "this" }
	},
	method "parentcount" {
		c_method_call "unsigned int"  "git_commit_parentcount" {}
	},
	method "parent" {
		c_call "GitError" "git_commit_parent"
			{ "Commit *", "&parent>1", "Commit *", "this", "unsigned int", "n" }
	},
}
