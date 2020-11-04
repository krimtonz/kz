local args = { ... }
local in_rom
local opt_out_rom
local opt_lite = false

while(args[1]) do
    if args[1] == "-o" then
        opt_out_rom = args[2]
        table.remove(args, 1)
        table.remove(args, 1)
    elseif args[1] == "lite" then
        opt_lite = "lite"
        table.remove(args, 1)
    else
        in_rom = args[1]
        table.remove(args, 1)
    end
end

local make = loadfile("build/lua/make.lua")
local patched_rom, rom, rom_info, kz_version = make(in_rom, opt_lite)

if(opt_out_rom ~= nil) then
    print("Saving " .. opt_out_rom)
    patched_rom:save(opt_out_rom)
else
    print("Saving build/" .. kz_version .. ".z64")
    patched_rom:save("build/" .. kz_version .. ".z64")
end