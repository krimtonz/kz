CC          = mips64-gcc
LD          = mips64-g++
AS			= mips64-gcc
OBJCOPY     = mips64-objcopy
ARMIPS      = armips
BUILDFILE   = build.asm

ADDRESS     = 0x806D1020
CFLAGS      = -c -MMD -MP -std=gnu11 -Wall -ffunction-sections -fdata-sections -O1 -fno-reorder-blocks
LDFLAGS     = -T kz-NZSE.ld -nostartfiles -specs=nosys.specs -Wl,--gc-sections -Wl,--defsym,start=$(ADDRESS)
SRCDIR      = src
OBJDIR      = obj
BINDIR      = bin
CFILES      = *.c
SFILES		= *.S
NAME        = kz
CSRC       := $(foreach s,$(CFILES),$(wildcard $(SRCDIR)/$(s)))
COBJ        = $(patsubst $(SRCDIR)/%,$(OBJDIR)/%.o,$(CSRC))
SSRC	   := $(foreach s,$(SFILES),$(wildcard $(SRCDIR)/$(s)))
SOBJ		= $(patsubst $(SRCDIR)/%,$(OBJDIR)/%.o,$(SSRC))
ELF         = $(BINDIR)/$(NAME).elf
BIN         = $(BINDIR)/$(NAME).bin
OUTDIR      = $(OBJDIR) $(BINDIR)

.PHONY: all patch clean

all: $(BIN)

$(COBJ)     : $(OBJDIR)/%.o: $(SRCDIR)/% | $(OBJDIR)
	$(CC) $(CFLAGS) $< -o $@

$(SOBJ)		: $(OBJDIR)/%.o: $(SRCDIR)/% | $(OBJDIR)
	$(AS) -c -MMD -MP $< -o $@

$(ELF)      : $(COBJ) $(SOBJ) | $(BINDIR)
	$(LD) $(LDFLAGS) $^ -o $@

$(BIN)      : $(ELF) | $(BINDIR)
	$(OBJCOPY) -S -O binary $< $@

patch       : $(BIN)
	$(ARMIPS) $(BUILDFILE)

clean       :
	rm -rf $(OBJDIR) $(BINDIR)

$(OUTDIR)   : 
	mkdir -p $@