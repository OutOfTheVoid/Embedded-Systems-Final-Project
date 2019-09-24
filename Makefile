#######################################
# user configuration:
#######################################
# TARGET: name of the output file
TARGET = main
# MCU: part number to build for
MCU = TM4C123GH6PM
# SOURCES: list of input source sources
FREERTOS_SOURCES = freertos/queue.c freertos/list.c freertos/tasks.c freertos/timers.c freertos/portable/GCC/ARM_CM4F/port.c freertos/portable/MemMang/heap_1.c
SOURCES = main.c gcc_startup.c isr.c clock.c dma.c memcpy.c audio_task.c keyboard_task.c serial_task.c tlv5616_sound.c SSD2119.c $(FREERTOS_SOURCES)
# INCLUDES: list of includes, by default, use Includes directory
INCLUDES = -Isrc -Isrc/freertos -Isrc/freertos/portable/GCC/ARM_CM4F -Isrc/freertos/portable/MemMang
# OUTDIR: directory to use for output
OUTDIR = build
# ENTRY: entry point
ENTRY = _reset_isr

# LD_SCRIPT: linker script
LD_SCRIPT = $(MCU).ld

#tools
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
RM      = rm -f
MKDIR	= mkdir -p

# define flags
CFLAGS = -g -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
CFLAGS +=-Os -ffunction-sections -fdata-sections -MD -std=c99 -Wall
CFLAGS += -pedantic -DPART_$(MCU) $(INCLUDES) -c
CFLAGS += -DTARGET_IS_BLIZZARD_RA1
LDFLAGS = -T $(LD_SCRIPT) --entry $(ENTRY) --gc-sections ../tools/gcc-arm-none-eabi-8-2018-q4-major/arm-none-eabi/lib/thumb/v7e-m+fp/hard/libm.a ../tools/gcc-arm-none-eabi-8-2018-q4-major/arm-none-eabi/lib/thumb/v7e-m+fp/hard/libc.a ../tools/gcc-arm-none-eabi-8-2018-q4-major/lib/gcc/arm-none-eabi/8.2.1/thumb/v7e-m+fp/hard/libgcc.a

vpath %.c src src/freertos src/freertos/portable/GCC/ARM_CM4F src/freertos/portable/MemMang

# list of object files, placed in the build directory regardless of source path
OBJECTS = $(addprefix $(OUTDIR)/,$(notdir $(SOURCES:.c=.o)))

# default: build bin
all: $(OUTDIR)/$(TARGET).bin

$(OUTDIR)/%.o: %.c | $(OUTDIR)
	$(CC) -o $@ $^ $(CFLAGS)

$(OUTDIR)/a.out: $(OBJECTS)
	$(LD) -o $@ $^ $(LDFLAGS)

$(OUTDIR)/main.bin: $(OUTDIR)/a.out
	$(OBJCOPY) -O binary $< $@

# create the output directory
$(OUTDIR):
	$(MKDIR) $(OUTDIR)

clean:
	-$(RM) $(OUTDIR)/*
	
cbf: clean build flash
	
flash: $(OUTDIR)/$(TARGET).bin
	lm4flash $(OUTDIR)/$(TARGET).bin

.PHONY: all clean
