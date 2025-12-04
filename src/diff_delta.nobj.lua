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

object "DiffDelta" {
	c_source [[
typedef git_diff_delta DiffDelta;
]],
	constants {
		UNMODIFIED = 0,
		ADDED = 1,
		DELETED = 2,
		MODIFIED = 3,
		RENAMED = 4,
		COPIED = 5,
		IGNORED = 6,
		UNTRACKED = 7,
		TYPECHANGE = 8,
		UNREADABLE = 9,
		CONFLICTED = 10,
	},
	constructor "get" {
		c_call "const DiffDelta *>1" "git_diff_get_delta" { "Diff *", "diff", "size_t", "idx" },
	},
	field "unsigned int" "status",
	field "uint32_t" "flags",
	field "uint16_t" "similarity",
	field "uint16_t" "nfiles",
	method "old_file" {
		var_out { "DiffFile *", "old_file" },
		c_source [[
		${old_file} = &${this}->old_file;
]],
	},
	method "new_file" {
		var_out { "DiffFile *", "new_file" },
		c_source [[
		${new_file} = &${this}->new_file;
]],
	},
}
