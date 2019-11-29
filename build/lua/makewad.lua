local arg = {...}
local gzinject = os.getenv("GZINJECT")
if(gzinject == nil or gzinject == "") then
    gzinject="gzinject"
end
print("Extracting " .. arg[1] .. "\n")
os.execute(gzinject .. 
            " -a extract" ..
            " -w \"" .. arg[1] .."\"" ..
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
os.execute(gzinject .. 
            " -d \"build/wadextract\"" ..
            " -k \"build/common-key.bin\"" .. 
            " -a pack" ..
            " -w \"build/" .. kz_version .. ".wad\"" ..
            " -i " .. title_id ..
            " -t \"" .. title .. "\"" ..
            " -p \"" .. patch_file .."\"" ..
            " -p \"" .. vc_fix_patch .. "\"" ..
            " -p \"patch/compress.gzi\"" ..
            " --dol-inject \"" .. vc_fix_inject .. "\"" ..
            " --dol-loading \"" .. vc_fix_addr .. "\"" ..
            " --dol-after 0" ..
            " --cleanup" ..
            " --verbose")