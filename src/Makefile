# Actuator v5.0 Makefile
# Ian Thompson <ijt5@cornell.edu>
# CUAUV 2013-14 Gemini

# List of commands:
# 'make' or 'make all': Builds all hex files listed in TARGETS
# 'make <target>.hex': Build a specific hex file
# 'make <target>.dis': Generate a disassembly of a target with debugging symbols enabled
# 'make clean': Deletes all built files, leaving just the sources
# 'make depclean': Removes the dependency list, used to determine what headers
# 	are used by what source files. You must call 'make' twice after this to
# 	build properly again
# 'make fuses': Sets the fuses on a connected MCU, according to above settings
# 'make flash.<target>': Flashes <target>.hex to the attached board,
# 	recompiling files as necessary

# Target rules:
# TARGETS is a list of hexfiles that can be built, of the form <target>.hex
# <target>_OBJS is a list of files required to build the target. Replace .c
# 	with .o when adding files to this list. Headers will be automatically
# 	tracked for files listed here.
# usp_OBJS are the files required for serial functionality, and can be included
# 	by adding $(usp_OBJS) to the target list
# CONF_FILE is the .conf file for serial communication. May be ommitted if
# 	serial is not used

TARGETS = actuator.hex serial.hex blinkey.hex
usp_OBJS = buffer.o packet.o uart.o
actuator_OBJS = actuator.o $(usp_OBJS)
blinkey_OBJS = blinkey.o
serial_OBJS = serial.o $(usp_OBJS)
CONF_FILE = actuator.conf

# Build flags:
# CFLAGS are compiler flags, LDFLAGS are linker flags
# -DF_CPU and -mmcu are required to build a program correctly
# -flto produces much better optimization, but may cause lesser versions of gcc
#  	to die. Removes debugging symbols and code from disassemblies. Also causes
#  	"misspelled signal handler" warnings during linking, which can be safely
#  	ignored (gcc bug #59396).
# -Wall -Wextra -Werror turns on all warnings and makes them into errors. May
#  	not be a good idea if you can't figure out compiler warnings
# -Os optimizes the code to be as small as possible. Probably what you want
CFLAGS += -flto -Wall -Wextra -Werror -Os -std=c99 -mmcu=atmega644 -DF_CPU=20000000
LDFLAGS += -flto -Os -mmcu=atmega644

# Tool definitions:
# Make sure DEFGEN points to the correct folder for the jarfile
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
DEFGEN = java -jar ../../../AVRCode/defgenerate/v2/dist/defgen.jar
AVRDUDE = avrdude -p m644 -c avrispmkII

# Fuse settings:
# Used by 'make fuses'
LFUSE = 0xf7
HFUSE = 0xd9
EFUSE = 0xff

# --------- DO NOT EDIT BELOW THIS LINE -------- #

DEPDIR = .deps
DEPS = $(patsubst %.c,$(DEPDIR)/%.d,$(wildcard *.c))

.PHONY: all clean depclean fuses

# DEPS prereq generates dependency files for new .c files
all: $(DEPS) $(TARGETS)

definitions.h: $(CONF_FILE)
	$(DEFGEN) $^

.SECONDEXPANSION:
%.bin: $$($$*_OBJS)
	$(LINK.o) -o $@ $^

%.hex: %.bin
	$(OBJCOPY) -O ihex $^ $@

%.do: CFLAGS += -g
%.do: %.c
	$(COMPILE.c) -o $@ $^

%.dbin: $$(subst .o,.do,$$($$*_OBJS))
	$(LINK.o) -o $@ $^

%.dis: %.dbin
	$(OBJDUMP) -d -S $^ > $@

# Need a flash target list so that they can be marked as PHONY
FLASH_TARGETS = $(patsubst %.hex,flash.%,$(filter %.hex,$(TARGETS)))
.PHONY: $(FLASH_TARGETS)
$(FLASH_TARGETS): flash.%: %.hex
	@echo Flashing $<
	$(AVRDUDE) -U flash:w:$<

fuses:
	$(AVRDUDE) -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m

clean:
	-rm -f *.o *.hex *.dis definitions.h *_usd.conf

# Dependency calculation and inclusion
$(DEPS): $$(patsubst $(DEPDIR)/%.d,%.c,$$@) | $(DEPDIR)
	$(COMPILE.c) -MF"$@" -MG -MM -MT"$(<:.c=.o)" $<

-include $(DEPDIR)/*.d

$(DEPDIR):
	mkdir $(DEPDIR)

depclean:
	-rm -rf $(DEPDIR)
