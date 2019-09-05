# NSVM
NyaSama Virtual Machine

用于 NSASM 的字节码解释器
## Introduction (zh_CN)

#### 部分指令将不支持, 部分指令与 NSASM 有差异

```
mov     数据传送
push    入栈
pop     出栈
in      输入
out     输出
add     加法
inc     加一
sub     减法
dec     减一
mul     乘法
div     除法
cmp     比较器
jz      零则跳转
jnz     非零则跳转
jg      大于则跳转
jl      小于则跳转
and     与
or      或
xor     异或
not     非
shl     左移
shr     右移
end     程序结束
nop     空指令
rst     复位
```

### 基本结构

```cpp
VM 基本上与 NSASM 解释器核心类似, 但是执行阶段为字节码解释执行
取指令及取数据期望能达到 O(1) 的级别

以下为指令结构
struct op {
    uint8_t     op_index;       // 1 byte, 指令编号低 8 位
    uint8_t     dst_type;       // 1 byte, 低 7 位为目标操作数类型
             // dst_type & 0x80    1 bit, 指令编号第 9 位, 目前作为保留位
    uint8_t     src_type;       // 1 byte, 低 7 位为源操作数类型
             // src_type & 0x80    1 bit, 指令编号第 10 位, 目前作为保留位
    uint8_t     ext_type;       // 1 byte, 低 7 位为附加操作数类型
             // ext_type & 0x80    1 bit, 指令编号第 11 位, 目前作为保留位
    uint32_t    dst;            // 4 bytes, 立即数或虚拟地址
    uint32_t    src;            // 4 bytes, 立即数或虚拟地址
    uint32_t    ext;            // 4 bytes, 立即数或虚拟地址
}                               // 16 bytes

程序标号被编译后, 所在行被空指令代替, 以便计算偏移地址
```

### 其他信息

正在开发中, 还没有详细的文档

#### Copyright © NSDN 2014 - 2019
