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

object "Blame" {
	c_source [[
#include <git2/blame.h>
typedef git_blame Blame;
]],
	constructor "file" {
		c_call { "GitError", "err" } "git_blame_file"
			{ "Blame *", "&this>1", "Repository *", "repo", "const char *", "path", "BlameOptions *", "opts" },
	},
	destructor {
		c_method_call "void" "git_blame_free" {}
	},
	method "count" {
		c_method_call { "size_t", "count" } "git_blame_hunkcount" {}
	},
	method "linecount" {
		var_out { "size_t", "count" },
		c_source [[
		size_t hunk_count = git_blame_hunkcount(${this});
		size_t idx;
		${count} = 0;

		for (idx = 0; idx < hunk_count; idx++) {
			const git_blame_hunk *hunk = git_blame_get_hunk_byindex(${this}, idx);
			if (hunk != NULL) {
				size_t last_line = hunk->final_start_line_number + hunk->lines_in_hunk - 1;
				if (last_line > ${count}) {
					${count} = last_line;
				}
			}
		}
]]
	},
	method "buffer" {
		c_source [[
		size_t buffer_len_idx2;
		const char * buffer_idx2;
		int blame_flags_idx1 = OBJ_UDATA_FLAG_OWN;
		Blame * blame_idx1;
		GitError err_idx2 = GIT_OK;
		buffer_idx2 = luaL_checklstring(L,2,&(buffer_len_idx2));
		err_idx2 = git_blame_buffer(&(blame_idx1), this_idx1, buffer_idx2, buffer_len_idx2);
		if(!(GIT_OK != err_idx2)) {
			obj_type_Blame_push(L, blame_idx1, blame_flags_idx1);
		} else {
			lua_pushnil(L);
		}
		error_code__GitError__push(L, err_idx2);
		return 2;
]]
	},
	method "get_hunk_byindex" {
		var_in { "size_t", "index" },
		var_out { "BlameHunk *", "hunk" },
		c_source [[
		${hunk} = (BlameHunk *)git_blame_get_hunk_byindex(${this}, ${index});
]]
	},
	method "get_hunk_byline" {
		var_in { "size_t", "linenum" },
		var_out { "BlameHunk *", "hunk" },
		c_source [[
		${hunk} = (BlameHunk *)git_blame_get_hunk_byline(${this}, ${linenum});
]]
	},
}
