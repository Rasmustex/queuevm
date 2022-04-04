#include "qvm.h"
#include "queue/queue.h"
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

const char *err_as_str(ERR err) {
    switch (err) {
    case ERR_OK:              return "ERR_OK";
    case ERR_QUEUE_UNDERFLOW: return "ERR_QUEUE_UNDERFLOW";
    case ERR_QUEUE_REALLOC:   return "ERR_QUEUE_REALLOC";
    case ERR_ILLEGAL_INST:    return "ERR_ILLEGAL_INST";
    case ERR_BAD_INST_PTR:    return "ERR_BAD_INST_PTR";
    default:                  return "Unreachable error";
    }
}

const char *inst_as_str(INST inst) {
    switch (inst) {
    case INST_NOP:            return "INST_NOP";
    case INST_ENQUEUE:        return "INST_ENQUEUE";
    case INST_DEQUEUE:        return "INST_DEQUEUE";
    case INST_ADDI:           return "INST_ADDI";
    case INST_SUBI:           return "INST_SUBI";
    case INST_ADDU:           return "INST_ADDU";
    case INST_SUBU:           return "INST_SUBU";
    case INST_ADDF:           return "INST_ADDF";
    case INST_SUBF:           return "INST_SUBF";
    case INST_MULI:           return "INST_MULI";
    case INST_DIVI:           return "INST_DIVI";
    case INST_MULU:           return "INST_MULU";
    case INST_DIVU:           return "INST_DIVU";
    case INST_MULF:           return "INST_MULF";
    case INST_DIVF:           return "INST_DIVF";
    case INST_DUP:            return "INST_DUP";
    case INST_SKIP:           return "INST_SKIP";
    case INST_CHEAT:          return "INST_CHEAT";
    case INST_LDA:            return "INST_LDA";
    case INST_STA:            return "INST_STA";
    case INST_EQ:             return "INST_EQ";
    case INST_JUMP:           return "INST_JUMP";
    case INST_JZ:             return "INST_JZ";
    case INST_JNZ:            return "INST_JNZ";
    case INST_PUTI:           return "INST_PUTI";
    case INST_PUTU:           return "INST_PUTU";
    case INST_PUTF:           return "INST_PUTF";
    case INST_PUTPTR:         return "INST_PUTPTR";
    case INST_HALT:           return "INST_HALT";
    case INST_COUNT: default: return "Unreachable instruction";
    }
}

void quasm_dump_program_to_file(Quasm *quasm, const char *fname) {
    FILE *f = fopen(fname, "wb");
    if(f == NULL) {
        fprintf(stderr, "Error: Failed to open file '%s'\n", fname);
        exit(1);
    }
    fwrite(quasm->program, sizeof(Inst), quasm->program_size, f);
    if(ferror(f)) {
        fprintf(stderr, "Error: Failed to write to file '%s': %s\n", fname, strerror(errno));
        exit(1);
    }
    fclose(f);
    return;
}

void qvm_load_program_from_file(Qvm *qvm, const char *fname) {
    FILE *f = fopen(fname, "rb");
    if(f == NULL) {
        fprintf(stderr, "Error: Failed to open file '%s'\n", fname);
        free(qvm->queue.data);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    if(ferror(f)) {
        fprintf(stderr, "Error: Failed to move cursor in file '%s': %s\n", fname, strerror(errno));
        fclose(f);
        free(qvm->queue.data);
        exit(1);
    }

    size_t file_size = ftell(f) / sizeof(Inst);
    if(ferror(f)) {
        fprintf(stderr, "Error: Failed to get cursor position in file '%s': %s\n", fname, strerror(errno));
        free(qvm->queue.data);
        fclose(f);
        exit(1);
    }
    if(file_size > PROGRAM_CAP) {
        fprintf(stderr, "Error: size of program in file '%s' is above max program capacity\n", fname);
        free(qvm->queue.data);
        fclose(f);
        exit(1);
    }

    fseek(f, 0, SEEK_SET);
    if(ferror(f)) {
        fprintf(stderr, "Error: Failed to move cursor in file '%s': %s\n", fname, strerror(errno));
        fclose(f);
        free(qvm->queue.data);
        exit(1);
    }

    fread(qvm->program, sizeof(Inst), file_size, f);
    qvm->program_size = file_size;
    if(ferror(f)) {
        fprintf(stderr, "Error: Failed to write to file '%s': %s\n", fname, strerror(errno));
        fclose(f);
        free(qvm->queue.data);
        exit(1);
    }
    fclose(f);
    return;
}
