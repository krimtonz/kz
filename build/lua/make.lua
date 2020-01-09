require("build/lua/rom_info")

local arg={ ... }

local rom = gru.n64rom_load(arg[1])
local rom_info = roms[rom:crc32()]

-- We don't have an identiable rom here, stop patching
if rom_info == nil then
    error("invalid rom", 0)
    return 1
end

local suffix = ""
local ram = 0x80800000

-- arg[2] is true if building the lite version
if arg[2] == true then
    suffix = "-lite"
    ram = 0x806E0000
end

local kz_version = "kz" .. suffix .. "-" .. rom_info.rom_id
print("Building " .. kz_version)

local make = "make "
if(wiivc) then
    make = "VCCPPFLAGS='-DWIIVC' " .. make
end
local _,_,res = os.execute(make .. 
                           kz_version ..
                           " patch/gsc/" .. kz_version .. "/hooks.gsc")
if res ~= 0 then
    error("could not build kz", 0)
end

local fs = gru.z64fs_load_blob(rom)
local code = fs:get(rom_info.code)
local hooks = gru.gsc_load("patch/gsc/" .. kz_version .. "/hooks.gsc")

print("Applying hooks")
hooks:shift(-rom_info.code_ram)
hooks:apply_be(code)

print("Applying size patch")
local ups = gru.gsc_load("patch/gsc/" .. kz_version .. "/ups_size_patch.gsc")
ups:shift(-rom_info.code_ram)
ups:apply_be(code)

print("Assembling rom")
fs:replace(rom_info.code, code, fs:compressed(rom_info.code))
local patched_rom = fs:assemble_rom()

print("Applying memory hack")
local zero_patch
if arg[2] == true then
    zero_patch = gru.gsc_load("patch/gsc/zero_patch_lite.gsc")
else
    zero_patch = gru.gsc_load("patch/gsc/zero_patch.gsc")
end
zero_patch:apply_be(patched_rom)

local prev_ldr = patched_rom:copy(0x1000, 0x60)
local kz = gru.blob_load("bin/" .. kz_version .. "/kz.bin")
local payload_rom = fs:prom_tail()

print("Building Loader")
local _,_,res = os.execute(string.format("make CPPFLAGS='-DROM=0x%08x -DRAM=0x%08x -DSIZE=0x%08x'" ..
                                         " ldr-" .. kz_version, 
                                         payload_rom,
                                         ram,
                                         kz:size() + 0x60))
if(res ~= 0) then
    error("Could not build loader", 0)
end

local ldr = gru.blob_load("bin/ldr-" .. kz_version .. "/ldr.bin")

print("Inserting payload")
patched_rom:write(0x1000, ldr)
patched_rom:write(payload_rom, prev_ldr)
patched_rom:write(payload_rom + 0x60, kz)
patched_rom:crc_update()

return patched_rom, rom, rom_info, kz_version