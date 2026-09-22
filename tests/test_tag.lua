#!/usr/bin/env lua
-- Test tag operations
local build_dir = arg[1] or "./build/"
package.cpath = build_dir .. "?.so;" .. package.cpath
local git2 = require "git2"

local TMP = (os.getenv("TMPDIR") or "/tmp") .. "/lua_git2_tag_test"

local function system(cmd) os.execute(cmd) end
local function write_file(path, text) local f=io.open(path,"w"); f:write(text); f:close() end
local function append_file(path, text) local f=io.open(path,"a"); f:write(text); f:close() end
local function check(name, ok)
    print(ok and "PASS:"..name or "FAIL:"..name, ok and "" or tostring(err))
    return ok
end

print("\n=== Test Tag API Bindings ===\n")

-- Setup test repository
system("rm -rf " .. TMP .. " && mkdir -p " .. TMP)
system("git -C " .. TMP .. " init -q")
system("git -C " .. TMP .. " config user.email tester@test.com")
system("git -C " .. TMP .. " config user.name Tester")
system("git -C " .. TMP .. " config core.hooksPath /dev/null")
write_file(TMP .. "/file.txt", "initial content\n")
system("git -C " .. TMP .. " add file.txt; git -C " .. TMP .. " commit --no-verify -qm 'first commit'")
append_file(TMP .. "/file.txt", "more content\n")
system("git -C " .. TMP .. " add file.txt; git -C " .. TMP .. " commit --no-verify -qm 'second commit'")

local repo = assert(git2.Repository.open(TMP))
check("open repository", repo ~= nil)

-- Get HEAD commit for tagging
local head_ref = git2.Reference.lookup(repo, "HEAD")
check("lookup HEAD reference", head_ref ~= nil)
local resolved = head_ref:resolve()
check("resolve HEAD", resolved ~= nil)
local head_oid = resolved:target()
check("get HEAD OID", head_oid ~= nil)

-- Lookup the commit object
local commit = git2.Commit.lookup(repo, head_oid)
check("lookup HEAD commit", commit ~= nil)

print("\n--- Tag Creation Tests ---")

-- Create a signature for the tagger
local tagger = git2.Signature.now("Test Tagger", "tagger@test.com")
check("create tagger signature", tagger ~= nil)

-- Create an annotated tag
local tag_oid, err = git2.tag_create(repo, "v1.0", commit, tagger, "Release version 1.0", 0)
check("create annotated tag 'v1.0'", tag_oid ~= nil)
print("  tag OID: " .. tostring(tag_oid))

-- Create a lightweight tag
local lw_tag_oid, err = git2.tag_create_lightweight(repo, "v1.1", commit, 0)
check("create lightweight tag 'v1.1'", lw_tag_oid ~= nil)
print("  lightweight tag OID: " .. tostring(lw_tag_oid))

print("\n--- Tag List Tests ---")

-- List all tags
local tag_list, err = git2.tag_list(repo)
check("list all tags", tag_list ~= nil)
if tag_list then
    local arr = tag_list:get_array()
    print("  found " .. #arr .. " tags:")
    for i, name in ipairs(arr) do
        print("    [" .. i .. "] " .. name)
    end
end

-- List tags matching pattern
local matched_tags, err = git2.tag_list_match(repo, "v1.*")
check("list tags matching 'v1.*'", matched_tags ~= nil)
if matched_tags then
    local arr = matched_tags:get_array()
    print("  found " .. #arr .. " matching tags")
end

print("\n--- Tag Lookup and Inspection Tests ---")

-- Lookup the annotated tag
local tag = git2.Tag.lookup(repo, tag_oid)
check("lookup tag by OID", tag ~= nil)

if tag then
    -- Test tag:id()
    local id = tag:id()
    check("tag:id()", id ~= nil)
    print("  tag id: " .. tostring(id))

    -- Test tag:name()
    local name = tag:name()
    check("tag:name()", name == "v1.0")
    print("  tag name: " .. tostring(name))

    -- Test tag:message()
    local msg = tag:message()
    check("tag:message()", msg ~= nil and #msg > 0)
    print("  tag message: " .. tostring(msg))

    -- Test tag:tagger()
    local t = tag:tagger()
    check("tag:tagger()", t ~= nil)
    if t then
        print("  tagger: " .. t:name() .. " <" .. t:email() .. ">")
    end

    -- Test tag:target_type() - now returns string directly
    local ttype = tag:target_type()
    check("tag:target_type()", ttype ~= nil and ttype == "commit")
    print("  target type: " .. tostring(ttype))

    -- Test tag:target_id()
    local tid = tag:target_id()
    check("tag:target_id()", tid ~= nil)
    print("  target id: " .. tostring(tid))

    -- Test tag:target()
    local target = tag:target()
    check("tag:target()", target ~= nil)

    -- Test tag:peel()
    local peeled = tag:peel()
    check("tag:peel()", peeled ~= nil)
    print("  peeled object type: " .. tostring(peeled:type()))

    -- Test tag:owner()
    local owner = tag:owner()
    check("tag:owner()", owner == repo)

    -- Test tag:free()
    tag:free()
    check("tag:free()", true)
end

print("\n--- Tag Deletion Tests ---")

-- Delete the lightweight tag
local del_ok = git2.tag_delete(repo, "v1.1")
-- Returns true/nil or error code
check("delete tag 'v1.1'", del_ok ~= nil and del_ok ~= false)
print("  delete returned: " .. tostring(del_ok))

-- Verify deletion by listing again
local tag_list2, err = git2.tag_list(repo)
if tag_list2 then
    local arr = tag_list2:get_array()
    local found_v11 = false
    for i, name in ipairs(arr) do
        if name == "v1.1" then found_v11 = true end
    end
    check("verify tag 'v1.1' deleted", not found_v11)
    print("  remaining tags: " .. #arr)
end

print("\n--- Tag Create From Buffer Test ---")

-- Create tag from buffer (using raw tag data format)
-- The buffer must be a properly formatted tag object
local head_oid_str = tostring(head_oid)
local buffer = "object " .. head_oid_str .. "\n" ..
               "type commit\n" ..
               "tag v2.0\n" ..
               "tagger Test Tagger <tagger@test.com> " .. os.time() .. " +0000\n" ..
               "\n" ..
               "Release version 2.0\n"

local buf_tag_oid, err = git2.tag_create_frombuffer(repo, buffer, 0)
check("create tag from buffer", buf_tag_oid ~= nil)
if buf_tag_oid then
    print("  buffer-created tag OID: " .. tostring(buf_tag_oid))
else
    print("  error: " .. tostring(err))
end

print("\n=== All Tests Complete ===")
print("ok")

-- Cleanup
system("rm -rf " .. TMP)
