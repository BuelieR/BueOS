; BueOS GDT/IDT 汇编实现
; GDT/IDT Assembly Implementation
; 实现GDT和IDT的加载功能
; 使用NASM汇编语法

bits 32
section .text

; 加载GDT
; Load GDT
; 参数: gdt_ptr指针(通过栈传递)
global load_gdt_asm
load_gdt_asm:
    mov eax, [esp + 4]  ; 获取gdt_ptr参数
    lgdt [eax]          ; 加载GDT

    ; 切换到保护模式
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; 远跳转刷新CS寄存器
    jmp 0x08:.flush_cs  ; 代码段选择子(在GDT中的索引为1)
.flush_cs:
    ; 设置数据段选择子
    mov ax, 0x10        ; 数据段选择子(在GDT中的索引为2)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; 验证保护模式
    mov edi, 0xB8000
    mov word [edi], 0x0A41 ; 显示'A'验证内存访问
    
    ; 显示保护模式已启用
    mov word [edi+2], 0x0A50 ; 'P'
    mov word [edi+4], 0x0A4D ; 'M'
    
    ; 验证控制寄存器
    mov eax, cr0
    and eax, 0x1
    cmp eax, 0x1
    je .pm_ok
    mov word [edi+6], 0x0A45 ; 'E' (Error)
    jmp $
.pm_ok:
    mov word [edi+6], 0x0A4F ; 'O' (OK)
    ret

; 加载IDT
; Load IDT
; 参数: idt_ptr指针(通过栈传递)
global load_idt_asm
load_idt_asm:
    mov eax, [esp + 4]  ; 获取idt_ptr参数
    lidt [eax]          ; 加载IDT
    ret

; 中断处理程序包装器
; Interrupt Handler Wrapper
extern interrupt_handlers
global interrupt_handler_wrapper
interrupt_handler_wrapper:
    pusha               ; 保存所有通用寄存器
    
    ; 保存段寄存器
    push ds
    push es
    push fs
    push gs
    
    ; 设置内核数据段
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; 获取中断号
    mov ebx, [esp+44]   ; 从栈中获取中断号
    
    ; 调用C中断处理程序
    push dword [esp+48] ; 错误代码
    push ebx            ; 中断号
    call [interrupt_handlers + ebx*4]
    add esp, 8          ; 清理栈
    
    ; 恢复段寄存器
    pop gs
    pop fs
    pop es
    pop ds
    
    popa               ; 恢复所有通用寄存器
    add esp, 8         ; 清理错误代码和中断号
    iret               ; 从中断返回

; 中断处理程序入口点数组
; Interrupt Handler Entry Points
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    cli
    push byte 0        ; 压入伪错误代码
    push byte %1       ; 压入中断号
    jmp isr_common
%endmacro

%macro ISR_ERRCODE 1
global isr%1
isr%1:
    cli
    push byte %1       ; 压入中断号
    jmp isr_common
%endmacro

; 定义中断处理程序入口点
ISR_NOERRCODE 0
ISR_NOERRCODE 1
; ... 其他中断处理程序

; 公共中断处理代码
isr_common:
    pusha              ; 保存所有通用寄存器
    
    ; 设置内核数据段
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; 调用C中断处理程序
    mov eax, esp
    push eax
    call interrupt_handler_wrapper
    pop eax
    
    popa               ; 恢复所有通用寄存器
    add esp, 8        ; 清理错误代码和中断号
    sti
    iret               ; 从中断返回
