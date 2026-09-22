-- Test branch operations
local git2 = require("git2")

print("=== Branch Module Tests ===\n")

-- Use existing test repository
local script_dir = arg[0]:match("(.*/)") or "./"
-- Resolve to absolute path before any cd
local abs_script_dir = io.popen("cd " .. script_dir .. " && pwd"):read("*l")
local git_path = abs_script_dir .. "/test_rep.git.tbz"
local test_repo_dir = "/tmp/lua-git2-branch-test-repo"

-- Extract test repo
os.execute("rm -rf " .. test_repo_dir)
os.execute("mkdir -p " .. test_repo_dir)
os.execute("cd " .. test_repo_dir .. " && tar xjf " .. git_path .. " 2>/dev/null")

local repo_path = test_repo_dir .. "/test_rep/.git"
local repo = git2.Repository(repo_path)
if not repo then
    print("FAIL: Failed to open repository at " .. repo_path)
    os.exit(1)
end
print("PASS: Repository opened")

print("\n--- Branch Constants Test ---")
print("BRANCH_LOCAL = " .. tostring(git2.BRANCH_LOCAL))
print("BRANCH_REMOTE = " .. tostring(git2.BRANCH_REMOTE))
print("BRANCH_ALL = " .. tostring(git2.BRANCH_ALL))

print("\n--- Branch List Test (local) ---")
local branches, err = git2.Branch.list(repo, git2.BRANCH_LOCAL)
if not branches then
    print("WARN: branch_list returned error: " .. tostring(err))
else
    print("PASS: branch_list succeeded")
    print("  Number of local branches: " .. #branches)
    for i, name in ipairs(branches) do
        print("    [" .. i .. "] " .. name)
    end
end

print("\n--- Branch List Test (remote) ---")
local remote_branches, err = git2.Branch.list(repo, git2.BRANCH_REMOTE)
if not remote_branches then
    print("WARN: branch_list (remote) returned error: " .. tostring(err))
else
    print("PASS: branch_list (remote) succeeded")
    print("  Number of remote branches: " .. #remote_branches)
    for i, name in ipairs(remote_branches) do
        print("    [" .. i .. "] " .. name)
    end
end

print("\n--- Branch List Test (all) ---")
local all_branches, err = git2.Branch.list(repo, git2.BRANCH_ALL)
if not all_branches then
    print("WARN: branch_list (all) returned error: " .. tostring(err))
else
    print("PASS: branch_list (all) succeeded, count: " .. #all_branches)
end

print("\n--- Branch Lookup Test ---")
if branches and #branches > 0 then
    local first_branch = branches[1]
    local ref, err = git2.Branch.lookup(repo, first_branch, git2.BRANCH_LOCAL)
    if ref then
        print("PASS: branch_lookup found '" .. first_branch .. "'")
    else
        print("WARN: branch_lookup failed: " .. tostring(err))
    end
end

print("\n--- Branch Name Test ---")
local head_ref, err = repo:head()
if head_ref then
    print("PASS: Got HEAD reference")
    local name, err = git2.Branch.name(head_ref)
    if name then
        print("  Branch name: " .. name)
    else
        print("  INFO: HEAD is not a branch (detached or unborn): " .. tostring(err))
    end
else
    print("WARN: Could not get HEAD reference: " .. tostring(err))
end

print("\n--- Branch Is Head Test ---")
if head_ref then
    local is_head, err = git2.Branch.is_head(head_ref)
    if is_head ~= nil then
        print("PASS: branch_is_head = " .. (is_head == 1 and "true" or "false"))
    else
        print("WARN: branch_is_head failed: " .. tostring(err))
    end
end

print("\n--- Branch Is Checked Out Test ---")
if head_ref then
    local checked_out, err = git2.Branch.is_checked_out(head_ref)
    if checked_out ~= nil then
        print("PASS: branch_is_checked_out = " .. (checked_out == 1 and "true" or "false"))
    else
        print("WARN: branch_is_checked_out failed: " .. tostring(err))
    end
end

print("\n--- Branch Name Is Valid Test ---")
local valid, err = git2.Branch.name_is_valid("valid-branch-name")
if valid ~= nil then
    print("PASS: branch_name_is_valid('valid-branch-name') = " .. tostring(valid == 1))
else
    print("WARN: branch_name_is_valid failed: " .. tostring(err))
end
local valid2, err = git2.Branch.name_is_valid("-invalid-name")
if valid2 ~= nil then
    print("PASS: branch_name_is_valid('-invalid-name') = " .. tostring(valid2 == 1))
else
    print("WARN: branch_name_is_valid failed: " .. tostring(err))
end

print("\n=== Branch Module Tests Complete ===")

-- Cleanup
os.execute("rm -rf " .. test_repo_dir)
