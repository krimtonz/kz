# Installing kz

The supported  Versions are
* The Legend of Zelda: Majora's Mask (NTSC-U/1.0)
* The Legend of Zelda: Majora's Mask (NTSC-J/1.0)
* The Legend of Zelda: Majora's Mask (NTSC-J/1.1)

## Using kz for the Wii Virtual Console
Drag a compatible WiiVC Wad onto the `build-wad.bat` script

A patched WAD File will be created in the `build-wad.bat` script's directory.  The resulting wad will be called kz-NZS[J,E].wad.  You can install this WAD along side your vanilla Majora's Mask WADs and they will not be overwritten. 

## Using kz for the N64, WiiUVC, and emulators
Drag a compatible ROM onto the `build-rom-lite.bat` script

A patched ROM File will be created in the `build-rom-lite.bat` script's directory.  The resulting ROM will be called kz-NZS[J,E,J10]-lite.z64.

**Note:** lite versions run a very limited amount of memory and are more prone to crashing than WiiVC or full versions.  

## Using kz for patched emulators
Drag a compatible ROM onto the `build-rom.bat` script

A patched ROM FIle will be created in the `build-rom.bat` script's directory.  The resulting ROM will be called kz-NZS[J,E,J10].z64.

**Note**:  Full version roms will only work on a custom project64 emulator.  Full version roms do not work on the N64.  