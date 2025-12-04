-- Copyright (c) 2010-2012 by Robert G. Jakabosky <bobby@sharedrealm.com>
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentatsion files (the "Software"), to deal
-- in the Software without restriction, including without limitatsion the rights
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

object "DiffStats" {
	c_source [[
typedef git_diff_stats DiffStats;
]],
	constants {
		NONE = 0,
		FULL = 1,
		SHORT = 2,
		NUMBER = 4,
		INCLUDE_SUMMARY = 8,
	},
	constructor "get" {
		c_call { "GitError", "err" } "git_diff_get_stats" { "DiffStats *", "&this>1", "Diff *", "diff" },
	},
	method "insertions" {
		c_method_call { "size_t", "size" } "git_diff_stats_insertions" {},
	},
	method "deletions" {
		c_method_call { "size_t", "size" } "git_diff_stats_deletions" {},
	},
	method "files_changed" {
		c_method_call { "size_t", "size" } "git_diff_stats_files_changed" {},
	},
}
