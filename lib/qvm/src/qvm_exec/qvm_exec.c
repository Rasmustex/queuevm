#include <queue/queue.h>
#include <qvm/qvm.h>
#include <assert.h>
#include <qvm_exec/qvm_exec.h>

static ERR error;
// todo: file-global static err to make exec functions inlineable
inline static void exec_nop() {
    error = ERR_OK;
}

inline static void exec_enqueue(Qvm *qvm) {
    if(enqueue(&qvm->queue, qvm->program[qvm->ip].arg) != 0)
        error = ERR_QUEUE_REALLOC;
    else
        error = ERR_OK;
}

inline static void exec_dequeue(Qvm *qvm) {
    if(queue_empty(&qvm->queue)) {
        error = ERR_QUEUE_UNDERFLOW;
    } else {
        dequeue(&qvm->queue);
        error = ERR_OK;
    }
}

inline static void exec_addi(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        error = ERR_QUEUE_UNDERFLOW;
        return;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.i64 = a.i64 + dequeue(&qvm->queue).i64}) != 0)
        error = ERR_QUEUE_REALLOC;
    else
        error = ERR_OK;
}

inline static void exec_subi(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        error = ERR_QUEUE_UNDERFLOW;
        return;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.i64 = dequeue(&qvm->queue).i64 - a.i64}) != 0)
        error = ERR_QUEUE_REALLOC;
    else
        error = ERR_OK;
}

inline static void exec_addu(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        error = ERR_QUEUE_UNDERFLOW;
        return;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.u64 = a.u64 + dequeue(&qvm->queue).u64}) != 0)
        error = ERR_QUEUE_REALLOC;
    else
        error = ERR_OK;
}

inline static void exec_subu(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        error = ERR_QUEUE_UNDERFLOW;
        return;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.u64 = dequeue(&qvm->queue).u64 - a.u64}) != 0)
        error = ERR_QUEUE_REALLOC;
    else
        error = ERR_OK;
}

inline static void exec_addf(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        error = ERR_QUEUE_UNDERFLOW;
        return;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.f64 = a.f64 + dequeue(&qvm->queue).f64}) != 0)
        error = ERR_QUEUE_REALLOC;
    else
        error = ERR_OK;
}

inline static void exec_subf(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        error = ERR_QUEUE_UNDERFLOW;
        return;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.f64 = dequeue(&qvm->queue).f64 - a.f64}) != 0)
        error = ERR_QUEUE_REALLOC;
    else
        error = ERR_OK;
}

inline static void exec_muli(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        error = ERR_QUEUE_UNDERFLOW;
        return;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.i64 = dequeue(&qvm->queue).i64 * a.i64}) != 0)
        error = ERR_QUEUE_REALLOC;
    else
        error = ERR_OK;
}

inline static void exec_divi(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        error = ERR_QUEUE_UNDERFLOW;
        return;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.i64 = dequeue(&qvm->queue).i64 / a.i64}) != 0)
        error = ERR_QUEUE_REALLOC;
    else
        error = ERR_OK;
}

inline static void exec_mulu(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        error = ERR_QUEUE_UNDERFLOW;
        return;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.u64 = dequeue(&qvm->queue).u64 * a.u64}) != 0)
        error = ERR_QUEUE_REALLOC;
    else
        error = ERR_OK;
}

inline static void exec_divu(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        error = ERR_QUEUE_UNDERFLOW;
        return;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.u64 = dequeue(&qvm->queue).u64 / a.u64}) != 0)
        error = ERR_QUEUE_REALLOC;
    else
        error = ERR_OK;
}

inline static void exec_mulf(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        error = ERR_QUEUE_UNDERFLOW;
        return;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.f64 = dequeue(&qvm->queue).f64 * a.f64}) != 0)
        error = ERR_QUEUE_REALLOC;
    else
        error = ERR_OK;
}

inline static void exec_divf(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        error = ERR_QUEUE_UNDERFLOW;
        return;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.f64 = dequeue(&qvm->queue).f64 / a.f64}) != 0)
        error = ERR_QUEUE_REALLOC;
    else
        error = ERR_OK;
}

inline static void exec_dup(Qvm *qvm) {
    if(queue_empty(&qvm->queue))
        error = ERR_QUEUE_UNDERFLOW;
    else if(enqueue(&qvm->queue, queue_front(&qvm->queue)) != 0)
        error = ERR_QUEUE_REALLOC;
    else
        error = ERR_OK;
}

inline static void exec_skip(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        error = ERR_QUEUE_UNDERFLOW;
        return;
    }

    queue_skip(&qvm->queue);
    error = ERR_OK;
}

inline static void exec_cheat(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        error = ERR_QUEUE_UNDERFLOW;
        return;
    }

    queue_cheat(&qvm->queue);
    error = ERR_OK;
}

inline static void exec_lda(Qvm *qvm) {
    if(queue_empty(&qvm->queue)) {
        error = ERR_QUEUE_UNDERFLOW;
        return;
    }
    qvm->a = queue_front(&qvm->queue);
    error = ERR_OK;
}

inline static void exec_sta(Qvm *qvm) {
    if(enqueue(&qvm->queue, qvm->a) != 0)
        error = ERR_QUEUE_REALLOC;
    else
        error = ERR_OK;
}

inline static void exec_eq(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        error = ERR_QUEUE_UNDERFLOW;
        return;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.i64 = a.i64 == dequeue(&qvm->queue).i64}) != 0)
        error = ERR_QUEUE_REALLOC;
    else
        error = ERR_OK;
}

inline static void exec_jump(Qvm *qvm) {
    if(qvm->program[qvm->ip].arg.u64 > qvm->program_size) {
        error = ERR_BAD_INST_PTR;
    } else {
        qvm->ip = qvm->program[qvm->ip].arg.u64 - 1; // minus one to account for increment at end of function
        error = ERR_OK;
    }
}

inline static void exec_jz(Qvm *qvm) {
    if(qvm->queue.element_count < 1) {
        error = ERR_QUEUE_UNDERFLOW;
        return;
    }

    if(qvm->program[qvm->ip].arg.u64 > qvm->program_size) {
        error = ERR_BAD_INST_PTR;
    } else if(dequeue(&qvm->queue).u64 == 0) {
        qvm->ip = qvm->program[qvm->ip].arg.u64 - 1; // minus one to account for increment at end of function
        error = ERR_OK;
    }
}

inline static void exec_jnz(Qvm *qvm) {
    if(qvm->queue.element_count < 1) {
        error = ERR_QUEUE_UNDERFLOW;
        return;
    }

    if(qvm->program[qvm->ip].arg.u64 > qvm->program_size) // ip is word which is uint (for now) and will always be read here as such - so no risk of negative ip
        error = ERR_BAD_INST_PTR;
    else if(dequeue(&qvm->queue).u64 != 0) {
        qvm->ip = qvm->program[qvm->ip].arg.u64 - 1; // minus one to account for increment at end of function
        error = ERR_OK;
    }
}

inline static void exec_puti(Qvm *qvm) {
    if(queue_empty(&qvm->queue))
        error = ERR_QUEUE_UNDERFLOW;
    else {
        printf("%ld\n", dequeue(&qvm->queue).i64);
        error = ERR_OK;
    }
}

inline static void exec_putu(Qvm *qvm) {
    if(queue_empty(&qvm->queue))
        error = ERR_QUEUE_UNDERFLOW;
    else {
        printf("%lu\n", dequeue(&qvm->queue).u64);
        error = ERR_OK;
    }
}

inline static void exec_putf(Qvm *qvm) {
    if(queue_empty(&qvm->queue))
        error = ERR_QUEUE_UNDERFLOW;
    else {
        printf("%lf\n", dequeue(&qvm->queue).f64);
        error = ERR_OK;
    }
}

inline static void exec_putptr(Qvm *qvm) {
    if(queue_empty(&qvm->queue))
        error = ERR_QUEUE_UNDERFLOW;
    else {
        printf("%p\n", dequeue(&qvm->queue).ptr64);
        error = ERR_OK;
    }
}

inline static void exec_illegal_inst() {
    error = ERR_ILLEGAL_INST;
}

static void (*exec_arr[])(Qvm *q) = {
    [INST_NOP] = exec_nop,
    [INST_ENQUEUE] = exec_enqueue,
    [INST_DEQUEUE] = exec_dequeue,
    [INST_ADDI] = exec_addi,
    [INST_SUBI] = exec_subi,
    [INST_ADDU] = exec_addu,
    [INST_SUBU] = exec_subu,
    [INST_ADDF] = exec_addf,
    [INST_SUBF] = exec_subf,
    [INST_MULI] = exec_muli,
    [INST_DIVI] = exec_divi,
    [INST_MULU] = exec_mulu,
    [INST_DIVU] = exec_divu,
    [INST_MULF] = exec_mulf,
    [INST_DIVF] = exec_divf,
    [INST_DUP] = exec_dup,
    [INST_SKIP] = exec_skip,
    [INST_CHEAT] = exec_cheat,
    [INST_LDA] = exec_lda,
    [INST_STA] = exec_sta,
    [INST_EQ] = exec_eq,
    [INST_JUMP] = exec_jump,
    [INST_JZ] = exec_jz,
    [INST_JNZ] = exec_jnz,
    [INST_PUTI] = exec_puti,  // the puts are temporary
    [INST_PUTU] = exec_putu,
    [INST_PUTF] = exec_putf,
    [INST_PUTPTR] = exec_putptr,
    [INST_HALT] = exec_illegal_inst,
    [INST_COUNT] = exec_illegal_inst
};

void qvm_inst_exec(Qvm *qvm) {
    assert(INST_COUNT == 29);
    exec_arr[qvm->program[qvm->ip].inst](qvm);
    qvm->ip++;

    error = (qvm->ip >= qvm->program_size) * ERR_BAD_INST_PTR + (qvm->ip < qvm->program_size) * error;
}

void qvm_run(Qvm *qvm, bool debug, int64_t limit) {
    if(limit != -1) {
        int i = 0;
        if(debug) {
            while(qvm->program[qvm->ip].inst != INST_HALT && i < limit) {
                printf("IP: %ld\n", qvm->ip);
                printf("Inst: %s\n", inst_as_str(qvm->program[qvm->ip].inst));
                qvm_inst_exec(qvm);
                if(error != ERR_OK) {
                    fprintf(stderr, "Runtime error: %s\n", err_as_str(error));
                    free(qvm->queue.data);
                    exit(1);
                }
                printf("A: u64: %lu i64: %ld f64: %f ptr: %p\n",  qvm->a.u64, qvm->a.i64, qvm->a.f64, qvm->a.ptr64);
                print_queue(&qvm->queue);
                getc(stdin);
                ++i;
            }
        } else {
            while(qvm->program[qvm->ip].inst != INST_HALT && i < limit) {
                qvm_inst_exec(qvm);
                if(error != ERR_OK) {
                    fprintf(stderr, "Runtime error: %s\n", err_as_str(error));
                    free(qvm->queue.data);
                    exit(1);
                }
                ++i;
            }
        }
    } else {
        if(debug) {
            while(qvm->program[qvm->ip].inst != INST_HALT) {
                printf("IP: %ld\n", qvm->ip);
                printf("Inst: %s\n", inst_as_str(qvm->program[qvm->ip].inst));
                qvm_inst_exec(qvm);
                if(error != ERR_OK) {
                    fprintf(stderr, "Runtime error: %s\n", err_as_str(error));
                    free(qvm->queue.data);
                    exit(1);
                }
                printf("A: u64: %lu i64: %ld f64: %f ptr: %p\n",  qvm->a.u64, qvm->a.i64, qvm->a.f64, qvm->a.ptr64);
                print_queue(&qvm->queue);
                getc(stdin);
            }
        } else {
            while(qvm->program[qvm->ip].inst != INST_HALT) {
                qvm_inst_exec(qvm);
                if(error != ERR_OK) {
                    fprintf(stderr, "Runtime error: %s\n", err_as_str(error));
                    free(qvm->queue.data);
                    exit(1);
                }
            }
        }
    }
}
