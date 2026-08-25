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

object "Diff" {
	c_source [[
typedef git_diff Diff;
]],
	constants {
		FORMAT_PATCH = 1,     -- GIT_DIFF_FORMAT_PATCH: unified "git diff -u" output
		FORMAT_PATCH_HEADER = 2,
		FORMAT_RAW = 3,
		FORMAT_NAME_ONLY = 4,
		FORMAT_NAME_STATUS = 5,
	},
	constructor "index_to_workdir" {
		c_call { "GitError", "err" } "git_diff_index_to_workdir" { "Diff *", "&this>1", "Repository *", "repo", "Index *", "index", "DiffOptions *", "opts" },
	},
	constructor "tree_to_index" {
		c_call { "GitError", "err" } "git_diff_tree_to_index" { "Diff *", "&this>1", "Repository *", "repo", "Tree *", "tree", "Index *", "index", "DiffOptions *", "opts" },
	},
	constructor "tree_to_workdir_with_index" {
		c_call { "GitError", "err" } "git_diff_tree_to_workdir_with_index" { "Diff *", "&this>1", "Repository *", "repo", "Tree *", "tree", "DiffOptions *", "opts" },
	},
	constructor "tree_to_tree" {
		c_call { "GitError", "err" } "git_diff_tree_to_tree" { "Diff *", "&this>1", "Repository *", "repo", "Tree *", "tree", "Tree *", "tree", "DiffOptions *", "opts" },
	},
	destructor {
		c_method_call "void" "git_diff_free" {}
	},
	method "num" {
		c_method_call { "size_t", "count" } "git_diff_num_deltas" {},
	},
	method "find_similar" {
		c_method_call { "GitError", "err" } "git_diff_find_similar" { "DiffFindOptions *", "opts" },
	},
	-- Render the diff to a unified "git diff -u" string. Exactly mirrors what
	-- `git diff` prints, including the `diff --git a/ b/`, `index <oid>..<oid>`,
	-- `--- a/`, `+++ b/`, and `@@ ... @@` sections. On success returns the text,
	-- on error returns nil.
	method "to_buf" {
		var_in { "unsigned int", "format" },
		var_out { "<any>", "buf" },
		c_source [[
		git_buf out = GIT_BUF_INIT;
		int rc = git_diff_to_buf(&out, ${this}, ${format});
		if (rc < 0) {
			if (out.ptr) git_buf_dispose(&out);
			lua_pushnil(L);
			${buf} = NULL;
		} else {
			lua_pushlstring(L, out.ptr ? out.ptr : "", out.size);
			git_buf_dispose(&out);
			${buf} = NULL;
		}
	]],
	},
}
