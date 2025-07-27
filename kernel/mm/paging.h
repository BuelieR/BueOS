/* BueOS Paging Management
 * BueOS 分页管理
 */

#ifndef _PAGING_H
#define _PAGING_H

// 基本类型定义
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long size_t;

// 页目录表项
typedef struct {
    uint32_t present    : 1;   // 页存在位
    uint32_t rw        : 1;   // 读写位
    uint32_t user      : 1;   // 用户/超级用户位
    uint32_t accessed  : 1;   // 访问位
    uint32_t dirty     : 1;   // 脏页位
    uint32_t unused    : 7;   // 未使用位
    uint32_t frame     : 20;  // 帧地址
} page_dir_entry_t;

// 页表项
typedef struct {
    uint32_t present    : 1;
    uint32_t rw        : 1;
    uint32_t user      : 1;
    uint32_t accessed  : 1;
    uint32_t dirty     : 1;
    uint32_t unused    : 7;
    uint32_t frame     : 20;
} page_table_entry_t;

// 页目录
typedef struct {
    page_dir_entry_t tables[1024];
    uint32_t physicalAddr;
} page_directory_t;

// 函数声明
void init_paging(void);
void switch_page_directory(page_directory_t* new_directory);
page_table_entry_t* get_page(uint32_t address, int make, page_directory_t* dir);
void page_fault_handler(void);
void* kmalloc_page(size_t size, int align);
void kfree_page(void* ptr);

#endif // _PAGING_H
