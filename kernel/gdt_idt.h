/* BueOS GDT/IDT 头文件
 * GDT/IDT Header File
 * 定义全局描述符表(GDT)和中断描述符表(IDT)相关结构
 * 不使用标准库和编译器扩展
 */

#ifndef KERNEL_GDT_IDT_H
#define KERNEL_GDT_IDT_H

// 自定义基本数据类型
// Custom basic data types
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

// 寄存器保存结构
// Register Save Structure
struct regs {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} __attribute__((packed));

// GDT描述符结构
// GDT Descriptor Structure
struct gdt_entry {
    uint16_t limit_low;     // 段界限低16位
    uint16_t base_low;      // 段基址低16位
    uint8_t base_middle;    // 段基址中8位
    uint8_t access;         // 访问权限
    uint8_t granularity;   // 段界限高4位 + 标志位
    uint8_t base_high;      // 段基址高8位
}; // 注意: 需要手动确保结构体布局正确

// GDT指针结构
// GDT Pointer Structure
struct gdt_ptr {
    uint16_t limit;         // GDT大小-1
    uint32_t base;          // GDT基地址
}; // 注意: 需要手动确保结构体布局正确

// IDT描述符结构
// IDT Descriptor Structure
struct idt_entry {
    uint16_t base_low;      // 中断处理程序地址低16位
    uint16_t selector;      // 代码段选择子
    uint8_t zero;           // 保留
    uint8_t flags;          // 标志位
    uint16_t base_high;     // 中断处理程序地址高16位
}; // 注意: 需要手动确保结构体布局正确

// IDT指针结构
// IDT Pointer Structure
struct idt_ptr {
    uint16_t limit;         // IDT大小-1
    uint32_t base;          // IDT基地址
}; // 注意: 需要手动确保结构体布局正确

// 中断处理程序类型
// Interrupt Handler Type
typedef void (*interrupt_handler_t)(uint32_t int_no, uint32_t err_code);

// 中断控制器端口
// PIC Ports
#define PIC1_CMD  0x20
#define PIC1_DATA 0x21
#define PIC2_CMD  0xA0
#define PIC2_DATA 0xA1

// 中断相关函数声明
// Interrupt Function Declarations
void init_gdt();            // 初始化GDT
void init_idt();            // 初始化IDT
void load_gdt();            // 加载GDT
void load_idt();            // 加载IDT
void register_interrupt_handler(uint8_t n, interrupt_handler_t handler); // 注册中断处理程序
void irq_install_handler(int irq, void (*handler)(struct regs *r)); // 安装IRQ处理程序
void irq_uninstall_handler(int irq); // 卸载IRQ处理程序
void enable_interrupts();   // 启用中断
void disable_interrupts();  // 禁用中断

#endif // KERNEL_GDT_IDT_H
