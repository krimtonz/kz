local arg = {...}
local gzinject = os.getenv("GZINJECT")
if(gzinject == nil or gzinject == "") then
    gzinject="gzinject"
end
os.execute(gzinject .. " -a extract -w \"" .. arg[1] .."\" -d build/wadextract --cleanup")
os.execute(gzinject .. " -a romc -m build/wadextract/content5/romc -o build/rom.z64")
local make = loadfile("build/makerom.lua")
local kz_version, patched_rom, patch_file, title_id, title = make("build/rom.z64")
patched_rom:write(4,patched_rom)
patched_rom:write32be(0,0x08000000)
patched_rom:save("build/wadextract/content5/romc")
os.execute(gzinject .. " -d build/wadextract -a pack -w build/" .. kz_version .. ".wad -i " .. title_id .. " -t \"" .. title .. "\" -p \"" .. patch_file .."\" --cleanup")