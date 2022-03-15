#include <stdio.h>
#include <qvm/qvm.h>

Qvm qvm = {0};

int main() {
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
}
