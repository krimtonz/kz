CC          = powerpc-eabi-gcc
LD          = powerpc-eabi-g++
AS			= powerpc-eabi-gcc -x assembler-with-cpp
OBJCOPY     = powerpc-eabi-objcopy --set-section-flags .bss=alloc,load,content
GENHOOKS	= CPPFLAGS='$(subst ','\'',$(CPPFLAGS))' ./genhooks
CFILES      = *.c
SFILES		= *.S
SRCDIR		= src
OBJDIR 		= obj
BINDIR		= ../patch/dol/vc
VC_VERSIONS	= NARJ NARE
NAME		= kz-vc
ADDRESS		=

ALL_CFLAGS      = -c -mcpu=750 -meabi -mhard-float -G 0 -O3 -ffunction-sections -fdata-sections $(CFLAGS)
ALL_CPPFLAGS	= $(CPPFLAGS)
ALL_LDFLAGS     = -T vc.ld -G 0 -nostartfiles -specs=nosys.specs $(LDFLAGS)

VC 			= $(foreach v,$(VC_VERSIONS),kz-vc-$(v))

VC-NARJ	= $(OBJ-kz-vc-NARJ) $(ELF-kz-vc-NARJ) $(HOOKS-kz-vc-NARJ)
VC-NARE	= $(OBJ-kz-vc-NARE) $(ELF-kz-vc-NARE) $(HOOKS-kz-vc-NARE)

all			: $(VC)
clean       :
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY		: all clean

define bin_template
SRCDIR-$(1)      = $(4)
OBJDIR-$(1)      = obj/$(1)
BINDIR-$(1)      = $$(BINDIR)/$(1)
NAME-$(1)        = $(1)
CSRC-$(1)       := $$(foreach s,$$(CFILES),$$(wildcard $$(SRCDIR-$(1))/$$(s)))
COBJ-$(1)        = $$(patsubst $$(SRCDIR-$(1))/%,$$(OBJDIR-$(1))/%.o,$$(CSRC-$(1)))
SSRC-$(1)       := $$(foreach s,$$(SFILES),$$(wildcard $$(SRCDIR-$(1))/$$(s)))
SOBJ-$(1)		 = $$(patsubst $$(SRCDIR-$(1))/%,$$(OBJDIR-$(1))/%.o,$$(SSRC-$(1)))
OBJ-$(1)		 = $$(COBJ-$(1)) $$(SOBJ-$(1))
ELF-$(1)         = $$(BINDIR-$(1))/$(3).elf
BIN-$(1)         = $$(BINDIR-$(1))/$(3).bin
OUTDIR-$(1)      = $$(OBJDIR-$(1)) $$(BINDIR-$(1))
HOOKS-$(1)		 = $$(BINDIR-$(1))/$(3).gzi
BUILD-$(1)		 = $(1)
CLEAN-$(1)		 = clean-$(1)
$$(ELF-$(1))		 : LDFLAGS += -Wl,--section-start,.init=$$(ADDRESS) -Wl,--defsym,start=$$(ADDRESS)
$$(BUILD-$(1))       : $$(BIN-$(1)) $$(HOOKS-$(1))
$$(CLEAN-$(1))       :
	rm -rf $$(OUTDIR-$(1))

$$(COBJ-$(1))     : $$(OBJDIR-$(1))/%.o: $$(SRCDIR-$(1))/% | $$(OBJDIR-$(1))
	$(CC) $$(ALL_CPPFLAGS) $$(ALL_CFLAGS) $$< -o $$@
$$(SOBJ-$(1))		: $$(OBJDIR-$(1))/%.o: $$(SRCDIR-$(1))/% | $$(OBJDIR-$(1))
	$(AS) -c -MMD -MP $$(ALL_CPPFLAGS) $$< -o $$@
$$(ELF-$(1))      : $$(OBJ-$(1)) | $$(BINDIR-$(1))
	$(LD) $$(ALL_LDFLAGS) $$^ -o $$@
$$(BIN-$(1))      : $$(ELF-$(1)) | $$(BINDIR-$(1))
	$(OBJCOPY) -S -O binary $$< $$@
$$(OUTDIR-$(1))   :
	mkdir -p $$@
$$(HOOKS-$(1))	: $$(ELF-$(1)) $$(BIN-$(1)) | $$(BINDIR-$(1))
	$$(GENHOOKS) $$^ >$$@
endef

$(foreach v,$(VC_VERSIONS),$(eval $(call bin_template,kz-vc-$(v),$(v),kz-vc,src)))

$(VC-NARJ)  	: CPPFLAGS	?=	-DVC_VERSION=NARJ
$(VC-NARJ)		: ADDRESS	:=	0x80212320
$(VC-NARE)		: CPPFLAGS	?=	-DVC_VERSION=NARE
$(VC-NARE)		: ADDRESS	:=	0x8020d4a0