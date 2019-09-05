#include "nsvm.h"

#include <stdio.h>

uint32_t regs[0xFF] = { 0 };

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
    uint32_t* dst = NULL;
    if (op->dst_type == 0x01)
        dst = &(regs[op->dst & 0xFF]);
    else
        dst = &(op->dst);
    nsvm_jmp(*dst);
    return NSVM_RET_OK;
}

nsvm_ret nsasm_mov(NSVM_OP* op) {
    uint32_t* dst = NULL;
    if (op->dst_type == 0x01)
        dst = &(regs[op->dst & 0xFF]);
    else
        dst = &(op->dst);
    uint32_t* src = NULL;
    if (op->src_type == 0x01)
        src = &(regs[op->src & 0xFF]);
    else
        src = &(op->src);
    *dst = *src;
    
    printf("%x, %x; %x, %x; %x, %x\n", op->dst_type, op->dst, op->src_type, op->src, *dst, *src);

    return NSVM_RET_OK;
}

nsvm_ret nsasm_add(NSVM_OP* op) {
    uint32_t* dst = NULL;
    if (op->dst_type == 0x01)
        dst = &(regs[op->dst & 0xFF]);
    else
        dst = &(op->dst);
    uint32_t* src = NULL;
    if (op->src_type == 0x01)
        src = &(regs[op->src & 0xFF]);
    else
        src = &(op->src);
    uint32_t* ext = NULL;
    if (op->ext_type == 0x01)
        ext = &(regs[op->ext & 0xFF]);
    else
        ext = &(op->ext);
    *dst = *src + *ext;
    return NSVM_RET_OK;
}

nsvm_ret nsasm_prt(NSVM_OP* op) {
    uint32_t* dst = NULL;
    if (op->dst_type == 0x01)
        dst = &(regs[op->dst & 0xFF]);
    else
        dst = &(op->dst);
    printf("Test print: 0x%x\n", *dst);
    return NSVM_RET_OK;
}

void load_nsasm_to_nsvm() {
    nsvm_put(0x00, NSVM_OP_NARG, &nsasm_nop);
    nsvm_put(0x01, NSVM_OP_NARG, &nsasm_end);
    nsvm_put(0x02, NSVM_OP_NARG, &nsasm_rst);
    nsvm_put(0x03, NSVM_OP_DST, &nsasm_jmp);
    nsvm_put(0x10, NSVM_OP_DST_SRC, &nsasm_mov);
    nsvm_put(0x11, NSVM_OP_DST_SRC_EXT, &nsasm_add);
    nsvm_put(0x12, NSVM_OP_DST, &nsasm_prt);
}
