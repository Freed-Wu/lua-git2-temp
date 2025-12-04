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

object "CheckoutOptions" {
	c_source [[
typedef git_checkout_options CheckoutOptions;
]],
	constants {
		VERSION                      = 1,
		SAFE                         = 0,
		FORCE                        = 2,
		RECREATE_MISSING             = 4,
		ALLOW_CONFLICTS              = 16,
		REMOVE_UNTRACKED             = 32,
		REMOVE_IGNORED               = 64,
		UPDATE_ONLY                  = 128,
		DONT_UPDATE_INDEX            = 256,
		NO_REFRESH                   = 512,
		SKIP_UNMERGED                = 1024,
		USE_OURS                     = 2048,
		USE_THEIRS                   = 4096,
		DISABLE_PATHSPEC_MATCH       = 8192,
		SKIP_LOCKED_DIRECTORIES      = 262144,
		DONT_OVERWRITE_IGNORED       = 524288,
		CONFLICT_STYLE_MERGE         = 1048576,
		CONFLICT_STYLE_DIFF3         = 2097152,
		DONT_REMOVE_EXISTING         = 4194304,
		DONT_WRITE_INDEX             = 8388608,
		DRY_RUN                      = 16777216,
		CONFLICT_STYLE_ZDIFF3        = 33554432,
		NONE                         = 1073741824,
		UPDATE_SUBMODULES            = 65536,
		UPDATE_SUBMODULES_IF_CHANGED = 131072,
	},
	constructor "init" {
		c_source [[
	${this} = calloc(1, sizeof(CheckoutOptions));
	git_checkout_options_init(${this}, GIT_STATUS_OPTIONS_VERSION);
]],
	},
	destructor {
		c_source [[
	free(${this});
]]
	},
	field "unsigned int" "version",
	field "unsigned int" "checkout_strategy",
	method "set_checkout_strategy" {
		var_in { "unsigned int", "checkout_strategy" },
		c_source [[
		${this}->checkout_strategy = ${checkout_strategy};
]],
	},
	field "int" "disable_filters",
	method "set_disable_filters" {
		var_in { "int", "disable_filters" },
		c_source [[
		${this}->disable_filters = ${disable_filters};
]],
	},
	field "unsigned int" "dir_mode",
	method "set_dir_mode" {
		var_in { "unsigned int", "dir_mode" },
		c_source [[
		${this}->dir_mode = ${dir_mode};
]],
	},
	field "unsigned int" "file_mode",
	method "set_file_mode" {
		var_in { "unsigned int", "file_mode" },
		c_source [[
		${this}->file_mode = ${file_mode};
]],
	},
	field "int" "file_open_flags",
	method "set_file_open_flags" {
		var_in { "int", "file_open_flags" },
		c_source [[
		${this}->file_open_flags = ${file_open_flags};
]],
	},
	field "unsigned int" "notify_flags",
	method "set_notify_flags" {
		var_in { "unsigned int", "notify_flags" },
		c_source [[
		${this}->notify_flags = ${notify_flags};
]],
	},
	method "paths" {
		var_out { "StrArray *", "paths" },
		c_source [[
		${paths} = &${this}->paths;
]],
	},
	method "set_paths" {
		var_in { "StrArray *", "paths" },
		c_source [[
		${this}->paths.strings = ${paths}->strings;
		${this}->paths.count = ${paths}->count;
]],
	},
	field "Tree *" "baseline",
	method "set_baseline" {
		var_in { "Tree *", "baseline" },
		c_source [[
		${this}->baseline = ${baseline};
]],
	},
	field "Index *" "baseline_index",
	method "set_baseline_index" {
		var_in { "Index *", "baseline_index" },
		c_source [[
		${this}->baseline_index = ${baseline_index};
]],
	},
	field "const char *" "target_directory",
	method "set_target_directory" {
		var_in { "const char *", "target_directory" },
		c_source [[
		${this}->target_directory = ${target_directory};
]],
	},
	field "const char *" "ancestor_label",
	method "set_ancestor_label" {
		var_in { "const char *", "ancestor_label" },
		c_source [[
		${this}->ancestor_label = ${ancestor_label};
]],
	},
	field "const char *" "our_label",
	method "set_our_label" {
		var_in { "const char *", "our_label" },
		c_source [[
		${this}->our_label = ${our_label};
]],
	},
	field "const char *" "their_label",
	method "set_their_label" {
		var_in { "const char *", "their_label" },
		c_source [[
		${this}->their_label = ${their_label};
]],
	},
}
