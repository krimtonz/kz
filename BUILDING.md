# Prerequisites
## Toolchain
In order to build kz, you need to have the [n64 toolchain](https://github.com/glankk/n64) installed.

If you're planning on building for the Wii U VC, you will need to be build
the toolchain with `CFLAGS_FOR_TARGET='-mno-check-zero-division' CXX_FLAGS_FOR_TARGET='-mno-check-zero-division'`

If building for the Wii U VC or the Wii VC, you will need the `--enable-vc` option when configuring the toolchain

## libUndermine
kz requires [libundermine-f3dex2](https://github.com/krimtonz/libundermine) This is also provided as a git submodule via `git submodule update --init`

To build the libunderime-f3dex2 submodule run `make libum`

## gzinject
In order to create a Wii Virtual Console WAD you will need [gzinject](https://github.com/krimtonz/gzinject)

# Building
To build all kz binaries, run `make all` to only build a sepecific version run `make kz-VERSION` Where VERSION is
`kz-NZSJ kz-NZSJ10 kz-NZSE kz-lite-NZSJ kz-lite-NZSJ10 kz-lite-NZSE`

# Patching
The patching scripts will run the building process, so you can skip the building process if you plan on patching.

## N64, WiiUVC, and Standard Emulators
To build a ROM that will work on the Wii U VC, N64, and standard N64 Emulators, run

    build/makerom-lite /patch/to/vanilla/rom.z64

## Full KZ Version ROM for custom Project64
To build a full kz version rom run
Note: this rom will only work on a modified [Project64](https://drive.google.com/file/d/1Ttd0sLqYF2VGZsFOwcWVT7issqgVeyo-/view).

    build/makerom /path/to/vanilla/rom.z64


To build a Wii VC Wad which contains the full
version of kz, run

    build/makewad /path/to/vanilla/wad.wad

By default the script will build the vc patches in the `vc` directory.  These patches are not required, but are recommended.  Building the patches will require the `homeboy` submodule.

    git submodule update --init

This will also require a compatible PowerPC&trade; Compiler.  See [devkitPro](https://devkitpro.org/) for a prebuilt toolchain.

In order to exclude the VC patches pass `--no-vc-fix` to the `makewad` script.