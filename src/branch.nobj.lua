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

-- Branch operations (git_branch_*)
-- Utility functions for branch-specific behavior

object "Branch" {
    -- This is a utility object, no associated C type
    c_source [[
/* Branch utility functions */
]],

    -- Create a new branch pointing to a commit
    c_function "create" {
        var_in{ "Repository *", "repo" },
        var_in{ "const char *", "branch_name" },
        var_in{ "const Commit *", "target" },
        var_in{ "int", "force" },
        var_out{ "Reference *", "ref_out" },
        var_out{ "GitError", "err" },
        c_source[[
        ${err} = git_branch_create(&${ref_out}, ${repo}, ${branch_name}, ${target}, ${force});
    ]],
    },

    -- Delete a branch
    c_function "delete" {
        var_in{ "Reference *", "branch" },
        var_out{ "GitError", "err" },
        c_source[[
        ${err} = git_branch_delete(${branch});
    ]],
    },

    -- Rename/move a branch
    c_function "move" {
        var_in{ "Reference *", "branch" },
        var_in{ "const char *", "new_branch_name" },
        var_in{ "int", "force" },
        var_out{ "Reference *", "ref_out" },
        var_out{ "GitError", "err" },
        c_source[[
        ${err} = git_branch_move(&${ref_out}, ${branch}, ${new_branch_name}, ${force});
    ]],
    },

    -- Lookup a branch by name in a repository
    c_function "lookup" {
        var_in{ "Repository *", "repo" },
        var_in{ "const char *", "branch_name" },
        var_in{ "int", "branch_type" },
        var_out{ "Reference *", "ref_out" },
        var_out{ "GitError", "err" },
        c_source[[
        ${err} = git_branch_lookup(&${ref_out}, ${repo}, ${branch_name}, ${branch_type});
    ]],
    },

    -- List branches using iterator pattern
    c_function "list" {
        var_in{ "Repository *", "repo" },
        var_in{ "int", "list_flags" },
        var_out{ "<any>", "branches" },
        var_out{ "GitError", "err" },
        c_source "pre" [[
        git_branch_iterator *iter = NULL;
    ]],
        c_source[[
        ${err} = git_branch_iterator_new(&iter, ${repo}, ${list_flags});
        if(${err} != GIT_OK) {
            lua_pushnil(L);
            return 2;
        }
        lua_newtable(L);
        {
            git_reference *ref = NULL;
            git_branch_t type;
            const char *name = NULL;
            int idx = 1;
            while((${err} = git_branch_next(&ref, &type, iter)) == GIT_OK) {
                if(git_branch_name(&name, ref) == GIT_OK && name) {
                    lua_pushstring(L, name);
                    lua_rawseti(L, -2, idx++);
                }
                git_reference_free(ref);
                ref = NULL;
                name = NULL;
            }
        }
        git_branch_iterator_free(iter);
        /* GIT_ITEROVER is expected, not a real error */
        if(${err} == GIT_ITEROVER) {
            ${err} = GIT_OK;
        }
    ]],
    },

    -- Check if branch is HEAD
    c_function "is_head" {
        var_in{ "Reference *", "branch" },
        var_out{ "int", "is_head" },
        var_out{ "GitError", "err" },
        c_source[[
        int result = git_branch_is_head(${branch});
        if (result < 0) {
            ${err} = (GitError)result;
            ${is_head} = 0;
        } else {
            ${err} = GIT_OK;
            ${is_head} = result;
        }
    ]],
    },

    -- Check if branch is checked out in any worktree
    c_function "is_checked_out" {
        var_in{ "Reference *", "branch" },
        var_out{ "int", "checked_out" },
        var_out{ "GitError", "err" },
        c_source[[
        int result = git_branch_is_checked_out(${branch});
        if (result < 0) {
            ${err} = (GitError)result;
            ${checked_out} = 0;
        } else {
            ${err} = GIT_OK;
            ${checked_out} = result;
        }
    ]],
    },

    -- Get branch name from reference
    c_function "name" {
        var_in{ "Reference *", "branch" },
        var_out{ "const char *", "name" },
        var_out{ "GitError", "err" },
        c_source[[
        ${err} = git_branch_name(&${name}, ${branch});
    ]],
    },

    -- Get remote name for a remote-tracking branch
    c_function "remote_name" {
        var_in{ "Repository *", "repo" },
        var_in{ "const char *", "refname" },
        var_out{ "<any>", "result" },
        c_source[[
        git_buf out = GIT_BUF_INIT;
        int rc = git_branch_remote_name(&out, ${repo}, ${refname});
        if(rc < 0) {
            if(out.ptr) git_buf_dispose(&out);
            lua_pushnil(L);
        } else {
            lua_pushlstring(L, out.ptr ? out.ptr : "", out.size);
            git_buf_dispose(&out);
        }
    ]],
    },

    -- Get upstream name for a local branch
    c_function "upstream_name" {
        var_in{ "Repository *", "repo" },
        var_in{ "const char *", "refname" },
        var_out{ "<any>", "result" },
        c_source[[
        git_buf out = GIT_BUF_INIT;
        int rc = git_branch_upstream_name(&out, ${repo}, ${refname});
        if(rc < 0) {
            if(out.ptr) git_buf_dispose(&out);
            lua_pushnil(L);
        } else {
            lua_pushlstring(L, out.ptr ? out.ptr : "", out.size);
            git_buf_dispose(&out);
        }
    ]],
    },

    -- Get upstream remote name for a local branch
    c_function "upstream_remote" {
        var_in{ "Repository *", "repo" },
        var_in{ "const char *", "refname" },
        var_out{ "<any>", "result" },
        c_source[[
        git_buf out = GIT_BUF_INIT;
        int rc = git_branch_upstream_remote(&out, ${repo}, ${refname});
        if(rc < 0) {
            if(out.ptr) git_buf_dispose(&out);
            lua_pushnil(L);
        } else {
            lua_pushlstring(L, out.ptr ? out.ptr : "", out.size);
            git_buf_dispose(&out);
        }
    ]],
    },

    -- Get upstream merge ref for a local branch
    c_function "upstream_merge" {
        var_in{ "Repository *", "repo" },
        var_in{ "const char *", "refname" },
        var_out{ "<any>", "result" },
        c_source[[
        git_buf out = GIT_BUF_INIT;
        int rc = git_branch_upstream_merge(&out, ${repo}, ${refname});
        if(rc < 0) {
            if(out.ptr) git_buf_dispose(&out);
            lua_pushnil(L);
        } else {
            lua_pushlstring(L, out.ptr ? out.ptr : "", out.size);
            git_buf_dispose(&out);
        }
    ]],
    },

    -- Get upstream reference
    c_function "upstream" {
        var_in{ "Reference *", "branch" },
        var_out{ "Reference *", "upstream_out" },
        var_out{ "GitError", "err" },
        c_source[[
        ${err} = git_branch_upstream(&${upstream_out}, ${branch});
    ]],
    },

    -- Set upstream reference
    c_function "set_upstream" {
        var_in{ "Reference *", "branch" },
        var_in{ "const char *", "upstream_name" },
        var_out{ "GitError", "err" },
        c_source[[
        ${err} = git_branch_set_upstream(${branch}, ${upstream_name});
    ]],
    },

    -- Check whether a branch name is valid
    c_function "name_is_valid" {
        var_in{ "const char *", "name" },
        var_out{ "int", "valid" },
        var_out{ "GitError", "err" },
        c_source[[
        ${err} = git_branch_name_is_valid(&${valid}, ${name});
    ]],
    },

    -- Create branch from OID (convenience function)
    c_function "create_from_oid" {
        var_in{ "Repository *", "repo" },
        var_in{ "const char *", "branch_name" },
        var_in{ "OID", "&commit_oid" },
        var_in{ "int", "force" },
        var_out{ "Reference *", "ref_out" },
        var_out{ "GitError", "err" },
        c_source[[
        git_commit *commit = NULL;
        ${err} = git_commit_lookup(&commit, ${repo}, &${commit_oid});
        if(${err} == GIT_OK) {
            ${err} = git_branch_create(&${ref_out}, ${repo}, ${branch_name}, commit, ${force});
            git_commit_free(commit);
        }
    ]],
    }
}
