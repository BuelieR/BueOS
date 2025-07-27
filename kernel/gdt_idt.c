/* BueOS GDT/IDT 实现文件
 * GDT/IDT Implementation File
 * 实现全局描述符表(GDT)和中断描述符表(IDT)的初始化和加载
 * 不使用标准库
 */

#include "gdt_idt.h"

// GDT表
// GDT Table
struct gdt_entry gdt[3];
struct gdt_ptr gp;

// IDT表
// IDT Table
struct idt_entry idt[256];
struct idt_ptr ip;

// I/O端口输出
// I/O Port Output
void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

// 设置GDT描述符
// Set GDT Descriptor
void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    // 设置段基址
    // Set segment base address
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    // 设置段界限
    // Set segment limit
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);

    // 设置访问权限和标志位
    // Set access flags and granularity
    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}

// 初始化GDT
// Initialize GDT
void init_gdt() {
    // 设置GDT指针
    // Set GDT pointer
    gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gp.base = (uint32_t)&gdt;

    // 设置空描述符
    // Set null descriptor
    gdt_set_gate(0, 0, 0, 0, 0);

    // 设置代码段描述符
    // Set code segment descriptor
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // 设置数据段描述符
    // Set data segment descriptor
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    // 加载GDT
    // Load GDT
    load_gdt();
}

// 加载GDT (汇编实现)
// Load GDT (Assembly implementation)
extern void load_gdt_asm(uint32_t);

void load_gdt() {
    load_gdt_asm((uint32_t)&gp);
}

// 设置IDT描述符
// Set IDT Descriptor
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = sel;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

// 声明中断处理包装器
extern void interrupt_handler_wrapper();

// 中断处理程序数组
// Interrupt Handlers Array
interrupt_handler_t interrupt_handlers[256];

// 默认中断处理程序
// Default Interrupt Handler
static void default_interrupt_handler(uint32_t int_no, uint32_t err_code) {
    // 简单的错误处理
    // Simple error handling
    for(;;);
}

// 初始化IDT
// Initialize IDT
void init_idt() {
    // 设置IDT指针
    // Set IDT pointer
    ip.limit = (sizeof(struct idt_entry) * 256) - 1;
    ip.base = (uint32_t)&idt;

    // 初始化所有中断门和中断处理程序
    // Initialize all interrupt gates and handlers
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, (uint32_t)interrupt_handler_wrapper, 0x08, 0x8E);
        interrupt_handlers[i] = default_interrupt_handler;
    }

    // 重新映射PIC
    // Remap PIC
    outb(PIC1_CMD, 0x11);  // 初始化主PIC
    outb(PIC2_CMD, 0x11);  // 初始化从PIC
    
    // 设置PIC中断向量偏移
    outb(PIC1_DATA, 0x20); // 主PIC起始中断号: 0x20 (32)
    outb(PIC2_DATA, 0x28); // 从PIC起始中断号: 0x28 (40)
    
    // 设置级联
    outb(PIC1_DATA, 0x04); // 告诉主PIC从PIC在IRQ2
    outb(PIC2_DATA, 0x02); // 告诉从PIC其级联位置
    
    // 设置8086模式
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);
    
    // 屏蔽所有中断（后面会按需启用）
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
    outb(PIC2_CMD, 0x11);
    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);
    outb(PIC1_DATA, 0x0);
    outb(PIC2_DATA, 0x0);

    // 加载IDT
    // Load IDT
    load_idt();
}

// 注册中断处理程序
// Register Interrupt Handler
void register_interrupt_handler(uint8_t n, interrupt_handler_t handler) {
    interrupt_handlers[n] = handler;
}

// 启用中断
// Enable Interrupts
void enable_interrupts() {
    asm volatile("sti");
}

// 禁用中断
// Disable Interrupts
void disable_interrupts() {
    asm volatile("cli");
}

// 加载IDT (汇编实现)
// Load IDT (Assembly implementation)
extern void load_idt_asm(uint32_t);

void load_idt() {
    load_idt_asm((uint32_t)&ip);
}
