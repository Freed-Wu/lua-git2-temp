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

object "StatusOptions" {
	c_source [[
typedef git_status_options StatusOptions;
]],
	constants {
		VERSION                         = 1,
		SHOW_INDEX_AND_WORKDIR          = 0,
		SHOW_INDEX_ONLY                 = 1,
		SHOW_WORKDIR_ONLY               = 2,
		INCLUDE_UNTRACKED               = 1,
		INCLUDE_IGNORED                 = 2,
		INCLUDE_UNMODIFIED              = 4,
		EXCLUDE_SUBMODULES              = 8,
		RECURSE_UNTRACKED_DIRS          = 16,
		DISABLE_PATHSPEC_MATCH          = 32,
		RECURSE_IGNORED_DIRS            = 64,
		RENAMES_HEAD_TO_INDEX           = 128,
		RENAMES_INDEX_TO_WORKDIR        = 256,
		SORT_CASE_SENSITIVELY           = 512,
		SORT_CASE_INSENSITIVELY         = 1024,
		RENAMES_FROM_REWRITES           = 2048,
		NO_REFRESH                      = 4096,
		UPDATE_INDEX                    = 8192,
		INCLUDE_UNREADABLE              = 16384,
		INCLUDE_UNREADABLE_AS_UNTRACKED = 32768,
	},
	constructor "init" {
		c_source [[
	${this} = calloc(1, sizeof(StatusOptions));
	git_status_options_init(${this}, GIT_STATUS_OPTIONS_VERSION);
]],
	},
	destructor {
		c_source [[
	free(${this});
]]
	},
	field "unsigned int" "version",
	field "unsigned int" "show",
	method "set_show" {
		var_in { "unsigned int", "show" },
		c_source [[
		${this}->show = ${show};
]],
	},
	field "unsigned int" "flags",
	method "set_flags" {
		var_in { "unsigned int", "flags" },
		c_source [[
		${this}->flags = ${flags};
]],
	},
	method "pathspec" {
		var_out { "StrArray *", "pathspec" },
		c_source [[
		${pathspec} = &${this}->pathspec;
]],
	},
	method "set_pathspec" {
		var_in { "StrArray *", "pathspec" },
		c_source [[
		${this}->pathspec.strings = ${pathspec}->strings;
		${this}->pathspec.count = ${pathspec}->count;
]],
	},
	field "Tree *" "baseline",
	method "set_baseline" {
		var_in { "Tree *", "baseline" },
		c_source [[
		${this}->baseline = ${baseline};
]],
	},
	field "uint16_t" "rename_threshold",
	method "set_rename_threshold" {
		var_in { "uint16_t", "rename_threshold" },
		c_source [[
		${this}->rename_threshold = ${rename_threshold};
]],
	},
}
