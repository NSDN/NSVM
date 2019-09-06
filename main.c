#include <stdio.h>
#include <stdlib.h>

#include "nsvm.h"

extern void load_nsasm_to_nsvm();

uint32_t fsize(FILE* file) {
	uint32_t tmp = ftell(file);
	fseek(file, 0, SEEK_END);
	uint32_t size = ftell(file);
	fseek(file, tmp, SEEK_SET);
	return size;
}

void main(int argc, char* argv[]) {
    printf("NyaSama Virtual Machine\n");
    printf("Ver: %s\n\n", NSVM_VER);

    if (argc < 2) {
        printf("Usage: this.exe [binary file]\n\n");
        return;
    }

    printf("Loading instruction set...\n");
    load_nsasm_to_nsvm();

    printf("Loading code...\n");
    FILE* fp = fopen(argv[1], "rb");
    uint32_t size = fsize(fp);
    uint8_t* code = (uint8_t*) malloc(size);
    fread(code, 1, size, fp);
    fclose(fp);

    nsvm_ret result = nsvm_run(code, size);
    printf("\n");

    free(code);

    printf("Result is: 0x%x\n", result);
    printf("\n");
    if (result == NSVM_RET_ERR) {
        NSVM_ERR_INFO* info = nsvm_err_info();
        printf("Error: %s\n", info->msg);
        printf("  at addr: 0x%x\n", info->addr);
        printf("    op: 0x%x\n", info->op.op_index);
        printf("    dst type: 0x%x, dst: 0x%x\n", info->op.type_dst, info->op.dst);
        printf("    src type: 0x%x, src: 0x%x\n", info->op.type_src, info->op.src);
    #ifdef NSVM_LONGLEN_OP
        printf("    ext type: 0x%x, ext: 0x%x\n", info->op.type_ext, info->op.ext);
    #endif
        printf("\n");
    }
}
