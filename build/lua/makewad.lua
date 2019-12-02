local arg = {...}

local opt_no_vc_fix
local in_wad

while arg[1] do
    if arg[1] == "-no-vc-fix" then
        opt_no_vc_fix = 1
    else
        in_wad = arg[1]
    end
    table.remove(arg,1)
end

local gzinject = os.getenv("GZINJECT")
if(gzinject == nil or gzinject == "") then
    gzinject="gzinject"
end
print("Extracting " .. in_wad .. "\n")
os.execute(gzinject .. 
            " -a extract" ..
            " -w \"" .. in_wad .."\"" ..
            " -d \"build/wadextract\"" ..
            " --cleanup" ..
            " --verbose" .. 
            " -k \"build/common-key.bin\"")
os.execute(gzinject .. 
            " -a romc" ..
            " -m \"build/wadextract/content5/romc\"" ..
            " -o \"build/rom.z64\"")

local make = loadfile("build/lua/makerom.lua")
local kz_version, patched_rom, patch_file, vc_fix_patch, vc_fix_inject, vc_fix_addr, title_id, title = make("build/rom.z64")
patched_rom:write(4,patched_rom)
patched_rom:write32be(0,0x08000000)
patched_rom:save("build/wadextract/content5/romc")
os.execute("rm -rf \"build/rom.z64\"")
print("Packing build/" .. kz_version .. ".wad\n")
local gzinject_pack = gzinject .. 
        " -d \"build/wadextract\"" ..
        " -k \"build/common-key.bin\"" .. 
        " -a pack" ..
        " -w \"build/" .. kz_version .. ".wad\"" ..
        " -i " .. title_id ..
        " -t \"" .. title .. "\"" ..
        " -p \"" .. patch_file .."\""

if(opt_no_vc_fix == nil or opt_no_vc_fix ~= 1) then
    local vcmake = os.execute("cd vc && make ../" .. vc_fix_inject .. "  && make ../" .. vc_fix_patch .. " && cd ..")
    if(vcmake ~= nil and vcmake == true) then
        gzinject_pack = gzinject_pack ..
            " -p \"" .. vc_fix_patch .. "\"" ..
            " --dol-inject \"" .. vc_fix_inject .. "\"" ..
            " --dol-loading \"" .. vc_fix_addr .. "\"" ..
            " --dol-after 0"
    end
end

gzinject_pack = gzinject_pack ..
        " -p \"patch/compress.gzi\"" ..
        " --cleanup" ..
        " --verbose"

os.execute(gzinject_pack)