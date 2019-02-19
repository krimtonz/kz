CC          = mips64-gcc
LD          = mips64-g++
OBJCOPY     = mips64-objcopy
ARMIPS      = armips
BUILDFILE   = build.asm

ADDRESS     = 0x80702B20
CFLAGS      = -c -MMD -MP -std=gnu11 -Wall -ffunction-sections -fdata-sections -O1 -fno-reorder-blocks
LDFLAGS     = -T kz-NZSE.ld -nostartfiles -specs=nosys.specs -Wl,--gc-sections -Wl,--defsym,start=$(ADDRESS)
SRCDIR      = src
OBJDIR      = obj
BINDIR      = bin
CFILES      = *.c
NAME        = kz
CSRC       := $(foreach s,$(CFILES),$(wildcard $(SRCDIR)/$(s)))
COBJ        = $(patsubst $(SRCDIR)/%,$(OBJDIR)/%.o,$(CSRC))
ELF         = $(BINDIR)/$(NAME).elf
BIN         = $(BINDIR)/$(NAME).bin
OUTDIR      = $(OBJDIR) $(BINDIR)

.PHONY: all patch clean

all: $(BIN)

 $(COBJ)    : $(OBJDIR)/%.o: $(SRCDIR)/% | $(OBJDIR)
	$(CC) $(CFLAGS) $< -o $@

$(ELF)      : $(COBJ) | $(BINDIR)
	$(LD) $(LDFLAGS) $^ -o $@

$(BIN)      : $(ELF) | $(BINDIR)
	$(OBJCOPY) -S -O binary $< $@

patch       : $(BIN)
	$(ARMIPS) $(BUILDFILE)

clean       :
	rm -rf $(OBJDIR) $(BINDIR)

$(OUTDIR)   : 
	mkdir -p $@