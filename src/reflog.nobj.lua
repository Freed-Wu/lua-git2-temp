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

-- Reflog operations (git_reflog_*)
-- Provides access to git reference log entries

object "Reflog" {
	basetype "git_reflog_t" "pointer",
	c_source [[
typedef git_reflog Reflog;
]],
	constructor "read" {
		c_call { "GitError", "err" } "git_reflog_read"
			{ "Reflog **", "&this>1", "Repository *", "repo", "const char *", "name" },
	},
	destructor {
		c_method_call "void" "git_reflog_free" {},
	},
	method "count" {
		c_call { "size_t", "ret" } "git_reflog_entrycount" { "Reflog *", "this" },
	},
	method "entry_by_index" {
		c_call "const ReflogEntry *>1" "git_reflog_entry_byindex" { "Reflog *", "this", "size_t", "idx" },
	},
	method "write" {
		c_call { "GitError", "err" } "git_reflog_write" { "Reflog *", "this" },
	},
	method "append" {
		c_call { "GitError", "err" } "git_reflog_append"
			{ "Reflog *", "this", "OID", "&id", "Signature *", "committer", "const char *", "msg" },
	},
}

object "ReflogEntry" {
	-- Borrowed pointer from Reflog, no destructor needed
	c_source [[
typedef git_reflog_entry ReflogEntry;
]],
	method "committer" {
		c_call "Signature *" "git_reflog_entry_committer" { "ReflogEntry *", "this" },
	},
	method "message" {
		c_call "const char *" "git_reflog_entry_message" { "ReflogEntry *", "this" },
	},
	method "oid_new" {
		c_call { "OID", "*out" } "git_reflog_entry_id_new" { "ReflogEntry *", "this" },
	},
	method "oid_old" {
		c_call { "OID", "*out" } "git_reflog_entry_id_old" { "ReflogEntry *", "this" },
	},
}

-- Module-level functions for reflog manipulation
c_function "reflog_delete" {
	var_in{ "Repository *", "repo" },
	var_in{ "const char *", "name" },
	var_out{ "GitError", "err" },
	c_source[[
	${err} = git_reflog_delete(${repo}, ${name});
]],
}

c_function "reflog_rename" {
	var_in{ "Repository *", "repo" },
	var_in{ "const char *", "old_name" },
	var_in{ "const char *", "new_name" },
	var_out{ "GitError", "err" },
	c_source[[
	${err} = git_reflog_rename(${repo}, ${old_name}, ${new_name});
]],
}
