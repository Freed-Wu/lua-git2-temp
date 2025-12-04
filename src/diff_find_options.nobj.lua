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

object "DiffFindOptions" {
	c_source [[
typedef git_diff_find_options DiffFindOptions;
]],
	constants {
		VERSION = 1,
		BY_CONFIG = 0,
		RENAMES = 1,
		RENAMES_FROM_REWRITES = 2,
		COPIES = 4,
		COPIES_FROM_UNMODIFIED = 8,
		REWRITES = 16,
		AND_BREAK_REWRITES = 48,
		FOR_UNTRACKED = 64,
		ALL = 255,
		IGNORE_LEADING_WHITESPACE = 0,
		IGNORE_WHITESPACE = 4096,
		DONT_IGNORE_WHITESPACE = 8192,
		EXACT_MATCH_ONLY = 16384,
		REMOVE_UNMODIFIED = 65536,
		BREAK_REWRITES = 32,
		BREAK_REWRITES_FOR_RENAMES_ONLY = 32768,
	},
	constructor "init" {
		c_source [[
	${this} = calloc(1, sizeof(DiffFindOptions));
	git_diff_find_options_init(${this}, GIT_DIFF_OPTIONS_VERSION);
]],
	},
	destructor {
		c_source [[
	if(${this}->metric != NULL) {
		free((void *)${this}->metric);
		${this}->metric = NULL;
	}
	free(${this});
]]
	},
	field "unsigned int" "version",
	field "unsigned int" "flags",
	method "set_flags" {
		var_in { "unsigned int", "flags" },
		c_source [[
		${this}->flags = ${flags};
]],
	},
	field "uint16_t" "rename_threshold",
	method "set_rename_threshold" {
		var_in { "uint16_t", "rename_threshold" },
		c_source [[
		${this}->rename_threshold = ${rename_threshold};
]],
	},
	field "uint16_t" "rename_from_rewrite_threshold",
	method "set_rename_from_rewrite_threshold" {
		var_in { "uint16_t", "rename_from_rewrite_threshold" },
		c_source [[
		${this}->rename_from_rewrite_threshold = ${rename_from_rewrite_threshold};
]],
	},
	field "uint16_t" "copy_threshold",
	method "set_copy_threshold" {
		var_in { "uint16_t", "copy_threshold" },
		c_source [[
		${this}->copy_threshold = ${copy_threshold};
]],
	},
	field "uint16_t" "break_rewrite_threshold",
	method "set_break_rewrite_threshold" {
		var_in { "uint16_t", "break_rewrite_threshold" },
		c_source [[
		${this}->break_rewrite_threshold = ${break_rewrite_threshold};
]],
	},
	field "size_t" "rename_limit",
	method "set_rename_limit" {
		var_in { "size_t", "rename_limit" },
		c_source [[
		${this}->rename_limit = ${rename_limit};
]],
	},
}
