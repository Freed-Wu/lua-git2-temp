#!/usr/bin/env lua
local build_dir = arg[1] or "./build/"
package.cpath = build_dir .. "?.so;" .. package.cpath
local git2 = require "git2"
local TMP = (os.getenv("TMPDIR") or "/tmp") .. "/lua_git2_reflog_test"
local function system(cmd) os.execute(cmd) end
local function write_file(path, text) local f=io.open(path,"w"); f:write(text); f:close() end
local function append_file(path, text) local f=io.open(path,"a"); f:write(text); f:close() end
local function check(name, ok) print(ok and "PASS:"..name or "FAIL:"..name, ok and "" or tostring(err)) return ok end
print("\n=== Test Reflog API Bindings ===\n")
system("rm -rf " .. TMP .. " && mkdir -p " .. TMP)
system("git -C " .. TMP .. " init -q")
system("git -C " .. TMP .. " config user.email tester@test.com")
system("git -C " .. TMP .. " config user.name Tester")
system("git -C " .. TMP .. " config core.hooksPath /dev/null")
write_file(TMP .. "/file.txt", "line 1\nline 2\n")
system("git -C " .. TMP .. " add file.txt; git -C " .. TMP .. " commit --no-verify -qm 'first'")
append_file(TMP .. "/file.txt", "line 3\n")
system("git -C " .. TMP .. " add file.txt; git -C " .. TMP .. " commit --no-verify -qm 'second'")
local repo = assert(git2.Repository.open(TMP))
check("open repository", repo ~= nil)
local ref_head = git2.Reference.lookup(repo, "HEAD")
check("lookup HEAD reference", ref_head ~= nil)
if ref_head then
    local resolved = ref_head:resolve()
    if resolved then
        local target = resolved:target()
        check("resolve and get target", target ~= nil and target.fmt ~= nil)
        resolved:delete()
    else
        check("resolved is valid userdata", resolved == true)
    end
    local head_reflog = git2.Reflog.read(repo, "HEAD")
    check("Reflog.read on HEAD", head_reflog ~= nil)
    if head_reflog then
        local count = head_reflog:count()
        check("reflog:count() returns number", type(count) == "number")
        print("  reflog has " .. count .. " entries")
        for i = 0, math.min(count, 3) - 1 do
            local entry = head_reflog:entry_by_index(i)
            if entry then
                local old_oid = entry:oid_old()
                local new_oid = entry:oid_new()
                local committer = entry:committer()
                local msg = entry:message()
                check("entry[" .. i .. "] oid_old", old_oid ~= nil and old_oid.fmt ~= nil)
                check("entry[" .. i .. "] oid_new", new_oid ~= nil and new_oid.fmt ~= nil)
                check("entry[" .. i .. "] committer", committer ~= nil)
                check("entry[" .. i .. "] message", msg ~= nil and type(msg) == "string")
            end
        end
    end
end
check("reflog_rename exists", type(git2.reflog_rename) == "function")
check("reflog_delete exists", type(git2.reflog_delete) == "function")
print("\n=== All Tests Complete ===")
print("ok")
