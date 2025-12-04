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

object "StatusEntry" {
	c_source [[
typedef git_status_entry StatusEntry;
]],
	constants {
		CURRENT = 0,
		INDEX_NEW = 1,
		INDEX_MODIFIED = 2,
		INDEX_DELETED = 4,
		INDEX_RENAMED = 8,
		INDEX_TYPECHANGE = 16,
		WT_NEW = 128,
		WT_MODIFIED = 256,
		WT_DELETED = 512,
		WT_TYPECHANGE = 1024,
		WT_RENAMED = 2048,
		WT_UNREADABLE = 4096,
		IGNORED = 16384,
		CONFLICTED = 32768,
	},
	constructor "byindex" {
		c_call "StatusEntry *>1" "git_status_byindex" { "StatusList *", "statuslist", "size_t", "idx" },
	},
	field "unsigned int" "status",
	field "DiffDelta *" "head_to_index",
	field "DiffDelta *" "index_to_workdir",
}
