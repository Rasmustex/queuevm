#ifndef QVM_EXEC_H
#define QVM_EXEC_H
#include <qvm/qvm.h>
ERR qvm_inst_exec(Qvm *qvm);
void qvm_run(Qvm *qvm, bool debug, int64_t limit);
#endif
