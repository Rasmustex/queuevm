#include "queue.h"
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PROGRAM_CAP 1024
#define Word uint64_t

typedef enum {
    ERR_OK,
    ERR_QUEUE_UNDERFLOW,
    ERR_ILLEGAL_INST,
    ERR_BAD_INST_PTR,
} ERR;

const char *err_as_str(ERR err) {
    switch (err) {
    case ERR_OK:              return "ERR_OK";
    case ERR_QUEUE_UNDERFLOW: return "ERR_QUEUE_UNDERFLOW";
    case ERR_ILLEGAL_INST:    return "ERR_ILLEGAL_INST";
    case ERR_BAD_INST_PTR:    return "ERR_BAD_INST_PTR";
    default:                  return "Unreachable error";
    }
}

typedef enum {
    INST_NOP,
    INST_ENQUEUE,
    INST_DEQUEUE,
    INST_ADD,
    INST_HALT,
    INST_COUNT,
} INST;

const char *inst_as_str(INST inst) {
    switch (inst) {
    case INST_NOP:            return "INST_NOP";
    case INST_ENQUEUE:        return "INST_ENQUEUE";
    case INST_DEQUEUE:        return "INST_DEQUEUE";
    case INST_ADD:            return "INST_ADD";
    case INST_HALT:           return "INST_HALT";
    case INST_COUNT: default: return "Unreachable instruction";
    }
}

typedef struct {
    INST inst;
    Word arg;
} Inst;

typedef struct {
    Queue queue;
    Inst program[PROGRAM_CAP];
    size_t program_size;
    uint64_t ip;
} Qvm;

Qvm qvm = {0};

ERR inst_exec(Qvm *qvm);
void qvm_run(Qvm *qvm, bool debug);
void qvm_dump_program_to_file(Qvm *qvm, const char *fname);
void qvm_load_program_from_file(Qvm *qvm, const char *fname);

// TODO: better error handling
int main(int argc, const char **argv) {
    queue_init(&qvm.queue);
    /* qvm.program[0] = (Inst){.inst = INST_ENQUEUE, .arg = 34}; */
    /* qvm.program[1] = (Inst){.inst = INST_ENQUEUE, .arg = 3}; */
    /* qvm.program[2] = (Inst){.inst = INST_ADD}; */
    /* qvm.program[3] = (Inst){.inst = INST_HALT}; */
    /* qvm.program_size = 4; */
    qvm_load_program_from_file(&qvm, "test.qvm");
    qvm_run(&qvm, true);
    /* qvm_dump_program_to_file(&qvm, "test.qvm"); */
    free(qvm.queue.data);
    return 0;
}

// TODO: Add enqueue error handling
ERR inst_exec(Qvm *qvm) {
    switch (qvm->program[qvm->ip].inst) {
    case INST_NOP:
        break;
    case INST_ENQUEUE:
        enqueue(&qvm->queue, qvm->program[qvm->ip].arg);
        break;
    case INST_DEQUEUE:
        if(queue_empty(&qvm->queue))
            return ERR_QUEUE_UNDERFLOW;
        else
            dequeue(&qvm->queue);
        break;
    case INST_ADD: {
        if(qvm->queue.size < 2) {
            return ERR_QUEUE_UNDERFLOW;
        }
        Word a = dequeue(&qvm->queue);
        enqueue(&qvm->queue, a + dequeue(&qvm->queue));
        break;
    } case INST_COUNT:
    default:
        return ERR_ILLEGAL_INST;
        break;
    }
    qvm->ip++;
    return (qvm->ip >= qvm->program_size) * ERR_BAD_INST_PTR;
}

void qvm_run(Qvm *qvm, bool debug) {
    ERR err;
    if(debug) {
        while(qvm->program[qvm->ip].inst != INST_HALT) {
            if((err = inst_exec(qvm))!= ERR_OK) {
                fprintf(stderr, "Runtime error: %s\n", err_as_str(err));
                free(qvm->queue.data);
                exit(1);
            }
            print_queue(&qvm->queue);
            getc(stdin);
        }
    } else {
        while(qvm->program[qvm->ip].inst != INST_HALT) {
            if((err = inst_exec(qvm))!= ERR_OK) {
                fprintf(stderr, "Runtime error: %s\n", err_as_str(err));
                free(qvm->queue.data);
                exit(1);
            }
            print_queue(&qvm->queue); // will be removed once print instructions introduced
        }
    }
}

void qvm_dump_program_to_file(Qvm *qvm, const char *fname) {
    FILE *f = fopen(fname, "wb");
    if(f == NULL) {
        fprintf(stderr, "Error: Failed to open file '%s'\n", fname);
        free(qvm->queue.data);
        exit(1);
    }
    fwrite(qvm->program, sizeof(Inst), qvm->program_size, f);
    if(ferror(f)) {
        fprintf(stderr, "Error: Failed to write to file '%s': %s\n", fname, strerror(errno));
        free(qvm->queue.data);
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
