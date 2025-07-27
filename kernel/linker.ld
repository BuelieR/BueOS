/* BueOS Kernel Linker Script
 * BueOS内核链接脚本
 * 定义内核的内存布局
 */

ENTRY(_start)  /* 入口点为_start函数 */

SECTIONS {
    /* 内核加载地址为1MB (传统x86内核加载地址) */
    . = 0x100000;
    
    /* 引导头必须位于最前面 */
    .boot_header : {
        *(.boot_header)
    }
    
    /* 内核代码段 */
    .text : {
        *(.start)   /* 启动代码 */
        *(.text)    /* 其他代码 */
    }
    
    /* 只读数据段 */
    .rodata : {
        *(.rodata*)
    }
    
    /* 数据段 */
    .data : {
        *(.data)
    }
    
    /* BSS段 (未初始化数据) */
    .bss : {
        *(COMMON)
        *(.bss)
    }
    
    /* 内核结束地址 */
    end = .;
}
