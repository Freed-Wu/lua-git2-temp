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

object "BlameOptions" {
	c_source [[
typedef git_blame_options BlameOptions;
]],
	constants {
		NORMAL                          = 0,
		TRACK_COPIES_SAME_FILE          = 1,
		TRACK_COPIES_SAME_COMMIT_MOVES  = 2,
		TRACK_COPIES_SAME_COMMIT_COPIES = 4,
		TRACK_COPIES_ANY_COMMIT_COPIES  = 8,
		FIRST_PARENT                    = 16,
		USE_MAILMAP                     = 32,
		IGNORE_WHITESPACE               = 64,
	},
	constructor "init" {
		c_source [[
	${this} = calloc(1, sizeof(BlameOptions));
	git_blame_options_init(${this}, GIT_BLAME_OPTIONS_VERSION);
]],
	},
	destructor {
		c_source [[
	free(${this});
]]
	},
	field "unsigned int" "flags",
	method "set_flags" {
		var_in { "unsigned int", "flags" },
		c_source [[
		${this}->flags = ${flags};
]]
	},
	field "uint16_t" "min_match_characters",
	method "set_min_match_characters" {
		var_in { "uint16_t", "min_match_characters" },
		c_source [[
		${this}->min_match_characters = ${min_match_characters};
]]
	},
	method "set_newest_commit" {
		var_in { "OID", "id" },
		c_source [[
		git_oid_cpy(&(${this}->newest_commit), &(${id}));
]]
	},
	method "set_oldest_commit" {
		var_in { "OID", "id" },
		c_source [[
		git_oid_cpy(&(${this}->oldest_commit), &(${id}));
]]
	},
	field "size_t" "min_line",
	method "set_min_line" {
		var_in { "size_t", "min_line" },
		c_source [[
		${this}->min_line = ${min_line};
]]
	},
	field "size_t" "max_line",
	method "set_max_line" {
		var_in { "size_t", "max_line" },
		c_source [[
		${this}->max_line = ${max_line};
]]
	},
}
