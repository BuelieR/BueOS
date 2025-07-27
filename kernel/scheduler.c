/* BueOS Scheduler Implementation
 * BueOS调度器实现
 * 实现基本的进程调度功能
 */

#include "scheduler.h"
#include <stddef.h>
#include <stdint.h>

// 简单的内存管理（临时实现）
#define HEAP_START 0x200000
#define HEAP_SIZE  0x100000
static uint32_t heap_ptr = HEAP_START;

// 当前运行进程
static pcb_t* current_task = NULL;

// 就绪队列
static pcb_t* ready_queue = NULL;

// 进程ID计数器
static uint32_t next_pid = 1;

// 简单的内存分配函数
static void* kmalloc(uint32_t size) {
    if (heap_ptr + size > HEAP_START + HEAP_SIZE) {
        return NULL;    // 内存不足
    }
    
    void* ptr = (void*)heap_ptr;
    heap_ptr += size;
    return ptr;
}

// 初始化调度器
void init_scheduler(void) {
    current_task = NULL;
    ready_queue = NULL;
    next_pid = 1;
}

// 创建新进程
pcb_t* create_process(void (*entry)(void), uint32_t priority) {
    // 分配PCB内存 (简化实现，实际需要内存管理)
    pcb_t* new_task = (pcb_t*)0x100000; // 临时地址
    
    // 初始化PCB
    new_task->pid = next_pid++;
    new_task->state = TASK_READY;
    new_task->priority = priority;
    new_task->eip = (uint32_t)entry;
    
    // 添加到就绪队列
    new_task->next = ready_queue;
    ready_queue = new_task;
    
    return new_task;
}

// 切换进程
void switch_task(pcb_t* new_task) {
    if (current_task != NULL) {
        // 保存当前任务状态
        __asm__ __volatile__("mov %%esp, %0" : "=r"(current_task->esp));
        current_task->state = TASK_READY;
    }
    
    // 切换到新任务
    current_task = new_task;
    current_task->state = TASK_RUNNING;
    
    // 恢复新任务状态
    __asm__ __volatile__("mov %0, %%esp" : : "r"(current_task->esp));
    __asm__ __volatile__("jmp *%0" : : "r"(current_task->eip));
}

// 简单的轮转调度算法
static pcb_t* schedule(void) {
    if (ready_queue == NULL) {
        return NULL;
    }
    
    // 从就绪队列头部取一个任务
    pcb_t* selected = ready_queue;
    ready_queue = ready_queue->next;
    
    return selected;
}

// 调度循环
void scheduler_loop(void) {
    while (1) {
        pcb_t* next_task = schedule();
        if (next_task != NULL) {
            switch_task(next_task);
        }
        
        // 如果没有任务可运行，则休眠
        __asm__ __volatile__("hlt");
    }
}
