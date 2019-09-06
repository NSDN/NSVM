#include "nsvm.h"

#include <stdio.h>
#include "memman.h"

nsvm_arg sreg = 0;
nsvm_arg regs[0xFF] = { 0 };
MemoryManager* mm = NULL;

#define __NSASM_GET_ARG(arg)            \
    nsvm_arg* arg = NULL;               \
    if (op->type_ ## arg == 0x01)   \
        arg = &(regs[op->arg & 0xFF]);  \
    else                                \
        arg = &(op->arg);

nsvm_ret nsasm_nop(NSVM_OP* op) {
    return NSVM_RET_OK;
}
nsvm_ret nsasm_end(NSVM_OP* op) {
    return NSVM_RET_END;
}
nsvm_ret nsasm_rst(NSVM_OP* op) {
    return NSVM_RET_RST;
}

nsvm_ret nsasm_jmp(NSVM_OP* op) {
    __NSASM_GET_ARG(dst);
    nsvm_jmp(*dst);
    return NSVM_RET_OK;
}
nsvm_ret nsasm_jz(NSVM_OP* op) {
    __NSASM_GET_ARG(dst);
    if (sreg == 0)
        nsvm_jmp(*dst);
    return NSVM_RET_OK;
}
nsvm_ret nsasm_jnz(NSVM_OP* op) {
    __NSASM_GET_ARG(dst);
    if (sreg != 0)
        nsvm_jmp(*dst);
    return NSVM_RET_OK;
}
nsvm_ret nsasm_jg(NSVM_OP* op) {
    __NSASM_GET_ARG(dst);
    if (sreg > 0)
        nsvm_jmp(*dst);
    return NSVM_RET_OK;
}
nsvm_ret nsasm_jl(NSVM_OP* op) {
    __NSASM_GET_ARG(dst);
    if (sreg < 0)
        nsvm_jmp(*dst);
    return NSVM_RET_OK;
}
nsvm_ret nsasm_cmp(NSVM_OP* op) {
    __NSASM_GET_ARG(dst);
    __NSASM_GET_ARG(src);
    sreg = *dst - *src;
    return NSVM_RET_OK;
}

nsvm_ret nsasm_mov(NSVM_OP* op) {
    __NSASM_GET_ARG(dst);
    __NSASM_GET_ARG(src);
    *dst = *src;
    return NSVM_RET_OK;
}
nsvm_ret nsasm_push(NSVM_OP* op) {
    __NSASM_GET_ARG(dst);
    return mm->push(mm->p, dst);
}
nsvm_ret nsasm_pop(NSVM_OP* op) {
    __NSASM_GET_ARG(dst);
    return mm->pop(mm->p, dst);
}
nsvm_ret nsasm_prt(NSVM_OP* op) {
    __NSASM_GET_ARG(dst);
    printf("PRT: 0x%x\n", *dst);
    return NSVM_RET_OK;
}

nsvm_ret nsasm_add(NSVM_OP* op) {
    __NSASM_GET_ARG(dst);
    __NSASM_GET_ARG(src);
    __NSASM_GET_ARG(ext);
    *dst = *src + *ext;
    return NSVM_RET_OK;
}
nsvm_ret nsasm_inc(NSVM_OP* op) {
    __NSASM_GET_ARG(dst);
    *dst += 1;
    return NSVM_RET_OK;
}
nsvm_ret nsasm_sub(NSVM_OP* op) {
    __NSASM_GET_ARG(dst);
    __NSASM_GET_ARG(src);
    __NSASM_GET_ARG(ext);
    *dst = *src - *ext;
    return NSVM_RET_OK;
}
nsvm_ret nsasm_dec(NSVM_OP* op) {
    __NSASM_GET_ARG(dst);
    *dst -= 1;
    return NSVM_RET_OK;
}
nsvm_ret nsasm_mul(NSVM_OP* op) {
    __NSASM_GET_ARG(dst);
    __NSASM_GET_ARG(src);
    __NSASM_GET_ARG(ext);
    *dst = *src * *ext;
    return NSVM_RET_OK;
}
nsvm_ret nsasm_div(NSVM_OP* op) {
    __NSASM_GET_ARG(dst);
    __NSASM_GET_ARG(src);
    __NSASM_GET_ARG(ext);
    *dst = *src / *ext;
    return NSVM_RET_OK;
}
nsvm_ret nsasm_mod(NSVM_OP* op) {
    __NSASM_GET_ARG(dst);
    __NSASM_GET_ARG(src);
    __NSASM_GET_ARG(ext);
    *dst = *src % *ext;
    return NSVM_RET_OK;
}

nsvm_ret nsasm_and(NSVM_OP* op) {
    __NSASM_GET_ARG(dst);
    __NSASM_GET_ARG(src);
    __NSASM_GET_ARG(ext);
    *dst = *src & *ext;
    return NSVM_RET_OK;
}
nsvm_ret nsasm_or(NSVM_OP* op) {
    __NSASM_GET_ARG(dst);
    __NSASM_GET_ARG(src);
    __NSASM_GET_ARG(ext);
    *dst = *src | *ext;
    return NSVM_RET_OK;
}
nsvm_ret nsasm_xor(NSVM_OP* op) {
    __NSASM_GET_ARG(dst);
    __NSASM_GET_ARG(src);
    __NSASM_GET_ARG(ext);
    *dst = *src ^ *ext;
    return NSVM_RET_OK;
}
nsvm_ret nsasm_not(NSVM_OP* op) {
    __NSASM_GET_ARG(dst);
    *dst = ~(*dst);
    return NSVM_RET_OK;
}
nsvm_ret nsasm_shl(NSVM_OP* op) {
    __NSASM_GET_ARG(dst);
    __NSASM_GET_ARG(src);
    *dst = *dst << *src;
    return NSVM_RET_OK;
}
nsvm_ret nsasm_shr(NSVM_OP* op) {
    __NSASM_GET_ARG(dst);
    __NSASM_GET_ARG(src);
    *dst = *dst >> *src;
    return NSVM_RET_OK;
}

void load_nsasm_to_nsvm() {
    if (mm != NULL)
        DisposeMemoryManager(mm);
    mm = InitMemoryManager(0xFF, 0xFF);

    nsvm_put(0x00, NSVM_OP_NARG, &nsasm_nop);
    nsvm_put(0x01, NSVM_OP_NARG, &nsasm_end);
    nsvm_put(0x02, NSVM_OP_NARG, &nsasm_rst);

    nsvm_put(0x03, NSVM_OP_DST, &nsasm_jmp);
    nsvm_put(0x04, NSVM_OP_DST, &nsasm_jz);
    nsvm_put(0x05, NSVM_OP_DST, &nsasm_jnz);
    nsvm_put(0x06, NSVM_OP_DST, &nsasm_jg);
    nsvm_put(0x07, NSVM_OP_DST, &nsasm_jl);
    nsvm_put(0x08, NSVM_OP_DST_SRC, &nsasm_cmp);

    nsvm_put(0x10, NSVM_OP_DST_SRC, &nsasm_mov);
    nsvm_put(0x11, NSVM_OP_DST, &nsasm_push);
    nsvm_put(0x12, NSVM_OP_DST, &nsasm_pop);
    nsvm_put(0x13, NSVM_OP_DST, &nsasm_prt);

    nsvm_put(0x20, NSVM_OP_DST_SRC_EXT, &nsasm_add);
    nsvm_put(0x21, NSVM_OP_DST, &nsasm_inc);
    nsvm_put(0x22, NSVM_OP_DST_SRC_EXT, &nsasm_sub);
    nsvm_put(0x23, NSVM_OP_DST, &nsasm_dec);
    nsvm_put(0x24, NSVM_OP_DST_SRC_EXT, &nsasm_mul);
    nsvm_put(0x25, NSVM_OP_DST_SRC_EXT, &nsasm_div);
    nsvm_put(0x26, NSVM_OP_DST_SRC_EXT, &nsasm_mod);

    nsvm_put(0x30, NSVM_OP_DST_SRC_EXT, &nsasm_and);
    nsvm_put(0x31, NSVM_OP_DST_SRC_EXT, &nsasm_or);
    nsvm_put(0x32, NSVM_OP_DST_SRC_EXT, &nsasm_xor);
    nsvm_put(0x33, NSVM_OP_DST, &nsasm_not);
    nsvm_put(0x34, NSVM_OP_DST_SRC, &nsasm_shl);
    nsvm_put(0x35, NSVM_OP_DST_SRC, &nsasm_shr);
}
