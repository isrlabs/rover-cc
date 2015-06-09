#############
# TOOLCHAIN #
#############

CC =		avr-gcc
CPP =		avr-g++
LD =		avr-ld
STRIP =		avr-strip
OBJCOPY =	avr-objcopy
SIZE =		avr-size


#########
# BUILD #
#########

TARGET =	rover
SRCDIR =	src
BUILDDIR =	build
SOURCES = 	hardware/hardware.cc	\
		hardware/uart.cc 	\
		hardware/pwm.cc 	\
		hardware/drivetrain.cc 	\
		hardware/mast.cc


##############
# PARAMETERS #
##############

PROGRAMMER =	avrisp2
MCU =		atmega2560
PART =		m2560
F_CPU =		16000000
BAUD =		9600
PORT =		$(shell ls /dev/ttyACM* | head -1)
BINFORMAT =	ihex
PGMPROTO =	wiring
CPPFLAGS =	-Os -Werror -Wall -DF_CPU=$(F_CPU) -mmcu=$(MCU) -Isrc -DBAUD=$(BAUD) -g
AVRDUDE =	avrdude -D -v -p $(PART) -c $(PROGRAMMER) -P $(PORT)
SOURCES :=	$(addprefix $(SRCDIR)/, $(SOURCES))
BUILDTARGET :=	$(addprefix $(BUILDDIR)/, $(TARGET))

all: $(BUILDDIR) $(BUILDTARGET).hex

$(BUILDDIR):
	mkdir $@

show-sources:
	echo $(SOURCES)

show-upload-port:
	echo $(PORT)

.elf.hex:

$(BUILDTARGET).hex: $(BUILDTARGET).elf
	$(OBJCOPY) -O $(BINFORMAT) -R .eeprom $(BUILDTARGET).elf	\
	    $(BUILDTARGET).hex
	$(SIZE) --mcu $(MCU) -C $(BUILDTARGET).elf

$(BUILDTARGET).elf: $(BUILDDIR) $(SRCDIR)/$(TARGET).cc $(SOURCES)
	$(CPP) $(CPPFLAGS) -o $(BUILDTARGET).elf $(SOURCES)	\
	    $(SRCDIR)/$(TARGET).cc
	$(STRIP) $(BUILDTARGET).elf

upload: $(BUILDTARGET).hex
	$(AVRDUDE) -U flash:w:$(BUILDTARGET).hex 2>/dev/null 1>/dev/null

clean:
	rm -f *.hex *.elf *.eeprom
	rm -rf $(BUILDDIR)

.PHONY: all clean upload
