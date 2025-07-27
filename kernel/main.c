/* BueOS Kernel Main Entry
 * BueOS内核主入口
 * 使用Clang编译，禁用标准库
 * 编译命令: clang -target i386-pc-none-elf -nostdlib -ffreestanding -c main.c -o main.o
 */

#include "gdt_idt.h"
#include "scheduler.h"

// 测试中断处理程序
// Test Interrupt Handler
static void test_interrupt_handler(uint32_t int_no, uint32_t err_code) {
    unsigned short* video_memory = (unsigned short*)0xB8000;
    const char* msg = "Interrupt Received!";
    for (int i = 0; msg[i] != '\0'; i++) {
        video_memory[80 + i] = (0x0A << 8) | msg[i];  // 绿色文字
    }
}

// 打印启动信息
// Print boot information
static void print_boot_info() {
    volatile unsigned short* video_memory = (unsigned short*)0xB8000;
    
    const char* boot_tasks[] = {
        "Initializing GDT/IDT",
        "Setting up interrupts",
        "Initializing scheduler",
        "Starting system services"
    };
    
    // 在屏幕第3行开始显示启动任务
    unsigned int task_count = 4; // 手动计算数组大小
    for (unsigned int i = 0; i < task_count; i++) {
        const char* task = boot_tasks[i];
        unsigned int j = 0;
        while (task[j] != '\0') {
            video_memory[160 + i*80 + j] = (0x07 << 8) | task[j];
            j++;
        }
    }
    
    // 确保显示更新
    asm volatile ("" : : "r"(video_memory) : "memory");
}

// 检查保护模式标志
static int check_protected_mode() {
    unsigned int cr0;
    asm volatile ("mov %%cr0, %0" : "=r"(cr0));
    return (cr0 & 0x1); // PE位
}

// Kernel entry point
// 内核入口点
void kernel_main(void) {
    // 检查处理器模式
    if (!check_protected_mode()) {
        // 紧急错误处理 - 显示红色错误信息
        volatile unsigned short* vmem = (volatile unsigned short*)0xB8000;
        const char* err = "ERROR: Not in protected mode!";
        for (int i = 0; err[i]; i++) {
            vmem[i] = (0x04 << 8) | err[i]; // 红色文字
        }
        while(1); // 挂起
    }

    // 设置显示模式 (使用volatile确保内存访问)
    volatile unsigned short* video_memory = (volatile unsigned short*)0xB8000;
    
    // 清屏并显示早期调试信息
    for (int i = 0; i < 80 * 25; i++) {
        video_memory[i] = (0x07 << 8) | ' ';
    }
    
    // 显示内核已启动
    const char* early_msg = "Kernel entered!";
    for (int i = 0; early_msg[i] != '\0'; i++) {
        video_memory[i] = (0x0A << 8) | early_msg[i];  // 绿色文字
    }
    
    const char* welcome = "BueOS Kernel - Initializing...";
    for (int i = 0; welcome[i] != '\0'; i++) {
        video_memory[i] = (0x07 << 8) | welcome[i];
    }

    // 打印启动信息
    print_boot_info();

    // 初始化GDT和IDT
    init_gdt();
    init_idt();

    // 初始化调度器
    init_scheduler();

    // 注册测试中断处理程序
    register_interrupt_handler(0x20, test_interrupt_handler);

    // 启用中断
    enable_interrupts();

    // 进入调度循环
    scheduler_loop();
}

// 内核启动代码 (由bootloader调用)
// Kernel startup code (called by bootloader)
__attribute__((section(".boot_header"))) 
const uint32_t boot_header[] = {
    0x1BADB002, // 魔数
    0,          // 标志
    0,          // 校验和
};

__attribute__((section(".start"))) 
void _start() {
    kernel_main();
}
