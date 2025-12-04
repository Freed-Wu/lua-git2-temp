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

object "DiffFile" {
	c_source [[
typedef git_diff_file DiffFile;
]],
	constants {
		BINARY = 1,
		NOT_BINARY = 2,
		VALID_ID = 4,
		EXISTS = 8,
		VALID_SIZE = 16,
	},
	destructor {
		c_source [[
	if(${this}->path != NULL) {
		free((void *)${this}->path);
		${this}->path = NULL;
	}
	free(${this});
]]
	},
	field "OID" "id",
	field "const char *" "path",
	field "uint64_t" "size",
	field "uint32_t" "flags",
	field "uint16_t" "mode",
	field "uint16_t" "id_abbrev",
}
