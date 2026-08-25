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
		c_method_call { "size_t", "count" } "git_blame_linecount" {}
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
