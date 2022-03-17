#include "qvm.h"
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
    case INST_ADD:            return "INST_ADD";
    case INST_HALT:           return "INST_HALT";
    case INST_COUNT: default: return "Unreachable instruction";
    }
}

// TODO: better error handling

// TODO: Add enqueue error handling
ERR qvm_inst_exec(Qvm *qvm) {
    switch (qvm->program[qvm->ip].inst) {
    case INST_NOP:
        break;
    case INST_ENQUEUE:
        // TODO: change to allow enqueue from register
        if(enqueue(&qvm->queue, qvm->program[qvm->ip].arg) != 0)
            return ERR_QUEUE_REALLOC;
        break;
    case INST_DEQUEUE:
        // TODO: change to dequeue into register, and add drop inst
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
    } case INST_COUNT: case INST_HALT: default:
        return ERR_ILLEGAL_INST;
        break;
    }
    qvm->ip++;
    return (qvm->ip >= qvm->program_size) * ERR_BAD_INST_PTR;
}

void qvm_run(Qvm *qvm, bool debug, int64_t limit) {
    ERR err;
    if(limit != -1) {
        int i = 0;
        if(debug) {
            while(qvm->program[qvm->ip].inst != INST_HALT && i < limit) {
                printf("Inst: %s\n", inst_as_str(qvm->program[qvm->ip].inst));
                if((err = qvm_inst_exec(qvm))!= ERR_OK) {
                    fprintf(stderr, "Runtime error: %s\n", err_as_str(err));
                    free(qvm->queue.data);
                    exit(1);
                }
                print_queue(&qvm->queue);
                getc(stdin);
                ++i;
            }
        } else {
            while(qvm->program[qvm->ip].inst != INST_HALT && i < limit) {
                printf("Inst: %s\n", inst_as_str(qvm->program[qvm->ip].inst));
                if((err = qvm_inst_exec(qvm))!= ERR_OK) {
                    fprintf(stderr, "Runtime error: %s\n", err_as_str(err));
                    free(qvm->queue.data);
                    exit(1);
                }
                print_queue(&qvm->queue); // will be removed once print instructions introduced
                ++i;
            }
        }
    } else {
        if(debug) {
            while(qvm->program[qvm->ip].inst != INST_HALT) {
                printf("Inst: %s\n", inst_as_str(qvm->program[qvm->ip].inst));
                if((err = qvm_inst_exec(qvm))!= ERR_OK) {
                    fprintf(stderr, "Runtime error: %s\n", err_as_str(err));
                    free(qvm->queue.data);
                    exit(1);
                }
                print_queue(&qvm->queue);
                getc(stdin);
            }
        } else {
            while(qvm->program[qvm->ip].inst != INST_HALT) {
                printf("Inst: %s\n", inst_as_str(qvm->program[qvm->ip].inst));
                if((err = qvm_inst_exec(qvm))!= ERR_OK) {
                    fprintf(stderr, "Runtime error: %s\n", err_as_str(err));
                    free(qvm->queue.data);
                    exit(1);
                }
                print_queue(&qvm->queue); // will be removed once print instructions introduced
            }
        }
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
