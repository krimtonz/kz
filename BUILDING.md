## Prerequisites
In order to build kz, you need to have the [n64 toolchain](https://github.com/glankk/n64) 
installed.  If you're planning on building for the Wii U VC, you will need to be build 
the toolchain with `CFLAGS_FOR_TARGET='-mno-check-zero-division' CXX_FLAGS_FOR_TARGET='-mno-check-zero-division'`
If building for the Wii U VC or the Wii VC, you will need the --enable-vc option when configuring the toolchain
In order to create a Wii Virtual Console WAD you will need [gzinject](https://github.com/krimtonz/gzinject)

## Building 
To build all kz binaries, run `make all` to only build a sepecific version run `make kz-VERSION` Where VERSION is
`kz-NZSJ kz-NZSJ10 kz-NZSE kz-lite-NZSJ kz-lite-NZSJ10 kz-lite-NZSE`

## Patching
In order to build a ROM that will work on the Wii U VC or N64, and standard N64 Emulators, run
`build/makerom-lite /patch/to/vanilla/rom.z64`, in order to build a full kz version rom
run `builld/makerom /path/to/vanilla/rom.z64` Note: this rom will only work on a modified
[Project64](https://github.com/krimtonz/project64).  To build a Wii VC Wad which contains the full
version of kz, run `build/makewad /path/to/vanilla/wad.wad`  The patching scripts will run
the building process, so you can skip the building process if you plan on patching.  