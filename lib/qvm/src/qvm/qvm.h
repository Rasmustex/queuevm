#ifndef QVM_H
#define QVM_H

#include <queue/queue.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PROGRAM_CAP 1024

typedef enum {
    ERR_OK,
    ERR_QUEUE_UNDERFLOW,
    ERR_QUEUE_REALLOC,
    ERR_ILLEGAL_INST,
    ERR_BAD_INST_PTR,
} ERR;

// TODO: zero flag
// Might want to rename dup
typedef enum {
    INST_NOP,
    INST_ENQUEUE,
    INST_DEQUEUE,
    INST_ADDI,
    INST_SUBI,
    INST_ADDU,
    INST_SUBU,
    INST_ADDF,
    INST_SUBF,
    INST_MULI,
    INST_DIVI,
    INST_MULU,
    INST_DIVU,
    INST_MULF,
    INST_DIVF,
    INST_DUP,
    INST_SKIP,
    INST_CHEAT,
    INST_LDA,
    INST_STA,
    INST_EQ,
    INST_JUMP,
    INST_JZ,
    INST_JNZ,
    INST_PUTI,  // the puts are temporary
    INST_PUTU,
    INST_PUTF,
    INST_PUTPTR,
    INST_HALT,
    INST_COUNT,
} INST;

typedef struct {
    INST inst;
    Word arg;
} Inst;

typedef struct {
    Queue queue;
    Inst program[PROGRAM_CAP];
    size_t program_size;
    uint64_t ip;
    Word a; // registers - may want x to be like x register in 6502
} Qvm;

void qvm_load_program_from_file(Qvm *qvm, const char *fname);
const char *inst_as_str(INST inst);
// TODO: separate library
typedef struct {
    Inst program[PROGRAM_CAP];
    size_t program_size;
} Quasm;

void quasm_dump_program_to_file(Quasm *quasm, const char *fname);
const char *err_as_str(ERR err);

#endif
