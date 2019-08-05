PACKAGE	   ?= $(NAME)
URL		   ?= github.com/krimtonz/kz
CC          = mips64-gcc
LD          = mips64-g++
AS			= mips64-gcc
OBJCOPY     = mips64-objcopy
ARMIPS      = armips
CFILES      = *.c
SFILES		= *.S
SRCDIR		= src
OBJDIR 		= obj
BINDIR		= bin
KZ_VERSIONS	= NZSE NZSJ
NAME		= kz

ADDRESS     = 0x80800020
CFLAGS      = -c -MMD -MP -std=gnu11 -Wall -ffunction-sections -fdata-sections -O1 -fno-reorder-blocks
CPPFLAGS	= -DPACKAGE=$(PACKAGE) -DURL=$(URL) -DF3DEX_GBI_2
LDFLAGS     = -nostartfiles -specs=nosys.specs -Wl,--gc-sections -Wl,--defsym,start=$(ADDRESS) 

KZ 			= $(foreach v,$(KZ_VERSIONS),patch-kz-$(v))
all			: $(KZ)
clean       :
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY		: all clean

define bin_template
SRCDIR-$(1)      = src
OBJDIR-$(1)      = obj/$(2)
BINDIR-$(1)      = bin/$(2)
NAME-$(1)        = $(1)
BUILDFILE-$(1)   = build.$(2).asm
CPPFLAGS-$(1)	 = -DZ2_VERSION=$(2) $(CPPFLAGS)
LDFLAGS-$(1)	 = -T kz-$(2).ld $(LDFLAGS)
CSRC-$(1)       := $$(foreach s,$$(CFILES),$$(wildcard $$(SRCDIR-$(1))/$$(s)))
COBJ-$(1)        = $$(patsubst $$(SRCDIR-$(1))/%,$$(OBJDIR-$(1))/%.o,$$(CSRC-$(1)))
SSRC-$(1)       := $$(foreach s,$$(SFILES),$$(wildcard $$(SRCDIR-$(1))/$$(s)))
SOBJ-$(1)		 = $$(patsubst $$(SRCDIR-$(1))/%,$$(OBJDIR-$(1))/%.o,$$(SSRC-$(1)))
ELF-$(1)         = $$(BINDIR-$(1))/$(3).elf
BIN-$(1)         = $$(BINDIR-$(1))/$(3).bin
OUTDIR-$(1)      = $$(OBJDIR-$(1)) $$(BINDIR-$(1))
BUILD-$(1)		 = $(1)
CLEAN-$(1)		 = clean-$(1)
$$(BUILD-$(1))       : $$(BIN-$(1))
$$(CLEAN-$(1))       :
	rm -rf $$(OUTDIR-$(1))
	
$$(COBJ-$(1))     : $$(OBJDIR-$(1))/%.o: $$(SRCDIR-$(1))/% | $$(OBJDIR-$(1))
	$(CC) $$(CPPFLAGS-$(1)) $$(CFLAGS) $$< -o $$@
$$(SOBJ-$(1))		: $$(OBJDIR-$(1))/%.o: $$(SRCDIR-$(1))/% | $$(OBJDIR-$(1))
	$(AS) -c -MMD -MP $$< -o $$@
$$(ELF-$(1))      : $$(COBJ-$(1)) $$(SOBJ-$(1)) | $$(BINDIR-$(1))
	$(LD) $$(LDFLAGS-$(1)) $$^ -o $$@
$$(BIN-$(1))      : $$(ELF-$(1)) | $$(BINDIR-$(1))
	$(OBJCOPY) -S -O binary $$< $$@
$$(OUTDIR-$(1))   : 
	mkdir -p $$@
patch-$(1)        : $$(BIN-$(1))
	$(ARMIPS) $$(BUILDFILE-$(1))
endef

$(foreach v,$(KZ_VERSIONS),$(eval $(call bin_template,kz-$(v),$(v),kz)))