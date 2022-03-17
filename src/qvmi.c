#include <qvm/qvm.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

Qvm qvm = {0};

void print_help(FILE *f, const char *progname) {
    fprintf(f, "Usage: %s [OPTIONS] <program.qvm>\n", progname);
    fprintf(f, "OPTIONS:\n");
    fprintf(f, "\t-d            run program in debug mode\n");
    fprintf(f, "\t-i <limit>    limit program execution to i number of instructions\n");
    fprintf(f, "\t-h            print this help menu\n");
}

int main(int argc, const char **argv) {
    // parse args - currently not so elegant
    if(argc < 2) {
        print_help(stderr, argv[0]);
        fprintf(stderr, "Error: no program name provided\n");
        return 1;
    }
    const char *arg, *fname;
    uint64_t strindex;
    int dflag, iflag;
    int64_t execution_limit = -1;
    dflag = iflag = 0;
    fname = NULL;
    for(int i = 1; i < argc; ++i) {
        arg = argv[i];
        strindex = 0;
        if(arg[0] == '-') {
            while(arg[++strindex] != '\0') {
                switch (arg[strindex]) {
                case 'h':
                    print_help(stdout, argv[0]);
                    return 0;
                case 'd':
                    if(!dflag) {
                        ++dflag;
                    } else {
                        print_help(stderr, argv[0]);
                        fprintf(stderr, "Error: cannot set flag '%c' more than once\n", arg[strindex]);
                        return 1;
                    }
                    break;
                case 'i':
                    if(!iflag) {
                        iflag++;
                        if(strindex != strlen(arg) - 1) {
                            print_help(stderr, argv[0]);
                            fprintf(stderr, "Error: flag '%c' requires argument\n", arg[strindex]);
                            return 1;
                        }
                    } else {
                        print_help(stderr, argv[0]);
                        fprintf(stderr, "Error: cannot set flag '%c' more than once\n", arg[strindex]);
                        return 1;
                    }
                    break;
                default:
                    print_help(stderr, argv[0]);
                    fprintf(stderr, "Error: unknown flag '%c'\n", arg[strindex]);
                    return 1;
                }
            }
        } else if (iflag == 1) {
            iflag++;
            while(arg[strindex] != '\0') {
                if(!isdigit(arg[strindex])) {
                    print_help(stderr, argv[0]);
                    fprintf(stderr, "Error: instruction number set by 'i' flag has to be positive whole number\n");
                    return 1;
                }
                strindex++;
            }
            execution_limit = atol(arg);
        } else {
            if(fname == NULL) {
                fname = arg;
            } else {
                print_help(stderr, argv[0]);
                fprintf(stderr, "Error: unknown argument: '%s'\n", arg);
                return 1;
            }
        }
    }
    if(iflag && (execution_limit == -1)) {
        print_help(stderr, argv[0]);
        fprintf(stderr, "Error: no limit provided for 'i' flag\n");
        return 1;
    }
    // initialise data queue
    queue_init(&qvm.queue);
    qvm_load_program_from_file(&qvm, fname);
    qvm_run(&qvm, dflag, execution_limit);
    free(qvm.queue.data);
}
