#!/usr/bin/env lua
-- Test TreeBuilder functionality

local git2 = require("git2")

print("=== TreeBuilder Module Tests ===\n")

-- Create a temporary repository for testing
local test_dir = "/tmp/lua-git2-treebuilder-test"
os.execute("rm -rf " .. test_dir)
os.execute("mkdir -p " .. test_dir)

local repo, err = git2.Repository.init(test_dir, 0)
if not repo then
    print("FAIL: Failed to create test repository: " .. tostring(err))
    os.exit(1)
end
print("PASS: Created test repository")

-- Create a signature for commits
local sig, err = git2.Signature.now("Test User", "test@example.com")
if not sig then
    print("FAIL: Failed to create signature: " .. tostring(err))
    os.exit(1)
end
print("PASS: Created signature")

-- Create a blob to use in tree entries
local blob_id, err = git2.Blob.from_buffer(repo, "test file content")
if not blob_id then
    print("FAIL: Failed to create blob: " .. tostring(err))
    os.exit(1)
end
print("PASS: Created blob with ID: " .. tostring(blob_id))

-- Test 1: Create empty TreeBuilder
print("\n--- Test 1: Create empty TreeBuilder ---")
local builder, err = git2.TreeBuilder.create(repo, nil)
if not builder then
    print("FAIL: Failed to create TreeBuilder: " .. tostring(err))
    os.exit(1)
end
print("PASS: Created empty TreeBuilder")

-- Test 2: Check initial entry count
print("\n--- Test 2: Check initial entry count ---")
local count = builder:entrycount()
print("Entry count: " .. count)
if count == 0 then
    print("PASS: Initial entry count is 0")
else
    print("FAIL: Expected 0 entries, got " .. count)
end

-- Test 3: Insert a file entry
print("\n--- Test 3: Insert file entry ---")
local ok, err = builder:insert("test.txt", blob_id, git2.FILEMODE_BLOB)
if ok then
    print("PASS: Inserted file entry")
else
    print("FAIL: Failed to insert entry: " .. tostring(err))
end

-- Test 4: Check entry count after insert
print("\n--- Test 4: Check entry count after insert ---")
count = builder:entrycount()
print("Entry count: " .. count)
if count == 1 then
    print("PASS: Entry count is 1")
else
    print("FAIL: Expected 1 entry, got " .. count)
end

-- Test 5: Get entry by name
print("\n--- Test 5: Get entry by name ---")
local entry = builder:get("test.txt")
if entry then
    print("PASS: Retrieved entry")
    print("  Name: " .. entry:name())
    print("  Filemode: " .. entry:filemode())
    print("  ID: " .. tostring(entry:id()))
else
    print("FAIL: Failed to get entry")
end

-- Test 6: Get non-existent entry
print("\n--- Test 6: Get non-existent entry ---")
local missing = builder:get("nonexistent.txt")
if missing then
    print("FAIL: Should not have found non-existent entry")
else
    print("PASS: Correctly returned nil for non-existent entry")
end

-- Test 7: Insert executable file
print("\n--- Test 7: Insert executable file ---")
local exec_blob_id, err = git2.Blob.from_buffer(repo, "#!/bin/bash\necho hello")
if not exec_blob_id then
    print("FAIL: Failed to create executable blob: " .. tostring(err))
else
    local ok, err = builder:insert("script.sh", exec_blob_id, git2.FILEMODE_BLOB_EXECUTABLE)
    if ok then
        print("PASS: Inserted executable entry")
        local entry = builder:get("script.sh")
        if entry then
            local mode = entry:filemode()
            print("  Filemode: " .. string.format("0%o", mode))
            if mode == git2.FILEMODE_BLOB_EXECUTABLE then
                print("PASS: Executable filemode is correct")
            else
                print("FAIL: Expected filemode " .. git2.FILEMODE_BLOB_EXECUTABLE .. ", got " .. mode)
            end
        end
    else
        print("FAIL: Failed to insert executable entry: " .. tostring(err))
    end
end

-- Test 8: Remove an entry
print("\n--- Test 8: Remove an entry ---")
local ok, err = builder:remove("test.txt")
if ok then
    print("PASS: Removed entry")
    count = builder:entrycount()
    print("Entry count after removal: " .. count)
    if count == 1 then
        print("PASS: Entry count is 1 after removal")
    else
        print("FAIL: Expected 1 entry, got " .. count)
    end
else
    print("FAIL: Failed to remove entry: " .. tostring(err))
end

-- Test 9: Write tree to repository
print("\n--- Test 9: Write tree to repository ---")
local tree_id, err = builder:write()
if tree_id then
    print("PASS: Wrote tree with ID: " .. tostring(tree_id))
else
    print("FAIL: Failed to write tree: " .. tostring(err))
end

-- Test 10: Verify tree can be looked up
print("\n--- Test 10: Verify tree can be looked up ---")
if tree_id then
    local tree, err = git2.Tree.lookup(repo, tree_id)
    if tree then
        print("PASS: Successfully looked up tree")
        local entry_count = tree:entrycount()
        print("  Tree has " .. entry_count .. " entries")
        if entry_count == 1 then
            print("PASS: Tree has correct number of entries")
        else
            print("FAIL: Expected 1 entry in tree, got " .. entry_count)
        end
    else
        print("FAIL: Failed to look up tree: " .. tostring(err))
    end
end

-- Test 11: Create TreeBuilder from existing tree
print("\n--- Test 11: Create TreeBuilder from existing tree ---")
if tree_id then
    local tree = git2.Tree.lookup(repo, tree_id)
    if tree then
        local builder2, err = git2.TreeBuilder.create(repo, tree)
        if builder2 then
            print("PASS: Created TreeBuilder from existing tree")
            local count2 = builder2:entrycount()
            print("  Entry count: " .. count2)
            if count2 == 1 then
                print("PASS: Builder initialized with correct entry count")
            else
                print("FAIL: Expected 1 entry, got " .. count2)
            end
        else
            print("FAIL: Failed to create TreeBuilder from tree: " .. tostring(err))
        end
    end
end

-- Test 12: Clear builder
print("\n--- Test 12: Clear builder ---")
local ok, err = builder:clear()
if ok then
    print("PASS: Cleared builder")
    count = builder:entrycount()
    if count == 0 then
        print("PASS: Entry count is 0 after clear")
    else
        print("FAIL: Expected 0 entries after clear, got " .. count)
    end
else
    print("FAIL: Failed to clear builder: " .. tostring(err))
end

-- Test 13: Filter entries
print("\n--- Test 13: Filter entries ---")
-- Add multiple entries
local blob1, _ = git2.Blob.from_buffer(repo, "file1 content")
local blob2, _ = git2.Blob.from_buffer(repo, "file2 content")
local blob3, _ = git2.Blob.from_buffer(repo, "file3 content")
builder:insert("keep1.txt", blob1, git2.FILEMODE_BLOB)
builder:insert("remove.txt", blob2, git2.FILEMODE_BLOB)
builder:insert("keep2.txt", blob3, git2.FILEMODE_BLOB)
print("Before filter: " .. builder:entrycount() .. " entries")

local ok, err = builder:filter(function(entry)
    local name = entry:name()
    -- Return true to remove the entry
    return name == "remove.txt"
end)
if ok or err == nil then
    print("PASS: Filter executed")
    count = builder:entrycount()
    print("After filter: " .. count .. " entries")
    if count == 2 then
        print("PASS: Filter removed correct number of entries")
    else
        print("FAIL: Expected 2 entries after filter, got " .. count)
    end
    -- Verify the right entries remain
    if builder:get("keep1.txt") and builder:get("keep2.txt") and not builder:get("remove.txt") then
        print("PASS: Correct entries remain after filter")
    else
        print("FAIL: Wrong entries remain after filter")
    end
else
    print("FAIL: Filter failed: " .. tostring(err))
end

-- Test 14: Create a commit with the tree
print("\n--- Test 14: Create commit with TreeBuilder tree ---")
-- First create an initial empty tree + commit (to use as parent)
local empty_bld, create_err = git2.TreeBuilder.create(repo, nil)
if not empty_bld then
    print("FAIL: Could not create empty builder: " .. tostring(create_err))
    return
end
local empty_tree_id, write_err = empty_bld:write()
if not empty_tree_id then
    print("FAIL: Could not write empty tree: " .. tostring(write_err))
    return
end
local empty_tree, lookup_err = git2.Tree.lookup(repo, empty_tree_id)
if not empty_tree then
    print("FAIL: Could not lookup empty tree: " .. tostring(lookup_err))
    return
end
local initial_commit_id, commit_err = git2.Commit.create(
    repo, "HEAD", sig, sig, "",
    "Initial commit", empty_tree
)
if not initial_commit_id then
    print("FAIL: Could not create initial commit: " .. tostring(commit_err))
    return
end
local initial_commit = git2.Commit.lookup(repo, initial_commit_id)
local final_tree_id, _ = builder:write()
local final_tree, lookup_err = git2.Tree.lookup(repo, final_tree_id)
if not final_tree then
    print("FAIL: Could not lookup final tree: " .. tostring(lookup_err))
    return
end
local commit_id, err = git2.Commit.create(
    repo,           -- repo
    "HEAD",         -- update_ref
    sig,            -- author
    sig,            -- committer
    "",             -- message_encoding (empty = default)
    "Test commit with TreeBuilder", -- message
    final_tree,     -- tree
    initial_commit  -- parent
)
if commit_id then
    print("PASS: Created commit with ID: " .. tostring(commit_id))
else
    print("FAIL: Commit creation returned: " .. tostring(err))
end

-- Cleanup
print("\n=== Cleanup ===")
os.execute("rm -rf " .. test_dir)
print("Removed test directory")

print("\n=== TreeBuilder Module Tests Complete ===")
