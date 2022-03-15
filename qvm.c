#include "queue.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PROGRAM_CAP 1024
#define Word uint64_t

typedef enum {
    ERR_OK,
    ERR_QUEUE_UNDERFLOW,
    ERR_INVALID_INST,
} ERR;

const char *err_as_str(ERR err) {
    switch (err) {
    case ERR_QUEUE_UNDERFLOW:
        return "ERR_QUEUE_UNDERFLOW";
    case ERR_INVALID_INST:
        return "ERR_INVALID_INST";
    case ERR_OK:
        return "ERR_OK";
    default:
        return "Unreachable error";
    }
}

typedef enum {
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

typedef struct {
    Queue queue;
    Inst program[PROGRAM_CAP];
    uint64_t ip;
} Qvm;

Qvm qvm = {0};

ERR inst_exec(Qvm *qvm);
ERR qvm_run(Qvm *qvm);

// TODO: better error handling
int main(int argc, const char **argv) {
    queue_init(&qvm.queue);
    qvm.program[0] = (Inst){.inst = INST_ENQUEUE, .arg = 34};
    qvm.program[1] = (Inst){.inst = INST_ENQUEUE, .arg = 3};
    qvm.program[2] = (Inst){.inst = INST_ADD};
    qvm.program[3] = (Inst){.inst = INST_HALT};
    ERR err;
    if((err = qvm_run(&qvm)) != ERR_OK) {
        printf("Error: %s", err_as_str(err));
        free(qvm.queue.data);
        return 1;
    }
    free(qvm.queue.data);
    return 0;
}

// TODO: Add enqueue error handling
ERR inst_exec(Qvm *qvm) {
    switch (qvm->program[qvm->ip].inst) {
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
        return ERR_INVALID_INST;
        break;
    }
    qvm->ip++;
    return ERR_OK;
}

ERR qvm_run(Qvm *qvm) {
    ERR err;
    while(qvm->program[qvm->ip].inst != INST_HALT) {
        if((err = inst_exec(qvm))!= ERR_OK)
            return err;
        print_queue(&qvm->queue);
    }
    return ERR_OK;
}
