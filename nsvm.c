#include "nsvm.h"

#include <string.h>

static NSVM_FUNC nsvmFuncList[NSVM_FUNC_MAX] = { 0 };
static uint32_t nsvmProgCnt = 0;

#define NSVM_JMP_DUMMY  0xFFFFFFFF
static uint32_t nsvmJmpAddr = NSVM_JMP_DUMMY;

static NSVM_ERR_INFO nsvmErrInfo = { 0 };

NSVM_ERR_INFO* nsvm_err_info() {
    return &nsvmErrInfo;
}

#ifdef NSVM_VARLEN_OP
nsvm_ret nsvm_put(nsvm_opi index, uint8_t length, nsvm_ret (*func)(NSVM_OP* op)) {
    nsvmFuncList[index].func = *func;
    nsvmFuncList[index].length = length;
}
#else
nsvm_ret nsvm_put(nsvm_opi index, nsvm_ret (*func)(NSVM_OP* op)) {
    nsvmFuncList[index].func = func;
}
#endif

void nsvm_jmp(nsvm_addr addr) {
    nsvmJmpAddr = addr;
}

nsvm_ret nsvm_exe(NSVM_OP* op) {
#if NSVM_FUNC_MAX <= 0xFF
    return nsvmFuncList[op->op_index].func(op);
#elif defined(NSVM_LONGLEN_OP)
    #if NSVM_FUNC_MAX == 0x1FF
        return nsvmFuncList[op->op_index | ((op->type_dst & 0x80) << 1)].func(op);
    #elif NSVM_FUNC_MAX == 0x3FF
        return nsvmFuncList[op->op_index | ((op->type_dst & 0x80) << 1) | ((op->type_src & 0x80) << 2)].func(op);
    #elif NSVM_FUNC_MAX == 0x7FF
        return nsvmFuncList[op->op_index | ((op->type_dst & 0x80) << 1) | ((op->type_src & 0x80) << 2) | ((op->type_ext & 0x80) << 3)].func(op);
    #endif
#endif
}

uint8_t __nsvm_get_op_len(uint8_t* code) {
#ifdef NSVM_VARLEN_OP
    #if NSVM_FUNC_MAX <= 0xFF
        return nsvmFuncList[*code].length;
    #elif defined(NSVM_LONGLEN_OP)
        #if NSVM_FUNC_MAX == 0x1FF
            return nsvmFuncList[*code | ((*(code + 1) & 0x80) << 1)].length;
        #elif NSVM_FUNC_MAX == 0x3FF
            return nsvmFuncList[*code | ((*(code + 1) & 0x80) << 1) | ((*(code + 6) & 0x80) << 2)].length;
        #elif NSVM_FUNC_MAX == 0x7FF
            return nsvmFuncList[*code | ((*(code + 1) & 0x80) << 1) | ((*(code + 6) & 0x80) << 2) | ((*(code + 11) & 0x80) << 3)].length;
        #endif
    #endif
#else
    return sizeof(NSVM_OP);
#endif
}

#define ___NSVM_GET_U32(addr) ((uint32_t) (*(addr) | (*(addr + 1) << 8) | (*(addr + 2) << 16) | (*(addr + 3) << 24)))
#define ___NSVM_GET_U16(addr) ((uint16_t) (*(addr) | (*(addr + 1) << 8)))

void __nsvm_parse_op(uint8_t* code, uint8_t length, NSVM_OP* op) {
    memset(op, 0, sizeof(NSVM_OP));

    #ifdef NSVM_VARLEN_OP
        if (length >= NSVM_OP_NARG) {
            op->op_index = *code;
        #ifdef NSVM_LONGLEN_OP
            if (length >= NSVM_OP_DST) {
                op->type_dst = *(code + 1);
                op->dst= ___NSVM_GET_U32(code + 2);
                if (length >= NSVM_OP_DST_SRC) {
                    op->type_src = *(code + 6);
                    op->src= ___NSVM_GET_U32(code + 7);
                    if (length == NSVM_OP_DST_SRC_EXT) {
                        op->type_ext = *(code + 11);
                        op->ext= ___NSVM_GET_U32(code + 12);
                    }
                }
            }
        #else
            if (length >= NSVM_OP_DST) {
                op->type_dst = *(code + 1);
                op->dst= ___NSVM_GET_U16(code + 2);
                if (length == NSVM_OP_DST_SRC) {
                    op->type_src = *(code + 4);
                    op->src= ___NSVM_GET_U16(code + 5);
                }
            }
        #endif
        }
    #else
        if (length == sizeof(NSVM_OP)) {
            op->op_index = *code;
        #ifdef NSVM_LONGLEN_OP
            op->type_dst = *(code + 1);
            op->dst= *(code + 2) | (*(code + 3) << 8) | (*(code + 4) << 16) | (*(code + 5) << 24);
            op->type_src = *(code + 6);
            op->src= *(code + 7) | (*(code + 8) << 8) | (*(code + 9) << 16) | (*(code + 10) << 24);
            op->type_ext = *(code + 11);
            op->ext= *(code + 12) | (*(code + 13) << 8) | (*(code + 14) << 16) | (*(code + 15) << 24);
        #else
            op->type_dst = *(code + 1);
            op->dst= *(code + 2) | (*(code + 3) << 8);
            op->type_src = *(code + 4);
            op->src= *(code + 5) | (*(code + 6) << 8);
            op->reversed = 0xFF;
        #endif
        }
    #endif
}

nsvm_ret nsvm_run(uint8_t* code, nsvm_addr length) {
    uint8_t op_length = 0; uint8_t* offset = code;
    NSVM_OP op_body = { 0 };
    nsvm_ret result = NSVM_RET_OK;

    memset(&nsvmErrInfo, 0, sizeof(NSVM_ERR_INFO));

    while (nsvmProgCnt < length) {
        offset = code + nsvmProgCnt;

        op_length = __nsvm_get_op_len(offset);
        if (nsvmProgCnt + op_length > length) {
            strcpy(nsvmErrInfo.msg, "Invaild Len");
            nsvmErrInfo.addr = nsvmProgCnt;
            return NSVM_RET_ERR;
        }

        __nsvm_parse_op(offset, op_length, &op_body);

        result = nsvm_exe(&op_body);

        if (result == NSVM_RET_ERR) {
            strcpy(nsvmErrInfo.msg, "Std Error");
            nsvmErrInfo.addr = nsvmProgCnt;
            memcpy(&(nsvmErrInfo.op), &op_body, sizeof(NSVM_OP));
            return NSVM_RET_ERR;
        }

        if (nsvmProgCnt >= length) {
            strcpy(nsvmErrInfo.msg, "Invaild Addr");
            nsvmErrInfo.addr = nsvmProgCnt;
            return NSVM_RET_ERR;
        }

        if (nsvmJmpAddr != NSVM_JMP_DUMMY) {
            if (nsvmJmpAddr >= length) {
                strcpy(nsvmErrInfo.msg, "Invaild Jump");
                nsvmErrInfo.addr = nsvmProgCnt;
                memcpy(&(nsvmErrInfo.op), &op_body, sizeof(NSVM_OP));
                return NSVM_RET_ERR;
            }
            nsvmProgCnt = nsvmJmpAddr;
            nsvmJmpAddr = NSVM_JMP_DUMMY;
        } else {
            if (result == NSVM_RET_END)
                break;
            else if (result == NSVM_RET_RST)
                nsvmProgCnt = 0;
            else
                nsvmProgCnt += op_length;
        }
    }

    return NSVM_RET_OK;
}
