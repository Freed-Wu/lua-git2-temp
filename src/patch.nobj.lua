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

object "Patch" {
	c_source [[
typedef git_patch Patch;
]],
	constructor "from_diff" {
		c_call { "GitError", "err" } "git_patch_from_diff"
			{ "Patch *", "&this>1", "Diff *", "diff", "size_t", "idx" },
	},
	constructor "from_buffers" {
		c_call { "GitError", "err" } "git_patch_from_buffers"
			{ "Patch *", "&this>1",
				"const char *", "old_buffer", "size_t", "old_len",
				"const char *", "old_path",
				"const char *", "new_buffer", "size_t", "new_len",
				"const char *", "new_path",
				"DiffOptions *", "opts" },
	},
	destructor {
		c_method_call "void" "git_patch_free" {}
	},
	method "num_hunks" {
		c_method_call { "size_t", "count" } "git_patch_num_hunks" {},
	},
	method "num_lines" {
		c_method_call { "size_t", "count" } "git_patch_num_lines_in_hunk"
			{ "size_t", "hunk_idx" },
	},
	method "line_origin" {
		var_in { "size_t", "hunk_idx" },
		var_in { "size_t", "line_idx" },
		var_out { "<any>", "origin" },
		c_source [[
	const git_diff_line *line;
	int rc = git_patch_get_line_in_hunk(&line, ${this}, ${hunk_idx}, ${line_idx});
	if (rc < 0) {
		lua_pushnil(L);
	} else {
		char buf[2] = { line->origin, '\0' };
		lua_pushstring(L, buf);
	}
]],
	},
}
