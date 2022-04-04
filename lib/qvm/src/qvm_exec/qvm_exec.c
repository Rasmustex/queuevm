#include <queue/queue.h>
#include <qvm/qvm.h>
#include <assert.h>
#include <qvm_exec/qvm_exec.h>

// todo: file-global static err to make exec functions inlineable
ERR exec_nop() {
    return ERR_OK;
}

ERR exec_enqueue(Qvm *qvm) {
    if(enqueue(&qvm->queue, qvm->program[qvm->ip].arg) != 0)
        return ERR_QUEUE_REALLOC;
    else
        return ERR_OK;
}
// TODO: enqueue into a register
ERR exec_dequeue(Qvm *qvm) {
    if(queue_empty(&qvm->queue))
        return ERR_QUEUE_UNDERFLOW;
    else
        dequeue(&qvm->queue);
    return ERR_OK;
}

ERR exec_addi(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        return ERR_QUEUE_UNDERFLOW;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.i64 = a.i64 + dequeue(&qvm->queue).i64}) != 0)
        return ERR_QUEUE_REALLOC;
    else
        return ERR_OK;
}

ERR exec_subi(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        return ERR_QUEUE_UNDERFLOW;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.i64 = dequeue(&qvm->queue).i64 - a.i64}) != 0)
        return ERR_QUEUE_REALLOC;
    else
        return ERR_OK;
}

ERR exec_addu(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        return ERR_QUEUE_UNDERFLOW;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.u64 = a.u64 + dequeue(&qvm->queue).u64}) != 0)
        return ERR_QUEUE_REALLOC;
    else
        return ERR_OK;
}

ERR exec_subu(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        return ERR_QUEUE_UNDERFLOW;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.u64 = dequeue(&qvm->queue).u64 - a.u64}) != 0)
        return ERR_QUEUE_REALLOC;
    else
        return ERR_OK;
}

ERR exec_addf(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        return ERR_QUEUE_UNDERFLOW;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.f64 = a.f64 + dequeue(&qvm->queue).f64}) != 0)
        return ERR_QUEUE_REALLOC;
    else
        return ERR_OK;
}

ERR exec_subf(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        return ERR_QUEUE_UNDERFLOW;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.f64 = dequeue(&qvm->queue).f64 - a.f64}) != 0)
        return ERR_QUEUE_REALLOC;
    else
        return ERR_OK;
}

ERR exec_muli(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        return ERR_QUEUE_UNDERFLOW;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.i64 = dequeue(&qvm->queue).i64 * a.i64}) != 0)
        return ERR_QUEUE_REALLOC;
    else
        return ERR_OK;
}

ERR exec_divi(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        return ERR_QUEUE_UNDERFLOW;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.i64 = dequeue(&qvm->queue).i64 / a.i64}) != 0)
        return ERR_QUEUE_REALLOC;
    else
        return ERR_OK;
}

ERR exec_mulu(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        return ERR_QUEUE_UNDERFLOW;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.u64 = dequeue(&qvm->queue).u64 * a.u64}) != 0)
        return ERR_QUEUE_REALLOC;
    else
        return ERR_OK;
}

ERR exec_divu(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        return ERR_QUEUE_UNDERFLOW;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.u64 = dequeue(&qvm->queue).u64 / a.u64}) != 0)
        return ERR_QUEUE_REALLOC;
    else
        return ERR_OK;
}

ERR exec_mulf(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        return ERR_QUEUE_UNDERFLOW;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.f64 = dequeue(&qvm->queue).f64 * a.f64}) != 0)
        return ERR_QUEUE_REALLOC;
    else
        return ERR_OK;
}

ERR exec_divf(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        return ERR_QUEUE_UNDERFLOW;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.f64 = dequeue(&qvm->queue).f64 / a.f64}) != 0)
        return ERR_QUEUE_REALLOC;
    else
        return ERR_OK;
}

ERR exec_dup(Qvm *qvm) {
    if(queue_empty(&qvm->queue))
        return ERR_QUEUE_UNDERFLOW;
    else if(enqueue(&qvm->queue, queue_front(&qvm->queue)) != 0)
        return ERR_QUEUE_REALLOC;
    else
        return ERR_OK;
}

ERR exec_skip(Qvm *qvm) {
    if(qvm->queue.element_count < 2)
        return ERR_QUEUE_UNDERFLOW;

    queue_skip(&qvm->queue);
    return ERR_OK;
}

ERR exec_cheat(Qvm *qvm) {
    if(qvm->queue.element_count < 2)
        return ERR_QUEUE_UNDERFLOW;

    queue_cheat(&qvm->queue);
    return ERR_OK;
}

ERR exec_lda(Qvm *qvm) {
    if(queue_empty(&qvm->queue))
        return ERR_QUEUE_UNDERFLOW;
    qvm->a = queue_front(&qvm->queue);
    return ERR_OK;
}

ERR exec_sta(Qvm *qvm) {
    if(enqueue(&qvm->queue, qvm->a) != 0)
        return ERR_QUEUE_REALLOC;
    else
        return ERR_OK;
}

ERR exec_eq(Qvm *qvm) {
    if(qvm->queue.element_count < 2) {
        return ERR_QUEUE_UNDERFLOW;
    }
    Word a = dequeue(&qvm->queue);
    if(enqueue(&qvm->queue, (Word){.i64 = a.i64 == dequeue(&qvm->queue).i64}) != 0)
        return ERR_QUEUE_REALLOC;
    else
        return ERR_OK;
}

ERR exec_jump(Qvm *qvm) {
    if(qvm->program[qvm->ip].arg.u64 > qvm->program_size)
        return ERR_BAD_INST_PTR;
    else
        qvm->ip = qvm->program[qvm->ip].arg.u64 - 1; // minus one to account for increment at end of function
    return ERR_OK;
}

ERR exec_jz(Qvm *qvm) {
    if(qvm->queue.element_count < 1)
        return ERR_QUEUE_UNDERFLOW;

    if(qvm->program[qvm->ip].arg.u64 > qvm->program_size) // ip is word which is uint (for now) and will always be read here as such - so no risk of negative ip
        return ERR_BAD_INST_PTR;
    else if(dequeue(&qvm->queue).u64 == 0)
        qvm->ip = qvm->program[qvm->ip].arg.u64 - 1; // minus one to account for increment at end of function
    return ERR_OK;
}

ERR exec_jnz(Qvm *qvm) {
    if(qvm->queue.element_count < 1)
        return ERR_QUEUE_UNDERFLOW;

    if(qvm->program[qvm->ip].arg.u64 > qvm->program_size) // ip is word which is uint (for now) and will always be read here as such - so no risk of negative ip
        return ERR_BAD_INST_PTR;
    else if(dequeue(&qvm->queue).u64 != 0)
        qvm->ip = qvm->program[qvm->ip].arg.u64 - 1; // minus one to account for increment at end of function
    return ERR_OK;
}

ERR exec_puti(Qvm *qvm) {
    if(queue_empty(&qvm->queue))
        return ERR_QUEUE_UNDERFLOW;
    printf("%ld\n", dequeue(&qvm->queue).i64);
    return ERR_OK;
}

ERR exec_putu(Qvm *qvm) {
    if(queue_empty(&qvm->queue))
        return ERR_QUEUE_UNDERFLOW;
    printf("%lu\n", dequeue(&qvm->queue).u64);
    return ERR_OK;
}

ERR exec_putf(Qvm *qvm) {
    if(queue_empty(&qvm->queue))
        return ERR_QUEUE_UNDERFLOW;
    printf("%lf\n", dequeue(&qvm->queue).f64);
    return ERR_OK;
}

ERR exec_putptr(Qvm *qvm) {
    if(queue_empty(&qvm->queue))
        return ERR_QUEUE_UNDERFLOW;
    printf("%p\n", dequeue(&qvm->queue).ptr64);
    return ERR_OK;
}

ERR exec_illegal_inst() {
    return ERR_ILLEGAL_INST;
}

ERR qvm_inst_exec(Qvm *qvm) {
    assert(INST_COUNT == 29);
    static ERR err;
    static ERR (*exec_arr[])(Qvm *q) = {
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
    err = exec_arr[qvm->program[qvm->ip].inst](qvm);
    qvm->ip++;

    return (qvm->ip >= qvm->program_size) * ERR_BAD_INST_PTR + (qvm->ip < qvm->program_size) * err;
}

void qvm_run(Qvm *qvm, bool debug, int64_t limit) {
    ERR err;
    if(limit != -1) {
        int i = 0;
        if(debug) {
            while(qvm->program[qvm->ip].inst != INST_HALT && i < limit) {
                printf("IP: %ld\n", qvm->ip);
                printf("Inst: %s\n", inst_as_str(qvm->program[qvm->ip].inst));
                if((err = qvm_inst_exec(qvm))!= ERR_OK) {
                    fprintf(stderr, "Runtime error: %s\n", err_as_str(err));
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
                if((err = qvm_inst_exec(qvm))!= ERR_OK) {
                    fprintf(stderr, "Runtime error: %s\n", err_as_str(err));
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
                if((err = qvm_inst_exec(qvm))!= ERR_OK) {
                    fprintf(stderr, "Runtime error: %s\n", err_as_str(err));
                    free(qvm->queue.data);
                    exit(1);
                }
                printf("A: u64: %lu i64: %ld f64: %f ptr: %p\n",  qvm->a.u64, qvm->a.i64, qvm->a.f64, qvm->a.ptr64);
                print_queue(&qvm->queue);
                getc(stdin);
            }
        } else {
            while(qvm->program[qvm->ip].inst != INST_HALT) {
                if((err = qvm_inst_exec(qvm))!= ERR_OK) {
                    fprintf(stderr, "Runtime error: %s\n", err_as_str(err));
                    free(qvm->queue.data);
                    exit(1);
                }
            }
        }
    }
}
