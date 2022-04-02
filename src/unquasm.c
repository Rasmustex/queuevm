#include <qvm/qvm.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

void print_help(FILE *f, const char *progname) {
    fprintf(f, "Usage: %s [OPTIONS] <filename.qvm>\t disassembles qvm bytecode file\n", progname);
    fprintf(f, "OPTIONS:\n");
    fprintf(f, "\t-h            print this help menu\n");
}

Qvm qvm = {0};

const char *inst_name[INST_COUNT] = {
    [INST_NOP]     = "nop",
    [INST_ENQUEUE] = "enq",
    [INST_DEQUEUE] = "deq",
    [INST_ADDI]    = "addi",
    [INST_SUBI]    = "subi",
    [INST_ADDU]    = "addu",
    [INST_SUBU]    = "subu",
    [INST_ADDF]    = "addf",
    [INST_SUBF]    = "subf",
    [INST_MULI]    = "muli",
    [INST_DIVI]    = "divi",
    [INST_MULU]    = "mulu",
    [INST_DIVU]    = "divu",
    [INST_MULF]    = "mulf",
    [INST_DIVF]    = "divf",
    [INST_DUP]     = "dup",
    [INST_SKIP]    = "skip",
    [INST_CHEAT]   = "cheat",
    [INST_LDA]     = "lda",
    [INST_STA]     = "sta",
    [INST_EQ]      = "eq",
    [INST_JUMP]    = "jmp",
    [INST_JZ]      = "jz",
    [INST_JNZ]     = "jnz",
    [INST_PUTI]    = "puti",
    [INST_PUTU]    = "putu",
    [INST_PUTF]    = "putf",
    [INST_PUTPTR]  = "putptr",
    [INST_HALT]    = "halt",
};

bool has_arg[INST_COUNT] = {
    [INST_NOP]     = false,
    [INST_ENQUEUE] = true,
    [INST_DEQUEUE] = false,
    [INST_ADDI]    = false,
    [INST_SUBI]    = false,
    [INST_ADDU]    = false,
    [INST_SUBU]    = false,
    [INST_ADDF]    = false,
    [INST_SUBF]    = false,
    [INST_MULI]    = false,
    [INST_DIVI]    = false,
    [INST_MULU]    = false,
    [INST_DIVU]    = false,
    [INST_MULF]    = false,
    [INST_DIVF]    = false,
    [INST_DUP]     = false,
    [INST_SKIP]    = false,
    [INST_CHEAT]   = false,
    [INST_LDA]     = false,
    [INST_STA]     = false,
    [INST_EQ]      = false,
    [INST_JUMP]    = true,
    [INST_JZ]      = true,
    [INST_JNZ]     = true,
    [INST_PUTI]    = false,
    [INST_PUTU]    = false,
    [INST_PUTF]    = false,
    [INST_PUTPTR]  = false,
    [INST_HALT]    = false,
};

int main(int argc, const char **argv) {
    if(argc < 2) {
        print_help(stderr, argv[0]);
        fprintf(stderr, "Error: no file argument provided\n");
        return 1;
    } else if(!strcmp(argv[1], "-h")) {
        print_help(stdout, argv[0]);
        return 0;
    }

    const char *fname = argv[1];
    qvm_load_program_from_file(&qvm, fname);
    for(uint64_t i = 0; i < qvm.program_size; ++i) {
        printf("%s", inst_name[qvm.program[i].inst]);
        if(has_arg[qvm.program[i].inst]) {
            printf(" %lu", qvm.program[i].arg.u64);
        }
        printf("\n");
    }
    return 0;
}
