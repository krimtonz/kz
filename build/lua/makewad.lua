local arg = {...}

local opt_no_vc_fix
local opt_raphnet
local opt_out_wad
local opt_lite
local in_wad

while arg[1] do
    if arg[1] == "--no-vc-fix" then
        opt_no_vc_fix = 1
        table.remove(arg, 1)
    elseif arg[1] == "--raphnet" then
        opt_raphnet = 1
        table.remove(arg, 1)
    elseif arg[1] == "--outwad" then
        opt_out_wad = arg[2]
        table.remove(arg, 1)
        table.remove(arg, 1)
    elseif arg[1] == "lite" then
        opt_lite = true
        table.remove(arg, 1)
    else
        in_wad = arg[1]
        table.remove(arg, 1)
    end
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

if opt_lite == nil then
    opt_lite = false
end

wiivc = true
local make = loadfile("build/lua/make.lua")
local patched_rom, rom, rom_info, kz_version = make("build/rom.z64", opt_lite)

patched_rom:write(4, patched_rom)
patched_rom:write32be(0, 0x08000000)
patched_rom:save("build/wadextract/content5/romc")

os.execute("rm -rf \"build/rom.z64\"")
print("Packing build/" .. kz_version .. ".wad\n")

local out_wad = "build/" .. kz_version .. ".wad"
if(opt_out_wad ~= nil) then
    out_wad = opt_out_wad
end

local emulator_patch = rom_info.wad_patch
if(opt_lite == true) then
    emulator_patch = emulator_patch .. "-lite"
end
emulator_patch = emulator_patch .. ".gzi"

local mapping_patch = nil
if(opt_raphnet ~= nil and opt_raphnet == 1) then
    mapping_patch = rom_info.wad_patch .. "-raphnet.gzi"
    out_wad = out_wad:gsub(".wad","-raphnet.wad")
else
    mapping_patch = rom_info.wad_patch .. "-remap.gzi"
end

local gzinject_pack = gzinject ..
        " -d \"build/wadextract\"" ..
        " -k \"build/common-key.bin\"" ..
        " -a pack" ..
        " -w \"" .. out_wad .. "\"" ..
        " -i " .. rom_info.title_id ..
        " -t \"" .. rom_info.title .. "\"" ..
        " -p \"" .. emulator_patch .."\"" ..
        " -p \"" .. mapping_patch .. "\""

if((opt_no_vc_fix == nil or opt_no_vc_fix ~= 1)) then
    local vcmake = os.execute("make " .. rom_info.vc_fix_inject .. "/homeboy.bin")
    if(vcmake ~= nil and vcmake == true) then
        gzinject_pack = gzinject_pack ..
            " -p \"" .. rom_info.vc_fix_inject .. "/kz-vc.gzi\"" ..
            " --dol-inject \"" .. rom_info.vc_fix_inject .. "/kz-vc.bin\"" ..
            " --dol-loading \"" .. rom_info.vc_fix_addr .. "\"" ..
            " --dol-inject \"" .. rom_info.vc_fix_inject .. "/homeboy.bin\"" ..
            " --dol-loading \"" .. rom_info.hb_addr .. "\"" ..
            " --dol-after 1"
    else
        print("Could not build vc patches\n")
        return
    end
end

gzinject_pack = gzinject_pack ..
        " -p \"patch/gzi/compress.gzi\"" ..
        " --cleanup" ..
        " --verbose"

os.execute(gzinject_pack)