#ifndef __NSVM_H_
#define __NSVM_H_


#include <stdint.h>

#define NSVM_VER "0.01"

/* ---------------- 配置区域开始 ---------------- */

#define NSVM_FUNC_MAX     0xFF
//#define NSVM_FUNC_MAX     0x1FF
//#define NSVM_FUNC_MAX     0x3FF
//#define NSVM_FUNC_MAX     0x7FF

/* 使用超过 256 种指令的情况下, 需开启长指令支持, 并关闭变长指令支持 */
/* 比如: 编号为 0x100 ~ 0x1FF 的指令省略目标操作数的时候会引发错误 */
//#define NSVM_VARLEN_OP
//#define NSVM_LONGLEN_OP

#define nsvm_ret        uint8_t
#define NSVM_RET_END    0xFF
#define NSVM_RET_RST    0xFE

#define NSVM_RET_OK     0x00
#define NSVM_RET_ERR    0x01

#define nsvm_addr       uint32_t

/* ---------------- 配置区域结束 ---------------- */


/* ------------------------------------------------- */
/* ---------------- 以下内容不应修改 ---------------- */
/* ------------------------------------------------- */

typedef struct {
    uint8_t     op_index;       // 1 byte, 指令编号低 8 位
    uint8_t     dst_type;       // 1 byte, 低 7 位为目标操作数类型
             // dst_type & 0x80    1 bit, 指令编号第 9 位, 目前作为保留位
    uint32_t    dst;            // 4 bytes, 立即数或虚拟地址
    uint8_t     src_type;       // 1 byte, 低 7 位为源操作数类型
             // src_type & 0x80    1 bit, 指令编号第 10 位, 目前作为保留位
    uint32_t    src;            // 4 bytes, 立即数或虚拟地址
    uint8_t     ext_type;       // 1 byte, 低 7 位为附加操作数类型
             // ext_type & 0x80    1 bit, 指令编号第 11 位, 目前作为保留位
    uint32_t    ext;            // 4 bytes, 立即数或虚拟地址
} NSVM_OP_L;                    // 16 bytes

typedef struct {
    uint8_t     op_index;       // 1 byte, 指令编号
    uint8_t     dst_type;       // 1 byte, 目标操作数类型
    uint16_t    dst;            // 2 bytes, 立即数或虚拟地址
    uint8_t     src_type;       // 1 byte, 源操作数类型
    uint16_t    src;            // 2 bytes, 立即数或虚拟地址
    uint8_t     reversed;       // 1 byte, 保留
} NSVM_OP_S;                    // 8 bytes

#ifdef NSVM_VARLEN_OP
    #define NSVM_OP_NARG                1
    #ifdef NSVM_LONGLEN_OP
        #define NSVM_OP_DST             6
        #define NSVM_OP_DST_SRC         11
        #define NSVM_OP_DST_SRC_EXT     16
    #else
        #define NSVM_OP_DST             4
        #define NSVM_OP_DST_SRC         7
    #endif
#endif

#ifdef NSVM_LONGLEN_OP
#define NSVM_OP NSVM_OP_L
#else
#define NSVM_OP NSVM_OP_S
#endif

typedef struct {
    nsvm_ret (*func)(NSVM_OP* op);
#ifdef NSVM_VARLEN_OP
    uint8_t length;
#endif
} NSVM_FUNC;

#ifndef NSVM_FUNC_MAX
#define NSVM_FUNC_MAX 0x80
#endif

#if NSVM_FUNC_MAX > 0xFF
#ifndef NSVM_LONGLEN_OP
#error NSVM_LONGLEN_OP should be defined.
#endif
#ifdef NSVM_VARLEN_OP
#error NSVM_VARLEN_OP should not be defined.
#endif
#endif

#if NSVM_FUNC_MAX <= 0xFF
#define nsvm_opi uint8_t
#else
#define nsvm_opi uint16_t
#endif

typedef struct {
    char msg[16];
    nsvm_addr addr;
    NSVM_OP op;
} NSVM_ERR_INFO;

NSVM_ERR_INFO* nsvm_err_info();

void nsvm_jmp(nsvm_addr addr);
nsvm_ret nsvm_exe(NSVM_OP* op);
nsvm_ret nsvm_run(uint8_t* code, nsvm_addr length);

#ifdef NSVM_VARLEN_OP
nsvm_ret nsvm_put(nsvm_opi index, uint8_t length, nsvm_ret (*func)(NSVM_OP* op));
#else
nsvm_ret nsvm_put(nsvm_opi index, nsvm_ret (*func)(NSVM_OP* op));
#endif


#endif
