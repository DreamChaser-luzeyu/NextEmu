/* 内存布局定义 */
MEMORY {
    MINICPU_MEM (rwx)  : ORIGIN = 0x180000000, LENGTH = 64M   /* MiniCPU Memory */
}

/* 段布局定义 */
SECTIONS {
    /* 代码段 (.text) 放在 MINICPU_MEM 开头 */
    .text : {
        *(.vectors)    /* 中断向量表优先 */
        *(.text*)      /* 所有代码段 */
        *(.rodata*)    /* 只读数据 */
    } > MINICPU_MEM

    /* 未初始化数据 (.bss) */
    .bss : {
        _sbss = .;      /* .bss 起始地址 */
        *(.bss*)
        *(COMMON)       /* 公共变量 */
        . = ALIGN(4);
        _ebss = .;      /* .bss 结束地址 */
    } > MINICPU_MEM

    /* 初始化数据 (.data) */
    .data : {
        _sdata = .;     /* RAM 中 .data 的起始地址 */
        *(.data*)       /* 初始化数据 */
        . = ALIGN(4);
        _edata = .;     /* RAM 中 .data 的结束地址 */
    } > MINICPU_MEM

    /* 自定义段 */
    .my_section : {
        __my_section_start = .;
        *(.my_section*) /* 收集所有 .my_section 段 */
        . = ALIGN(4);
        __my_section_end = .;
    } > MINICPU_MEM

    /* 堆栈区域 */
    .stack (NOLOAD) : {
        . = ALIGN(8);
        __stack_start = .;
        . += 0x10000;    /* 64KB 栈空间 */
        __stack_end = .; /* stack top */
    } > MINICPU_MEM

    /* 其他设置 */
    /DISCARD/ : {
        *(.comment)
        *(.note*)
    }
}
