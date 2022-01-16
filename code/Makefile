#
# Comment out this line when using windows
#
AVR_TOOLS_PATH = /opt/cross/avr/bin/
AVR_BUNUTILS_PATH = /usr/avr/bin/


# Select the appropriate serial port for your system.

PORT                 = usb
#PORT                 = /dev/ttyACM0
#PORT                 = /dev/ttyUSB0
#PORT                 = COM3

TARGET               = nixiestat

SRC                  = nixiestat.cpp \
                       esquare.cpp \
                       clock.cpp \
                       datetime.cpp \
                       ds1307.cpp \
\
                       DataLogger.cpp \
                       TempCycle.cpp \
                       thermostat.cpp \
                       switches.cpp \
                       display.cpp \
                       SystemLogger.cpp \
                       IOPort.cpp \
                       UiStateMachine_Simple.cpp \
                       ui.cpp

OBJS                 = nixiestat.o \
                       esquare.o \
                       clock.o \
                       datetime.o \
                       ds1307.o \
\
                       DataLogger.o \
                       TempCycle.o \
                       thermostat.o \
                       switches.o \
                       display.o \
                       SystemLogger.o \
                       IOPort.o \
                       UiStateMachine_Simple.o \
                       ui.o


UPLOAD_RATE          = 57600
#AVRDUDE_PROGRAMMER   = stk500v1
#AVRDUDE_PROGRAMMER   = stk500v2
AVRDUDE_PROGRAMMER   = avrisp2
MCU                  = atmega1284p
AVRDUDE_MCU          = m1284
F_CPU                = 16000000

############################################################################
# Below here nothing should be changed...

ARDUINO        = core

AVR_SRC        =  $(ARDUINO)/pins_arduino.c \
                  $(ARDUINO)/twi.c \
                  $(ARDUINO)/wiring.c \
                  $(ARDUINO)/wiring_analog.c \
                  $(ARDUINO)/wiring_digital.c \
                  $(ARDUINO)/wiring_pulse.c \
                  $(ARDUINO)/wiring_serial.c \
                  $(ARDUINO)/wiring_shift.c \
                  $(ARDUINO)/WInterrupts.c

AVR_CXXSRC     =  $(ARDUINO)/EEPROM.cpp \
                  $(ARDUINO)/HardwareSerial.cpp \
                  $(ARDUINO)/menu.cpp \
                  $(ARDUINO)/OneWire.cpp \
                  $(ARDUINO)/PID.cpp \
                  $(ARDUINO)/TempSensors.cpp \
                  $(ARDUINO)/Wire.cpp \
                  $(ARDUINO)/WMath.cpp \
                  $(ARDUINO)/Print.cpp


FORMAT = ihex


# Name of this Makefile (used for "make depend").
MAKEFILE = Makefile

# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
DEBUG = stabs

OPT = s

COMPILER_PATH=/usr/avr/bin

# Place -D or -U options here
CDEFS = -DF_CPU=$(F_CPU)
CXXDEFS = -DF_CPU=$(F_CPU)

# Place -I options here
CINCS = -I$(ARDUINO)
CXXINCS = -I$(ARDUINO)

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
#CSTANDARD = -std=gnu99
CDEBUG = -g$(DEBUG)
#CWARN = -Wall -Wstrict-prototypes
CWARN = -Wall
CTUNING = -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
#CEXTRA = -Wa,-adhlns=$(<:.c=
CFLAGS = $(CDEBUG) $(COMPILER_PATH_DEF) $(CDEFS) $(CINCS) -mrelax -O$(OPT) $(CWARN) $(CSTANDARD) $(CEXTRA)
CXXFLAGS = $(COMPILER_PATH_DEF) $(CDEFS) $(CINCS) -mrelax -O$(OPT)
#ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs 
LDFLAGS = -Wl,-u,vfprintf -lprintf_flt -lm -Xlinker -Map=nixiestat.map 


# Programming support using avrdude. Settings and variables.
AVRDUDE_PORT = $(PORT)
#AVRDUDE_FLAGS =-V -q -C./core/avrdude.conf \

AVRDUDE_WRITE_FLASH = -Uflash:w:$(TARGET).hex:i
AVRDUDE_FLAGS =-V -q \
-p$(AVRDUDE_MCU) -P$(AVRDUDE_PORT) -c$(AVRDUDE_PROGRAMMER) \
-b$(UPLOAD_RATE)

# Program settings
CC = $(AVR_TOOLS_PATH)avr-gcc
CXX = $(AVR_TOOLS_PATH)avr-g++
OBJCOPY = $(AVR_BUNUTILS_PATH)objcopy
OBJDUMP = $(AVR_BUNUTILS_PATH)objdump
AR  = $(AVR_BUNUTILS_PATH)ar
SIZE = $(AVR_BUNUTILS_PATH)size
NM = $(AVR_BUNUTILS_PATH)nm
AVRDUDE = avrdude
REMOVE = rm -f
MV = mv -f

# Define all object files.
OBJ = $(AVR_SRC:.c=.o) $(AVR_CXXSRC:.cpp=.o) $(ASRC:.S=.o) 

# Define all listing files.
LST = $(ASRC:.S=.lst) $(AVR_CXXSRC:.cpp=.lst) $(AVR_SRC:.c=.lst)

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS)
ALL_CXXFLAGS = -mmcu=$(MCU) -I. $(CXXFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)


# Default target.
all: build sizeafter
#all: applet_files build sizeafter

build: elf hex 

applet_files: $(SRC)
	# Here is the "preprocessing".
	# It creates a .cpp file based with the same name as the .pde file.
	# On top of the new .cpp file comes the WProgram.h header.
	# At the end there is a generic main() function attached.
	# Then the .cpp file will be compiled. Errors during compile will
	# refer to this new, automatically generated, file. 
	# Not the original .pde file you actually edit...
	test -d applet || mkdir applet
	echo '#include "WProgram.h"' > $(TARGET).cpp
	cat $(TARGET).pde >> $(TARGET).cpp
	cat $(ARDUINO)/main.cxx >> $(TARGET).cpp

elf: $(TARGET).elf
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss 
sym: $(TARGET).sym

# Program the device.  
upload: all $(TARGET).hex $(TARGET).eep $(TARGET).lss $(TARGET).sym
	./avr-mem.sh $(TARGET).elf
	avrdude -q -p $(AVRDUDE_MCU) -b 115200 -P $(PORT) -c avrispv2 -v -e -U lock:w:0x3F:m -U hfuse:w:0xD8:m -U lfuse:w:0xe6:m -U efuse:w:0xfb:m ; \
avrdude -q -p $(AVRDUDE_MCU) -b 115200 -P $(PORT) -s -c avrispv2 -VD -U flash:w:$(TARGET).hex:i ; \
avrdude -q -p $(AVRDUDE_MCU) -b 115200 -P $(PORT) -s -c avrispv2 -U lock:w:0x0F:m l

#	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)


# Display size of file.
#HEXSIZE = $(SIZE) --target=$(FORMAT) $(TARGET).hex
#ELFSIZE = $(SIZE)  $(TARGET).elf
#sizebefore:
#	@if [ -f $(TARGET).elf ]; then echo; echo $(MSG_SIZE_BEFORE); $(HEXSIZE); echo; fi
#
#sizeafter:
#	@if [ -f $(TARGET).elf ]; then echo; echo $(MSG_SIZE_AFTER); $(HEXSIZE); echo; fi


# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT=$(OBJCOPY) --debugging \
--change-section-address .data-0x800000 \
--change-section-address .bss-0x800000 \
--change-section-address .noinit-0x800000 \
--change-section-address .eeprom-0x810000 


coff: $(TARGET).elf
	$(COFFCONVERT) -O coff-avr $(TARGET).elf $(TARGET).cof


extcoff: $(TARGET).elf
	$(COFFCONVERT) -O coff-ext-avr $(TARGET).elf $(TARGET).cof


.SUFFIXES: .elf .hex .eep .lss .sym

.elf.hex:
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

.elf.eep:
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create extended listing file from ELF output file.
.elf.lss:
	$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
.elf.sym:
	$(NM) -n $< > $@

	# Link: create ELF output file from library.
$(TARGET).elf: $(OBJS) core.a 
	$(CC) $(ALL_CFLAGS) -o $@ $(OBJS) -L. core.a $(LDFLAGS)

core.a: $(OBJ)
	@for i in $(OBJ); do echo $(AR) rcs core.a $$i; $(AR) rcs core.a $$i; done



# Compile: create object files from C++ source files.
.cpp.o:
	$(CXX) -c $(ALL_CXXFLAGS) $< -o $@ 

# Compile: create object files from C source files.
.c.o:
	$(CC) -c $(ALL_CFLAGS) $< -o $@ 


# Compile: create assembler files from C source files.
.c.s:
	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
.S.o:
	$(CC) -c $(ALL_ASFLAGS) $< -o $@



# Target: clean project.
clean:
	$(REMOVE) $(TARGET).hex $(TARGET).eep $(TARGET).cof $(TARGET).elf \
	$(TARGET).map $(TARGET).sym $(TARGET).lss $(OBJS) core.a \
	$(OBJ) $(LST) $(AVR_SRC:.c=.s) $(AVR_SRC:.c=.d) $(AVR_CXXSRC:.cpp=.s) $(AVR_CXXSRC:.cpp=.d)
   

depend:
	if grep '^# DO NOT DELETE' $(MAKEFILE) >/dev/null; \
	then \
		sed -e '/^# DO NOT DELETE/,$$d' $(MAKEFILE) > \
			$(MAKEFILE).$$$$ && \
		$(MV) $(MAKEFILE).$$$$ $(MAKEFILE); \
	fi
	echo '# DO NOT DELETE THIS LINE -- make depend depends on it.' \
		>> $(MAKEFILE); \
	$(CC) -M -mmcu=$(MCU) $(CDEFS) $(CINCS) $(SRC) $(ASRC) >> $(MAKEFILE)

.PHONY:	all build elf hex eep lss sym program coff extcoff clean depend applet_files sizebefore sizeafter
# DO NOT DELETE THIS LINE -- make depend depends on it.
