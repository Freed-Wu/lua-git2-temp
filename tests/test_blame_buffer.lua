#!/usr/bin/env lua

local build_dir = arg[1]
if build_dir then
    package.cpath = build_dir .. "?.so;" .. package.cpath
end

local git2 = require "git2"

local TMP = (os.getenv("TMPDIR") or "/tmp") .. "/lua_git2_blame_buffer"

local function system(cmd)
    local ok = os.execute(cmd)
    if ok ~= 0 and ok ~= true then
        error("command failed: " .. cmd)
    end
end

local function write_file(path, text)
    local fh = assert(io.open(path, "w"))
    fh:write(text)
    fh:close()
end

local function append_file(path, text)
    local fh = assert(io.open(path, "a"))
    fh:write(text)
    fh:close()
end

local function assert_equal(expected, actual)
    assert(expected == actual, ("expected %s, got %s"):format(tostring(expected), tostring(actual)))
end

local function make_repo()
    system("rm -rf " .. TMP)
    system("mkdir -p " .. TMP)
    system("git -C " .. TMP .. " init -q")
    system("git -C " .. TMP .. " config user.email t@t")
    system("git -C " .. TMP .. " config user.name tester")
    write_file(TMP .. "/f", "a\nb\nc\n")
    system("git -C " .. TMP .. " add f")
    system("git -C " .. TMP .. " -c core.hooksPath=/dev/null -c commit.gpgsign=false commit --no-verify -qm base")
    append_file(TMP .. "/f", "d\ne\n")
end

make_repo()

local repo = assert(git2.Repository.open(TMP))
local opts = git2.BlameOptions.init()
local blame = assert(git2.Blame.file(repo, "f", opts))
assert_equal(3, blame:linecount())

local fh = assert(io.open(TMP .. "/f", "r"))
local contents = fh:read("*a")
fh:close()

local worktree_blame = assert(blame:buffer(contents))
assert_equal(5, worktree_blame:linecount())

local hunk = assert(worktree_blame:get_hunk_byline(5))
assert_equal(4, hunk:final_start_line_number())
assert_equal(2, hunk:lines_in_hunk())
assert_equal("0000000000000000000000000000000000000000", tostring(hunk:final_commit_id()))

print("ok")
