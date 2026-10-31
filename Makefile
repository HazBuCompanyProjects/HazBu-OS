AS = nasm
CC = gcc
LD = ld

BUILD_DIR = build
KERNEL_ELF = $(BUILD_DIR)/hazbu-kernel.elf

CFLAGS = -m32 -std=gnu11 -ffreestanding -fno-pie -fno-pic -fno-stack-protector -fno-builtin -Wall -Wextra -O2

ASFLAGS = -f elf32

.PHONY: all clean

all: $(KERNEL_ELF)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/boot.o: kernel/boot/boot.asm | $(BUILD_DIR)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/kernel.o: kernel/src/kernel.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_ELF): $(BUILD_DIR)/boot.o $(BUILD_DIR)/kernel.o kernel/linker.ld
	$(LD) -m elf_i386 -T kernel/linker.ld -nostdlib -o $@ $(BUILD_DIR)/boot.o $(BUILD_DIR)/kernel.o

clean:
	rm -rf $(BUILD_DIR)
