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

object "DiffOptions" {
	c_source [[
typedef git_diff_options DiffOptions;
]],
	constants {
		VERSION                         = 1,
		NORMAL                          = 0,
		REVERSE                         = 1,
		INCLUDE_IGNORED                 = 2,
		RECURSE_IGNORED_DIRS            = 4,
		INCLUDE_UNTRACKED               = 8,
		RECURSE_UNTRACKED_DIRS          = 16,
		INCLUDE_UNMODIFIED              = 32,
		INCLUDE_TYPECHANGE              = 64,
		INCLUDE_TYPECHANGE_TREES        = 128,
		IGNORE_FILEMODE                 = 256,
		IGNORE_SUBMODULES               = 512,
		IGNORE_CASE                     = 1024,
		INCLUDE_CASECHANGE              = 2048,
		DISABLE_PATHSPEC_MATCH          = 4096,
		SKIP_BINARY_CHECK               = 8192,
		ENABLE_FAST_UNTRACKED_DIRS      = 16384,
		UPDATE_INDEX                    = 32768,
		INCLUDE_UNREADABLE              = 65536,
		INCLUDE_UNREADABLE_AS_UNTRACKED = 131072,
		INDENT_HEURISTIC                = 262144,
		IGNORE_BLANK_LINES              = 524288,
		FORCE_TEXT                      = 1048576,
		FORCE_BINARY                    = 2097152,
		IGNORE_WHITESPACE               = 4194304,
		IGNORE_WHITESPACE_CHANGE        = 8388608,
		IGNORE_WHITESPACE_EOL           = 16777216,
		SHOW_UNTRACKED_CONTENT          = 33554432,
		SHOW_UNMODIFIED                 = 67108864,
		PATIENCE                        = 268435456,
		MINIMAL                         = 536870912,
		SHOW_BINARY                     = 1073741824,
		SUBMODULE_IGNORE_UNSPECIFIED    = -1,
		SUBMODULE_IGNORE_NONE           = 1,
		SUBMODULE_IGNORE_UNTRACKED      = 2,
		SUBMODULE_IGNORE_DIRTY          = 3,
		SUBMODULE_IGNORE_ALL            = 4,
	},
	constructor "init" {
		c_source [[
	${this} = calloc(1, sizeof(DiffOptions));
	git_diff_options_init(${this}, GIT_DIFF_OPTIONS_VERSION);
]],
	},
	destructor {
		c_source [[
	if(${this}->old_prefix != NULL) {
		free((void *)${this}->old_prefix);
		${this}->old_prefix = NULL;
	}
	if(${this}->new_prefix != NULL) {
		free((void *)${this}->new_prefix);
		${this}->new_prefix = NULL;
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
	field "unsigned int" "ignore_submodules",
	method "set_ignore_submodules" {
		var_in { "unsigned int", "ignore_submodules" },
		c_source [[
		${this}->ignore_submodules = ${ignore_submodules};
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
	field "uint32_t" "context_lines",
	method "set_context_lines" {
		var_in { "uint32_t", "context_lines" },
		c_source [[
		${this}->context_lines = ${context_lines};
]],
	},
	field "uint32_t" "interhunk_lines",
	method "set_interhunk_lines" {
		var_in { "uint32_t", "interhunk_lines" },
		c_source [[
		${this}->interhunk_lines = ${interhunk_lines};
]],
	},
	field "unsigned int" "oid_type",
	method "set_oid_type" {
		var_in { "unsigned int", "oid_type" },
		c_source [[
		${this}->oid_type = ${oid_type};
]],
	},
	field "uint16_t" "id_abbrev",
	method "set_id_abbrev" {
		var_in { "uint16_t", "id_abbrev" },
		c_source [[
		${this}->id_abbrev = ${id_abbrev};
]],
	},
	field "int64_t" "max_size",
	method "set_max_size" {
		var_in { "int64_t", "max_size" },
		c_source [[
		${this}->max_size = ${max_size};
]],
	},
	field "const char *" "old_prefix",
	method "set_old_prefix" {
		var_in { "const char *", "old_prefix" },
		c_source [[
		${this}->old_prefix = ${old_prefix};
]],
	},
	field "const char *" "new_prefix",
	method "set_new_prefix" {
		var_in { "const char *", "new_prefix" },
		c_source [[
		${this}->new_prefix = ${new_prefix};
]],
	},
}
