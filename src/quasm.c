#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

void print_help(FILE *f, const char *progname) {
    fprintf(f, "Usage: %s [OPTIONS] <filename.quasm>\t assembles quasm program to qvm bytecode\n", progname);
    fprintf(f, "OPTIONS:\n");
    fprintf(f, "\t-h            print this help menu\n");
}

int main(int argc, const char **argv) {
    if(argc < 2) {
        print_help(stderr, argv[0]);
        fprintf(stderr, "Error: no program name provided\n");
        return 1;
    }
    const char *arg, *fname;
    fname = NULL;
    size_t strindex;
    for(int i = 1; i < argc; ++i) {
        arg = argv[i];
        strindex = 0;
        if(arg[0] == '-') {
            while(arg[++strindex] != '\0') {
                switch (arg[strindex]) {
                case 'h':
                    print_help(stdout, argv[0]);
                    return 0;
                default:
                    print_help(stderr, argv[0]);
                    fprintf(stderr, "Error: unknown flag '%c'\n", arg[strindex]);
                    return 1;
                }
            }
        } else if(fname == NULL) {
            fname = arg;
        } else {
            print_help(stderr, argv[0]);
            fprintf(stderr, "Error: unknown argument: '%s'\n", arg);
            return 1;
        }
    }
    assert(0 && "quasm is not yet implemented");
}
