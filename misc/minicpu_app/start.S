.section .text
.global _start

_start:
    lui     a0, 0x80000     # 加载上立即数 (Load Upper Immediate)
    addi    a1, zero, 1     # 设置寄存器 a1 = 1
    addi    a2, zero, 2     # 设置寄存器 a2 = 2
    add     a0, a1, a2      # 加法指令
    call    init            # 调用初始化C函数

main_loop:
    j       main_loop       # 跳转到标签 (死循环)

# 程序结束标记
.end
