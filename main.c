#include <stdio.h>

#include "nsvm.h"

extern void load_nsasm_to_nsvm();

static uint8_t sample_code[] = {
//  nop
    0x00,
//  mov   reg   0                       num   0x32
    0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00,
//  prt   reg   0
    0x12, 0x01, 0x00, 0x00, 0x00, 0x00,
//  mov   reg   1                       num   0x16
    0x10, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00,
//  prt   reg   1
    0x12, 0x01, 0x01, 0x00, 0x00, 0x00,
//  mov   reg   2                       num   0x00
    0x10, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//  prt   reg   2
    0x12, 0x01, 0x02, 0x00, 0x00, 0x00,
//  add   reg   2                       reg   1                       reg   0
    0x11, 0x01, 0x02, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
//  prt   reg   2
    0x12, 0x01, 0x02, 0x00, 0x00, 0x00,
//  add   reg   2                       reg   2                       num   0x16
    0x11, 0x01, 0x02, 0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00,
//  prt   reg   2
    0x12, 0x01, 0x02, 0x00, 0x00, 0x00,
//  nop
    0x00,
//  end
    0x01,
//  prt   reg   2
    0x12, 0x01, 0x02, 0x00, 0x00, 0x00,
//  nop
    0x00
};

void main(int argc, char* argv[]) {
    printf("NyaSama Virtual Machine\n");
    printf("Ver: %s\n\n", NSVM_VER);

    printf("Loading instruction set...\n");
    load_nsasm_to_nsvm();

    printf("Sample code running...\n");
    nsvm_ret result = nsvm_run(sample_code, sizeof(sample_code));
    printf("\n");
    printf("Result is: 0x%x\n", result);
    printf("\n");
    NSVM_ERR_INFO* info = nsvm_err_info();
    printf("Error info: %s\n", info->msg);
    printf("      addr: 0x%x\n", info->addr);
    printf("      op: 0x%x\n", info->op.op_index);
    printf("      dst_type: 0x%x\n", info->op.dst_type);
    printf("      dst: 0x%x\n", info->op.dst);
    printf("      src_type: 0x%x\n", info->op.src_type);
    printf("      src: 0x%x\n", info->op.src);
    printf("      ext_type: 0x%x\n", info->op.ext_type);
    printf("      ext: 0x%x\n", info->op.ext);
    printf("\n");
}
