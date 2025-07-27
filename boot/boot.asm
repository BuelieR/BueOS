; BueOS Bootloader
; 启动加载程序
; 使用NASM汇编编写
; 编译命令: nasm -f bin boot.asm -o boot.bin

org 0x7C00      ; BIOS加载引导扇区到0x7C00
bits 16         ; 16位实模式

start:
    cli         ; 禁用中断
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00  ; 设置栈指针

    sti         ; 启用中断

    ; 显示启动信息
    mov si, msg_loading
    call print_string

    ; 等待键盘输入继续
    call wait_key

    ; 检查1MB内存是否可用
    mov ax, 0x1000
    mov es, ax
    mov word [es:0], 0x55AA
    cmp word [es:0], 0x55AA
    jne memory_error

    ; 加载内核到1MB地址 (0x100000)
    ; Load kernel to 1MB address
    mov ah, 0x02    ; BIOS read sector function
    mov al, 10      ; Number of sectors to read
    mov ch, 0       ; Cylinder number
    mov cl, 2       ; Sector number
    mov dh, 0       ; Head number
    mov dl, 0x80    ; Drive number
    mov bx, 0x1000  ; ES = 0x1000
    mov es, bx
    xor bx, bx      ; BX = 0
    int 0x13        ; BIOS disk interrupt
    jc disk_error   ; Jump if error

    ; 验证内核加载
    mov si, msg_kernel_loaded
    call print_string
    
    ; 检查内核魔数 (前4字节)
    mov ax, 0x1000
    mov es, ax
    cmp dword [es:0], 0x1BADB002 ; 内核魔数
    jne kernel_error
    
    ; 设置GDT
    cli
    lgdt [gdt_descriptor]
    
    ; 切换到保护模式
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    
    ; 远跳转刷新CS
    jmp 0x08:.protected_mode
    
.protected_mode:
    ; 设置数据段寄存器
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; 跳转到内核 (0x100000)
    mov si, msg_jumping
    call print_string
    jmp 0x08:0x100000
    
; 错误处理函数
disk_error:
    mov si, msg_disk_error
    call print_string
    jmp $

memory_error:
    mov si, msg_memory_error
    call print_string
    jmp $

kernel_error:
    mov si, msg_kernel_error
    call print_string
    jmp $

; 打印字符串函数
print_string:
    pusha
    mov ah, 0x0E    ; BIOS teletype output
.loop:
    lodsb           ; 加载下一个字符
    test al, al     ; 检查是否为字符串结尾
    jz .done
    int 0x10        ; 打印字符
    jmp .loop
.done:
    popa
    ret

; 等待按键函数
wait_key:
    mov ah, 0x00
    int 0x16        ; BIOS 键盘中断
    ret

; GDT定义
gdt_start:
    ; 空描述符
    dd 0x0
    dd 0x0
    
    ; 代码段描述符 (0x08)
    dw 0xFFFF      ; 段限长
    dw 0x0         ; 段基址低16位
    db 0x0         ; 段基址中8位
    db 10011010b   ; 访问权限
    db 11001111b   ; 段限长高4位 + 标志
    db 0x0         ; 段基址高8位
    
    ; 数据段描述符 (0x10)
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10010010b   ; 访问权限
    db 11001111b   ; 段限长高4位 + 标志
    db 0x0         ; 段基址高8位

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; GDT大小
    dd gdt_start                ; GDT基地址

; 字符串常量
msg_loading db 'Loading BueOS...', 13, 10, 0
msg_disk_error db 'Disk read error!', 13, 10, 0
msg_memory_error db 'Memory check failed!', 13, 10, 0
msg_kernel_error db 'Invalid kernel image!', 13, 10, 0
msg_kernel_loaded db 'Kernel loaded successfully!', 13, 10, 0
msg_jumping db 'Jumping to kernel...', 13, 10, 0
    db 10010010b
    db 11001111b
    db 0x0
    
gdt_end:

; GDT描述符
gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; GDT大小
    dd gdt_start                ; GDT地址

memory_error:
    mov si, msg_memory_error
    call print_string
    jmp $

disk_error:
    mov si, msg_disk_error
    call print_string
    jmp $

kernel_error:
    mov si, msg_kernel_error
    call print_string
    jmp $

; 数据区
msg_loading db "BueOS Bootloader - Loading...", 0x0D, 0x0A, 0
msg_disk_error db "Disk read error!", 0x0D, 0x0A, 0
msg_memory_error db "1MB memory not available!", 0x0D, 0x0A, 0
msg_kernel_loaded db "Kernel loaded, verifying...", 0x0D, 0x0A, 0
msg_kernel_error db "Invalid kernel image!", 0x0D, 0x0A, 0
msg_jumping db "Jumping to kernel...", 0x0D, 0x0A, 0

    hlt         ; 停机

; 打印字符串
; 参数: SI=字符串地址
print_string:
    pusha
    mov ah, 0x0E    ; BIOS打印字符功能
.print_loop:
    lodsb           ; 加载下一个字符
    or al, al       ; 检查是否结束
    jz .print_done
    int 0x10        ; 调用BIOS
    jmp .print_loop
.print_done:
    popa
    ret

; 等待按键
wait_key:
    mov ah, 0
    int 0x16        ; BIOS键盘输入
    ret

times 510-($-$$) db 0   ; 填充剩余空间
dw 0xAA55               ; 引导扇区标志
