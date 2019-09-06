# NSVM
NyaSama Virtual Machine

用于 NSASM 的字节码解释器
## Introduction (zh_CN)

#### 测试用指令集如下, 部分指令与 NSASM 有差异

```go
0x00    nop                                 空指令
0x01    end                                 终止解释器
0x02    rst                                 复位

0x03    jmp  addr                           直接跳转
0x04    jz   addr           rs == 0 ? jmp   零则跳转
0x05    jnz  addr           rs != 0 ? jmp   非零则跳转
0x06    jg   addr           rs >  0 ? jmp   大于则跳转
0x07    jl   addr           rs <  0 ? jmp   小于则跳转
0x08    cmp  r0, r1         rs = r0 - r1    比较

0x10    mov  r0, r1         r0 = r1         数据传送
0x11    push r0                             入栈
0x12    pop  r0                             出栈
0x13    prt  r0                             打印

0x20    add  r0, r1, r2     r0 = r1 + r2    加法
0x21    inc  r0             r0 += 1         加一
0x22    sub  r0, r1, r2     r0 = r1 - r2    减法
0x23    dec  r0             r0 -= 1         减一
0x24    mul  r0, r1, r2     r0 = r1 * r2    乘法
0x25    div  r0, r1, r2     r0 = r1 / r2    除法
0x26    mod  r0, r1, r2     r0 = r1 % r2    求余

0x30    and  r0, r1, r2     r0 = r1 & r2    与
0x31    or   r0, r1, r2     r0 = r1 | r2    或
0x32    xor  r0, r1, r2     r0 = r1 ^ r2    异或
0x33    not  r0             r0 = ~r0        非
0x34    shl  r0, r1         r0 = r0 << r1   左移
0x35    shr  r0, r1         r0 = r0 >> r1   右移
```

### 基本结构

```cpp
VM 基本上与 NSASM 解释器核心类似, 但是执行阶段为字节码解释执行
取指令及取数据期望能达到 O(1) 的级别
有预处理开关以确定指令是变长还是定长

以下为长指令结构
struct op {
    uint8_t     op_index;       // 1 byte, 指令编号低 8 位
    uint8_t     type_dst;       // 1 byte, 低 7 位为目标操作数类型
             // type_dst & 0x80    1 bit, 指令编号第 9 位, 目前作为保留位
    uint32_t    dst;            // 4 bytes, 立即数或虚拟地址
    uint8_t     type_src;       // 1 byte, 低 7 位为源操作数类型
             // type_src & 0x80    1 bit, 指令编号第 10 位, 目前作为保留位
    uint32_t    src;            // 4 bytes, 立即数或虚拟地址
    uint8_t     type_ext;       // 1 byte, 低 7 位为附加操作数类型
             // type_ext & 0x80    1 bit, 指令编号第 11 位, 目前作为保留位
    uint32_t    ext;            // 4 bytes, 立即数或虚拟地址
}                               // 16 bytes

以下为短指令结构
struct op {
    uint8_t     op_index;       // 1 byte, 指令编号
    uint8_t     type_dst;       // 1 byte, 目标操作数类型
    uint16_t    dst;            // 2 bytes, 立即数或虚拟地址
    uint8_t     type_src;       // 1 byte, 源操作数类型
    uint16_t    src;            // 2 bytes, 立即数或虚拟地址
    uint8_t     reversed;       // 1 byte, 保留
}                               // 8 bytes
```

### 其他信息

正在开发中, 还没有详细的文档

#### Copyright © NSDN 2014 - 2019
