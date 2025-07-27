/* BueOS Paging Implementation
 * BueOS 分页实现
 */

#include "paging.h"
#include "../gdt_idt.h"

// 位图数组用于跟踪物理页面使用情况
#define TOTAL_PAGES 1024 * 1024  // 4GB内存空间
static uint32_t page_bitmap[TOTAL_PAGES / 32];

// 当前页目录
static page_directory_t* current_directory = 0;

// 设置位图中的位
static void set_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr / 0x1000;
    uint32_t idx = frame / 32;
    uint32_t off = frame % 32;
    page_bitmap[idx] |= (1 << off);
}

// 清除位图中的位
static void clear_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr / 0x1000;
    uint32_t idx = frame / 32;
    uint32_t off = frame % 32;
    page_bitmap[idx] &= ~(1 << off);
}

// 测试位图中的位
static uint32_t test_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr / 0x1000;
    uint32_t idx = frame / 32;
    uint32_t off = frame % 32;
    return (page_bitmap[idx] & (1 << off));
}

// 查找第一个空闲帧
static uint32_t first_frame(void) {
    for (uint32_t i = 0; i < TOTAL_PAGES / 32; i++) {
        if (page_bitmap[i] != 0xFFFFFFFF) {
            for (uint32_t j = 0; j < 32; j++) {
                uint32_t toTest = 1 << j;
                if (!(page_bitmap[i] & toTest)) {
                    return i * 32 + j;
                }
            }
        }
    }
    return 0;
}

// 分配一个物理帧
void alloc_frame(page_table_entry_t* page, int is_kernel, int is_writeable) {
    if (page->frame != 0) {
        return;
    }
    
    uint32_t idx = first_frame();
    if (idx == 0) {
        // 没有空闲帧
        return;
    }
    
    set_frame(idx * 0x1000);
    page->present = 1;
    page->rw = (is_writeable) ? 1 : 0;
    page->user = (is_kernel) ? 0 : 1;
    page->frame = idx;
}

// 初始化分页
void init_paging(void) {
    // 清空位图
    for (uint32_t i = 0; i < TOTAL_PAGES / 32; i++) {
        page_bitmap[i] = 0;
    }
    
    // 创建页目录
    page_directory_t* dir = (page_directory_t*)kmalloc_page(sizeof(page_directory_t), 1);
    
    // 初始化页目录
    for (int i = 0; i < 1024; i++) {
        dir->tables[i].present = 0;
        dir->tables[i].rw = 1;
        dir->tables[i].user = 0;
        dir->tables[i].frame = 0;
    }
    
    // 标记内核空间
    uint32_t i = 0;
    while (i < 0x400000) {  // 前4MB
        alloc_frame(get_page(i, 1, dir), 1, 0);
        i += 0x1000;
    }
    
    // 注册页错误处理程序
    register_interrupt_handler(14, page_fault_handler);
    
    // 切换到新页目录
    switch_page_directory(dir);
}

// 页错误处理函数
void page_fault_handler(uint32_t int_no, uint32_t err_code) {
    // 获取CR2寄存器中的错误地址
    uint32_t faulting_address;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
    
    // 处理页错误...
    (void)int_no;  // 未使用参数
    (void)err_code; // 未使用参数
}

// 获取页表项
page_table_entry_t* get_page(uint32_t address, int make, page_directory_t* dir) {
    // 将地址转换为页索引
    address /= 0x1000;
    uint32_t table_idx = address / 1024;
    
    // 确保页目录有效
    if (dir->tables[table_idx].present == 0) {
        if (make) {
            uint32_t tmp = (uint32_t)kmalloc_page(sizeof(page_table_entry_t)*1024, 1);
            // 初始化新页表
            page_table_entry_t* new_table = (page_table_entry_t*)tmp;
            for (int i = 0; i < 1024; i++) {
                new_table[i].present = 0;
                new_table[i].rw = 1;
                new_table[i].user = 0;
                new_table[i].frame = 0;
            }
            dir->tables[table_idx].frame = tmp / 0x1000;
            dir->tables[table_idx].present = 1;
            dir->tables[table_idx].rw = 1;
            dir->tables[table_idx].user = 0;
        } else {
            return 0;
        }
    }
    
    // 返回页表项
    return &((page_table_entry_t*)(dir->tables[table_idx].frame * 0x1000))[address % 1024];
}

// 页分配函数
void* kmalloc_page(size_t size, int align) {
    // 简单实现 - 实际项目中需要更复杂的内存管理
    static uint32_t kmalloc_ptr = 0x100000; // 从1MB开始分配
    if (align == 1 && (kmalloc_ptr & 0xFFFFF000)) {
        kmalloc_ptr &= 0xFFFFF000;
        kmalloc_ptr += 0x1000;
    }
    uint32_t ret = kmalloc_ptr;
    kmalloc_ptr += size;
    return (void*)ret;
}

// 切换页目录
void switch_page_directory(page_directory_t* dir) {
    current_directory = dir;
    asm volatile("mov %0, %%cr3" :: "r"(dir->physicalAddr));
}
