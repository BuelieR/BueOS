/* BueOS Scheduler Header
 * BueOS调度器头文件
 * 定义调度器接口和数据结构
 */

#ifndef _SCHEDULER_H
#define _SCHEDULER_H


// 基本类型定义（避免标准库依赖）
typedef unsigned int uint32_t;

// 进程状态
// Process states
typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_TERMINATED
} task_state_t;

// 进程控制块(PCB)
// Process Control Block
typedef struct pcb {
    uint32_t esp;        // 栈指针
    uint32_t eip;        // 指令指针
    uint32_t pid;        // 进程ID
    task_state_t state;  // 进程状态
    uint32_t priority;   // 优先级
    struct pcb* next;    // 下一个PCB
} pcb_t;

// 初始化调度器
// Initialize scheduler
void init_scheduler(void);

// 调度循环
// Scheduler loop
void scheduler_loop(void);

// 创建新进程
// Create new process
pcb_t* create_process(void (*entry)(void), uint32_t priority);

// 切换进程
// Switch process
void switch_task(pcb_t* new_task);

#endif // _SCHEDULER_H
