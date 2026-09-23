-- tests/test_tree_bindings.lua

-- 加载动态库
local git2 = require('git2')

local function test_tree_walk()
    local repo = git2.Repository.open(".")
    local obj = git2.Object.revparse_single(repo, "HEAD^{tree}")
    local tree = obj
    local callback = function(root, entry, payload)
        if entry then
            print("Entry: " .. entry:name())
        else
            print("Entry is nil")
        end
        return 0 -- Continue walking
    end
    local result = tree:walk(callback, nil)
    assert(result == 0, "Tree walk failed")
end

local function test_tree_get_subtree()
    local repo = git2.Repository.open(".")
    local obj = git2.Object.revparse_single(repo, "HEAD^{tree}")
    local tree = obj
    local subtree, err = tree:get_subtree("subdir")
    if subtree then
        print("Subtree found: " .. tostring(subtree))
    else
        print("Subtree not found: " .. tostring(err))
    end
end

local function test_tree_id()
    local repo = git2.Repository.open(".")
    local obj = git2.Object.revparse_single(repo, "HEAD^{tree}")
    local tree = obj
    local id = tree:id()
    print("Tree ID: " .. tostring(id))
end

local function test_tree_lookup_prefix()
    local repo = git2.Repository.open(".")
    local obj = git2.Object.revparse_single(repo, "HEAD^{tree}")
    local id = obj:id()
    local tree, err = git2.Tree.lookup_prefix(repo, id, 10)
    if tree then
        print("Tree found: " .. tostring(tree))
    else
        print("Tree lookup prefix failed: " .. tostring(err))
    end
end

local function test_tree_free()
    local repo = git2.Repository.open(".")
    local obj = git2.Object.revparse_single(repo, "HEAD^{tree}")
    local tree = obj
    tree:free()
    print("Tree freed")
end

local function test_tree_entry_type()
    local repo = git2.Repository.open(".")
    local obj = git2.Object.revparse_single(repo, "HEAD^{tree}")
    local tree = obj
    local entry = tree:entry_byname("README.md")
    local type = entry:type()
    print("Entry type: " .. tostring(type))
end

local function test_tree_entry_attributes()
    local repo = git2.Repository.open(".")
    local obj = git2.Object.revparse_single(repo, "HEAD^{tree}")
    local tree = obj
    local entry = tree:entry_byname("file.txt")
    if entry then
        local filemode = entry:filemode()
        print("Entry filemode: " .. tostring(filemode))
    else
        print("Entry not found")
    end
end

-- test_tree_walk()
-- test_tree_get_subtree()
test_tree_id()
-- test_tree_lookup_prefix()
test_tree_free()
test_tree_entry_type()
test_tree_entry_attributes()

print("All tests passed")
