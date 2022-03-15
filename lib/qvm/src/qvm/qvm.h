#ifndef QVM_H
#define QVM_H

#include <queue/queue.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PROGRAM_CAP 1024
#define Word uint64_t

typedef enum {
    ERR_OK,
    ERR_QUEUE_UNDERFLOW,
    ERR_QUEUE_REALLOC,
    ERR_ILLEGAL_INST,
    ERR_BAD_INST_PTR,
} ERR;

typedef enum {
    INST_NOP,
    INST_ENQUEUE,
    INST_DEQUEUE,
    INST_ADD,
    INST_HALT,
    INST_COUNT,
} INST;

typedef struct {
    INST inst;
    Word arg;
} Inst;

// TODO: registers could be fun
typedef struct {
    Queue queue;
    Inst program[PROGRAM_CAP];
    size_t program_size;
    uint64_t ip;
} Qvm;

ERR inst_exec(Qvm *qvm);
void qvm_run(Qvm *qvm, bool debug, int limit);
void qvm_dump_program_to_file(Qvm *qvm, const char *fname);
void qvm_load_program_from_file(Qvm *qvm, const char *fname);

#endif
