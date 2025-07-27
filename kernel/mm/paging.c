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

// 切换页目录
void switch_page_directory(page_directory_t* dir) {
    current_directory = dir;
    asm volatile("mov %0, %%cr3" :: "r"(dir->physicalAddr));
}
