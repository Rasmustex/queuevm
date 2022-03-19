#include <qvm/qvm.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void print_help(FILE *f, const char *progname) {
    fprintf(f, "Usage: %s [OPTIONS] <filename.quasm>\t assembles quasm program to qvm bytecode\n", progname);
    fprintf(f, "OPTIONS:\n");
    fprintf(f, "\t-o <oname>    sets output to file with name <oname>\n");
    fprintf(f, "\t-h            print this help menu\n");
}

Quasm quasm = {0};
// Tokeniser stuff should be moved into separate lib
// Alternatively, it could instead be simplified
#define MAX_TOK 1024
char token[MAX_TOK];
// maybe add register and instruction as separate type
typedef enum {
    NAME,
    TOK_INT,
    TOK_FLOAT,
} TOKEN_TYPE;

const char *token_type_as_string(TOKEN_TYPE t) {
    switch (t) {
    case NAME: return "NAME";
    case TOK_INT: return "TOK_INT";
    case TOK_FLOAT: return "TOK_FLOAT";
    default: return "Unreachable token type";
    }
}

TOKEN_TYPE tt;
TOKEN_TYPE lex(FILE *f);

int main(int argc, const char **argv) {
    if(argc < 2) {
        print_help(stderr, argv[0]);
        fprintf(stderr, "Error: no program name provided\n");
        return 1;
    }
    const char *arg, *iname, *oname;
    iname = NULL;
    oname = "out.qvm";
    int oflag = 0;
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
                case 'o':
                    if(!oflag) {
                        ++oflag;
                        if(strindex != strlen(arg) - 1) {
                            print_help(stderr, argv[0]);
                            fprintf(stderr, "Error: flag '-%c' requires argument\n", arg[strindex]);
                            return 1;
                        }
                    } else {
                        print_help(stderr, argv[0]);
                        fprintf(stderr, "Error: flag '-%c': cannot define file name multiple times\n", arg[strindex]);
                        return 1;
                    }
                    break;
                default:
                    print_help(stderr, argv[0]);
                    fprintf(stderr, "Error: unknown flag '%c'\n", arg[strindex]);
                    return 1;
                }
            }
        } else if(oflag == 1) {
            ++oflag;
            oname = arg;
        } else if(iname == NULL) {
            iname = arg;
        } else {
            print_help(stderr, argv[0]);
            fprintf(stderr, "Error: unknown argument: '%s'\n", arg);
            return 1;
        }
    }
    FILE *f = fopen(iname, "r");
    if(f == NULL) {
        fprintf(stderr, "Error: Failed to open file '%s'\n", iname);
        return 1;
    }

    bool inst_needs_arg = false;
    while((int)lex(f) != EOF) {
        switch (tt) {
        case NAME:
            if(!inst_needs_arg) {
                if(!strcmp(token, "nop")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_NOP};
                    inst_needs_arg = false;
                }else if(!strcmp(token, "enq")) {
                    // TODO: change to allow enqueue from register
                    quasm.program[quasm.program_size] = (Inst) {.inst = INST_ENQUEUE};
                    inst_needs_arg = true;
                } else if(!strcmp(token, "deq")) {
                    // TODO: change to dequeue into register, and add drop inst
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_DEQUEUE};
                    inst_needs_arg = false;
                } else if(!strcmp(token, "addi")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_ADDI};
                    inst_needs_arg = false;
                } else if(!strcmp(token, "subi")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_SUBI};
                    inst_needs_arg = false;
                } else if(!strcmp(token, "addu")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_ADDU};
                    inst_needs_arg = false;
                } else if(!strcmp(token, "subu")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_SUBU};
                    inst_needs_arg = false;
                } else if(!strcmp(token, "addf")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_ADDF};
                    inst_needs_arg = false;
                } else if(!strcmp(token, "subf")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_SUBF};
                    inst_needs_arg = false;
                } else if(!strcmp(token, "dup")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_DUP};
                    inst_needs_arg = false;
                } else if(!strcmp(token, "skip")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_SKIP};
                    inst_needs_arg = false;
                } else if(!strcmp(token, "cheat")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_CHEAT};
                    inst_needs_arg = false;
                } else if(!strcmp(token, "lda")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_LDA};
                    inst_needs_arg = false;
                } else if(!strcmp(token, "sta")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_STA};
                    inst_needs_arg = false;
                } else if(!strcmp(token, "deqa")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_DEQUEUE, .arg.u64 = 'a'};
                    inst_needs_arg = false;
                } else if(!strcmp(token, "eq")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_EQ};
                    inst_needs_arg = false;
                } else if(!strcmp(token, "jmp")) {
                    quasm.program[quasm.program_size] = (Inst) {.inst = INST_JUMP};
                    inst_needs_arg = true;
                } else if(!strcmp(token, "jz")) {
                    quasm.program[quasm.program_size] = (Inst) {.inst = INST_JZ};
                    inst_needs_arg = true;
                } else if(!strcmp(token, "jnz")) {
                    quasm.program[quasm.program_size] = (Inst) {.inst = INST_JNZ};
                    inst_needs_arg = true;
                } else if(!strcmp(token, "halt")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_HALT};
                    inst_needs_arg = false;
                } else {
                    fprintf(stderr, "Error: unknown name '%s'\n", token);
                    fclose(f);
                    return 1;
                }
            } else {
                fprintf(stderr, "Error: name '%s' is not a valid instruction argument\n", token);
                fclose(f);
                return 1;
            }
            break;
        case TOK_INT:
            if(inst_needs_arg) {
                quasm.program[quasm.program_size].arg.u64 = atoll(token);
                inst_needs_arg = false;
                quasm.program_size++;
            } else {
                fprintf(stderr, "Error: instruction '%s' does not take a number argument\n", token);
                fclose(f);
                return 1;
            }
            break;
        case TOK_FLOAT:
            if(inst_needs_arg) {
                quasm.program[quasm.program_size].arg.f64 = atof(token);
                inst_needs_arg = false;
                quasm.program_size++;
            } else {
                fprintf(stderr, "Error: instruction '%s' does not take a number argument\n", token);
                fclose(f);
                return 1;
            }
            break;
        default:
            fprintf(stderr, "Error: token '%s' is not recognised\n", token);
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    quasm_dump_program_to_file(&quasm, oname);
}

TOKEN_TYPE lex(FILE *f) {
    char *p = token;
    uint64_t dotcount = 0;
    register char c;
    while((c = fgetc(f)) == ' ' || c == '\t' || c == '\n')
        ;
    if(isalpha(c)) {
        for(*p++ = c; (isalnum(c = fgetc(f)) || c == '_') && p - token < MAX_TOK - 1;)
            *p++ = c;
        *p = '\0';
        if(!isspace(c) && c != EOF) {
            fprintf(stderr, "Error: non-number token '%s' contains illegal character: '%c'\n", token, c);
            exit(1);
        }
        ungetc(c, f);
        return tt = NAME;
    } else if(isdigit(c) || c == '-') {
        for(*p++ = c; (isdigit(c = fgetc(f)) || c == '.') && p - token < MAX_TOK - 1;) {
            if(c == '.')
                ++dotcount;
            *p++ = c;
        }
        *p = '\0';
        if(!isspace(c) && c != EOF) {
            fprintf(stderr, "Error: number token '%s' cannot contain character that is neither number nor '.': '%c'\n", token, c);
            exit(1);
        }
        ungetc(c, f);
        if(dotcount) {
            if(dotcount != 1) {
                fprintf(stderr, "Error: number token '%s' contains too many '.'\n", token);
                exit(1);
            } else return tt = TOK_FLOAT;
        } else return tt = TOK_INT;
    } else if(c == ';') { // comment
        while((c = fgetc(f)) != '\n' && c != EOF)
            ;
        if(c == EOF)
            return EOF;
        else return lex(f);
    }
    else return tt = c;
}
