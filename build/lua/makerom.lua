require("build/lua/rom_info")
local arg={...}
local rom = gru.n64rom_load(arg[1])
local rom_info = roms[rom:crc32()]
if rom_info == nil then
    print("invalid rom")
    return 1
end
local suffix = ""
local ram = 0x80800000
if arg[2] == "lite" then
    suffix = "-lite"
    ram = 0x806E0000
end
local kz_version = "kz" .. suffix .. "-" .. rom_info.rom_id
print("Building " .. kz_version)
local _,_,res = os.execute("make " .. kz_version .. " && make patch/" .. kz_version .. "/hooks.gsc")
local fs = gru.z64fs_load_blob(rom)
local code = fs:get(rom_info.code)
local hooks = gru.gsc_load("patch/" .. kz_version .. "/hooks.gsc")
print("Applying hooks")
hooks:shift(-rom_info.code_ram)
hooks:apply_be(code)
print("Assembling rom")
fs:replace(rom_info.code,code,fs:compressed(rom_info.code))
local patched_rom = fs:assemble_rom()
print("Applying memory hack")
local zero_patch
if arg[2] == "lite" then
    zero_patch = gru.gsc_load("patch/zero_patch_lite.gsc")
else
    zero_patch = gru.gsc_load("patch/zero_patch.gsc")
end
zero_patch:apply_be(patched_rom)
local prev_ldr = patched_rom:copy(0x1000,0x60)
local kz = gru.blob_load("bin/" .. kz_version .. "/kz.bin")
local payload_rom = fs:prom_tail()
print("Building Loader")
local _,_,res = os.execute(string.format("make CPPFLAGS='-DROM=0x%08x -DRAM=0x%08x -DSIZE=0x%08x' ldr-" .. kz_version, payload_rom, ram, kz:size() + 0x60))
local ldr = gru.blob_load("bin/ldr-" .. kz_version .. "/ldr.bin")
print("Inserting payload")
patched_rom:write(0x1000,ldr)
patched_rom:write(payload_rom,prev_ldr)
patched_rom:write(payload_rom + 0x60,kz)
patched_rom:crc_update()
print("Saving build/" .. kz_version .. ".z64")
patched_rom:save("build/" .. kz_version .. ".z64")
return kz_version, patched_rom, rom_info.wad_patch, rom_info.hb_addr, rom_info.vc_fix_inject, rom_info.vc_fix_addr, rom_info.title_id, rom_info.title