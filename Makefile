# BueOS Makefile
# BueOS构建文件
# 使用NASM和Clang编译

# 工具定义
NASM := nasm
CLANG := clang
LD := ld
OBJCOPY := objcopy

# 编译选项
NASMFLAGS := -f bin
CFLAGS := -target i386-pc-none-elf -nostdlib -ffreestanding \
          -Wall -Wextra -Werror -pedantic \
          -Wno-unused-parameter -Wno-unused-function \
          -Wno-language-extension-token
LDFLAGS := -nostdlib -static -m elf_i386

# 目标文件
BOOTLOADER := boot/boot.bin
KERNEL := kernel/kernel.bin
OS_IMAGE := bueos.img

# 默认目标
all: $(OS_IMAGE)

# 构建bootloader
$(BOOTLOADER): boot/boot.asm
	$(NASM) $(NASMFLAGS) $< -o $@

# 构建内核
kernel/main.o: kernel/main.c kernel/gdt_idt.h kernel/scheduler.h kernel/mm/paging.h
	$(CLANG) $(CFLAGS) -c $< -o $@

kernel/gdt_idt.o: kernel/gdt_idt.c kernel/gdt_idt.h kernel/mm/paging.h
	$(CLANG) $(CFLAGS) -c $< -o $@

kernel/gdt_idt_asm.o: kernel/gdt_idt.asm
	$(NASM) -f elf32 $< -o $@

kernel/scheduler.o: kernel/scheduler.c kernel/scheduler.h kernel/mm/paging.h
	$(CLANG) $(CFLAGS) -c $< -o $@

kernel/mm/paging.o: kernel/mm/paging.c kernel/mm/paging.h kernel/gdt_idt.h
	$(CLANG) $(CFLAGS) -c $< -o $@

$(KERNEL): kernel/main.o kernel/gdt_idt.o kernel/gdt_idt_asm.o kernel/scheduler.o kernel/mm/paging.o
	$(LD) $(LDFLAGS) -T kernel/linker.ld $^ -o $@
	$(OBJCOPY) -O binary $@ $@

# 创建OS镜像
$(OS_IMAGE): $(BOOTLOADER) $(KERNEL)
	dd if=/dev/zero of=$@ bs=512 count=2880
	dd if=$(BOOTLOADER) of=$@ conv=notrunc
	dd if=$(KERNEL) of=$@ seek=1 conv=notrunc

# 清理
clean:
	rm -f $(BOOTLOADER) $(KERNEL) $(OS_IMAGE) kernel/*.o kernel/mm/*.o

.PHONY: all clean
