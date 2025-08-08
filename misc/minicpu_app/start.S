.section .text
.global _start

_start:
    # lui     a0, 0x80000     # 加载上立即数 (Load Upper Immediate)
    # addi    a1, zero, 1     # 设置寄存器 a1 = 1
    # addi    a2, zero, 2     # 设置寄存器 a2 = 2
    # add     a0, a1, a2      # 加法指令

    li      a0, 0x66ccf8    # a0: curr ctx reg addr
    ld      a1, 0(a0)       # a1: curr ctx index a1 = *(uint64_t*)(0x66ccf8)
    la      a2, __stack_end # a2: end addr of stack region
    slli    a3, a1, 13      # a3: stack offset of curr ctx
    sub     sp, a2, a3      # sp: stack pointer sp = a2 - a3

    # la      sp, __stack_end # Set stack base addr (From high addr to low addr)
    call    init            # 调用初始化C函数

main_loop:
    j       main_loop       # 跳转到标签 (死循环)

# 程序结束标记
.end
