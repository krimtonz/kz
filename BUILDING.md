# Prerequisites
In order to build kz, you need to have the [n64 toolchain](https://github.com/glankk/n64) installed.  

If you're planning on building for the Wii U VC, you will need to be build 
the toolchain with `CFLAGS_FOR_TARGET='-mno-check-zero-division' CXX_FLAGS_FOR_TARGET='-mno-check-zero-division'`


If building for the Wii U VC or the Wii VC, you will need the `--enable-vc` option when configuring the toolchain


In order to create a Wii Virtual Console WAD you will need [gzinject](https://github.com/krimtonz/gzinject)

kz has source code in the `vc` directory which will improve the WiiVC Emulator in regards to kz's code.  In order to build this you will need a PowerPC compiler.  [devkitPro](https://devkitpro.org/) contains an installable PowerPC toolchain called `devkitPPC`.  If you'd like to create your own toolchain see [wii-toolchain](https://github.com/krimtonz/wii-toolchain).  Building the vc improvements is optional, but recommended.

# Building 
To build all kz binaries, run `make all` to only build a sepecific version run `make kz-VERSION` Where VERSION is
`kz-NZSJ kz-NZSJ10 kz-NZSE kz-lite-NZSJ kz-lite-NZSJ10 kz-lite-NZSE`

# Building VC Enhancements
Run `make` in the `vc` directory in order to build vc enhancements.  This will also create the necessary gzinject patch file for including the enhancements

# Patching
The patching scripts will run the building process, so you can skip the building process if you plan on patching.  

#### N64, WiiUVC, and Standard Emulators
To build a ROM that will work on the Wii U VC, N64, and standard N64 Emulators, run

    build/makerom-lite /patch/to/vanilla/rom.z64

#### Full KZ Version ROM for custom Project64 
To build a full kz version rom run  
Note: this rom will only work on a modified [Project64](https://github.com/krimtonz/project64).  

    build/makerom /path/to/vanilla/rom.z64
    

To build a Wii VC Wad which contains the full
version of kz, run 

    build/makewad /path/to/vanilla/wad.wad

By default the wad patching script will attempt to build the vc enhacements mentioned above, if this part fails by not having a PowerPC compilier, the wad will be built without the enhancements.  

The wad building script will also attempt to include the [homeboy](https://github.com/krimtonz/homeboy) binary which is used for SD Card Access.  The binary compiled from the homeboy project should be placed in the `patch/dol/vc/kz-vc-NZS[E,J]` directory.