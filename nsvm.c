#include "nsvm.h"

#include <string.h>

static NSVM_FUNC nsvmFuncList[NSVM_FUNC_MAX] = { 0 };
static uint32_t nsvmProgCnt = 0;

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
    nsvmProgCnt = addr;
}

nsvm_ret nsvm_exe(NSVM_OP* op) {
#if NSVM_FUNC_MAX <= 0xFF
    return nsvmFuncList[op->op_index].func(op);
#elif defined(NSVM_LONGLEN_OP)
    #if NSVM_FUNC_MAX == 0x1FF
        return nsvmFuncList[op->op_index | ((op->dst_type & 0x80) << 1)].func(op);
    #elif NSVM_FUNC_MAX == 0x3FF
        return nsvmFuncList[op->op_index | ((op->dst_type & 0x80) << 1) | ((op->src_type & 0x80) << 2)].func(op);
    #elif NSVM_FUNC_MAX == 0x7FF
        return nsvmFuncList[op->op_index | ((op->dst_type & 0x80) << 1) | ((op->src_type & 0x80) << 2) | ((op->ext_type & 0x80) << 3)].func(op);
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

        memset(&op_body, 0, sizeof(NSVM_OP));
        memcpy(&op_body, offset, op_length);  // may cause bug

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

        if (result == NSVM_RET_END)
            return NSVM_RET_OK;
        else if (result == NSVM_RET_RST)
            nsvmProgCnt = 0;
        else {
            nsvmProgCnt += op_length;
        }
    }

    return NSVM_RET_OK;
}
