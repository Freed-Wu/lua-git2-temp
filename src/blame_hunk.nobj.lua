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

object "BlameHunk" {
	c_source [[
typedef git_blame_hunk BlameHunk;
]],
	-- Borrowed view into a Blame object; never owns or frees the hunk memory.
	field "size_t" "lines_in_hunk",
	field "OID" "final_commit_id",
	field "size_t" "final_start_line_number",
	field "size_t" "orig_start_line_number",
	field "const char *" "orig_path",
	field "const char *" "summary",
	field "char" "boundary",
	method "final_signature" {
		var_out { "Signature *", "sig" },
		c_source [[
		${sig} = ${this}->final_signature;
]]
	},
	method "final_committer" {
		var_out { "Signature *", "sig" },
		c_source [[
		${sig} = ${this}->final_committer;
]]
	},
	method "orig_signature" {
		var_out { "Signature *", "sig" },
		c_source [[
		${sig} = ${this}->orig_signature;
]]
	},
	method "orig_committer" {
		var_out { "Signature *", "sig" },
		c_source [[
		${sig} = ${this}->orig_committer;
]]
	},
}
