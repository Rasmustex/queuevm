#include <qvm/qvm.h>
#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
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

// Tokeniser stuff should be moved into separate lib
// Alternatively, it could instead be simplified
// maybe add register and instruction as separate type
typedef enum {
    TOK_NAME,
    TOK_LABEL,
    TOK_INT,
    TOK_FLOAT,
} TOKEN_TYPE;

const char *token_type_as_string(TOKEN_TYPE t) {
    switch (t) {
    case TOK_NAME:  return "TOK_NAME";
    case TOK_LABEL: return "TOK_LABEL";
    case TOK_INT:   return "TOK_INT";
    case TOK_FLOAT: return "TOK_FLOAT";
    default: return "Unreachable token type";
    }
}

#define MAX_TOK 1024
typedef struct {
    char content[MAX_TOK];
    uint64_t lno, cno;
} Token;
Token token = {0};

TOKEN_TYPE tt;
TOKEN_TYPE lex(FILE *f, const char *fname);

typedef struct {
    char name[MAX_TOK];
    uint64_t ip, lno, cno;
} Label;

#define MAX_LABELS 1024
Label labels[MAX_LABELS];
uint64_t label_sp;
uint64_t get_label_ip(const char* name, int *err);
Label deferred_operands[MAX_LABELS];
uint64_t deferred_operands_sp;

uint64_t lineno, prev_charno, charno;
#define HERE(x, y, z, a) "%s:%lu:%lu: " y, x, z, a

Quasm quasm = {0};

int main(int argc, const char **argv) {
    if(argc < 2) {
        print_help(stderr, argv[0]);
        fprintf(stderr, "Error: no program name provided\n");
        return 1;
    }

    label_sp = deferred_operands_sp = charno = 0;
    lineno = 1;

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
    while((int)lex(f, iname) != EOF) {
        switch (tt) {
        case TOK_NAME:
            if(!inst_needs_arg) {
                if(!strcmp(token.content, "nop")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_NOP};
                    inst_needs_arg = false;
                }else if(!strcmp(token.content, "enq")) {
                    // TODO: change to allow enqueue from register
                    quasm.program[quasm.program_size] = (Inst) {.inst = INST_ENQUEUE};
                    inst_needs_arg = true;
                } else if(!strcmp(token.content, "deq")) {
                    // TODO: change to dequeue into register, and add drop inst
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_DEQUEUE};
                    inst_needs_arg = false;
                } else if(!strcmp(token.content, "addi")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_ADDI};
                    inst_needs_arg = false;
                } else if(!strcmp(token.content, "subi")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_SUBI};
                    inst_needs_arg = false;
                } else if(!strcmp(token.content, "addu")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_ADDU};
                    inst_needs_arg = false;
                } else if(!strcmp(token.content, "subu")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_SUBU};
                    inst_needs_arg = false;
                } else if(!strcmp(token.content, "addf")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_ADDF};
                    inst_needs_arg = false;
                } else if(!strcmp(token.content, "subf")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_SUBF};
                    inst_needs_arg = false;
                } else if(!strcmp(token.content, "muli")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_MULI};
                    inst_needs_arg = false;
                } else if(!strcmp(token.content, "divi")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_DIVI};
                    inst_needs_arg = false;
                } else if(!strcmp(token.content, "mulu")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_MULU};
                    inst_needs_arg = false;
                } else if(!strcmp(token.content, "divu")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_DIVU};
                    inst_needs_arg = false;
                } else if(!strcmp(token.content, "mulf")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_MULF};
                    inst_needs_arg = false;
                } else if(!strcmp(token.content, "divf")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_DIVF};
                    inst_needs_arg = false;
                } else if(!strcmp(token.content, "dup")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_DUP};
                    inst_needs_arg = false;
                } else if(!strcmp(token.content, "skip")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_SKIP};
                    inst_needs_arg = false;
                } else if(!strcmp(token.content, "cheat")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_CHEAT};
                    inst_needs_arg = false;
                } else if(!strcmp(token.content, "lda")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_LDA};
                    inst_needs_arg = false;
                } else if(!strcmp(token.content, "sta")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_STA};
                    inst_needs_arg = false;
                } else if(!strcmp(token.content, "deqa")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_DEQUEUE, .arg.u64 = 'a'};
                    inst_needs_arg = false;
                } else if(!strcmp(token.content, "eq")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_EQ};
                    inst_needs_arg = false;
                } else if(!strcmp(token.content, "jmp")) {
                    quasm.program[quasm.program_size] = (Inst) {.inst = INST_JUMP};
                    inst_needs_arg = true;
                } else if(!strcmp(token.content, "jz")) {
                    quasm.program[quasm.program_size] = (Inst) {.inst = INST_JZ};
                    inst_needs_arg = true;
                } else if(!strcmp(token.content, "jnz")) {
                    quasm.program[quasm.program_size] = (Inst) {.inst = INST_JNZ};
                    inst_needs_arg = true;
                } else if(!strcmp(token.content, "halt")) {
                    quasm.program[quasm.program_size++] = (Inst) {.inst = INST_HALT};
                    inst_needs_arg = false;
                } else {
                    fprintf(stderr, HERE(iname, "Error: unknown name '%s'\n", token.lno, token.cno), token.content);
                    fclose(f);
                    return 1;
                }
            } else {
                int err;
                uint64_t label_ip;
                label_ip = get_label_ip(token.content, &err);
                if(err) {
                    assert(label_sp < MAX_LABELS && "Exceeded label stack capacity");
                    strncpy(deferred_operands[deferred_operands_sp].name, token.content, MAX_LABELS);
                    deferred_operands[deferred_operands_sp].ip = quasm.program_size;
                    deferred_operands[deferred_operands_sp].lno = token.lno;
                    deferred_operands[deferred_operands_sp].cno = token.cno;
                    ++deferred_operands_sp;
                } else {
                    quasm.program[quasm.program_size].arg.u64 = label_ip;
                }
                quasm.program_size++;
                inst_needs_arg = false;
            }
            break;
        case TOK_LABEL:
            assert(label_sp < MAX_LABELS && "Exceeded label stack capacity");
            strncpy(labels[label_sp].name, token.content, MAX_TOK);
            labels[label_sp++].ip = quasm.program_size;
            break;
        case TOK_INT:
            if(inst_needs_arg) {
                quasm.program[quasm.program_size].arg.u64 = atoll(token.content);
                inst_needs_arg = false;
                quasm.program_size++;
            } else {
                fprintf(stderr, HERE(iname, "Error: instruction '%s' does not take a number argument\n", token.lno, token.cno), inst_as_str(quasm.program[quasm.program_size - 1].inst));
                fclose(f);
                return 1;
            }
            break;
        case TOK_FLOAT:
            if(inst_needs_arg) {
                quasm.program[quasm.program_size].arg.f64 = atof(token.content);
                inst_needs_arg = false;
                quasm.program_size++;
            } else {
                fprintf(stderr, HERE(iname, "Error: instruction '%s' does not take a number argument\n", token.lno, token.cno), token.content);
                fclose(f);
                return 1;
            }
            break;
        default:
            fprintf(stderr, HERE(iname, "Error: token '%s' is not recognised\n", token.lno, token.cno), token.content);
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    int err;
    uint64_t label_ip;
    for(uint64_t i = 0; i < deferred_operands_sp; ++i) {
        label_ip = get_label_ip(deferred_operands[i].name, &err);
        if(err) {
            fprintf(stderr, HERE(iname, "Error: name '%s' is not a valid instruction argument or label\n", deferred_operands[i].lno, deferred_operands[i].cno), deferred_operands[i].name);
            return 1;
        } else {
            quasm.program[deferred_operands[i].ip].arg.u64 = label_ip;
        }
    }
    quasm_dump_program_to_file(&quasm, oname);
}

int fgetcc(FILE *f) {
    int c;
    prev_charno = charno;
    if((c = fgetc(f)) == '\n') {
        charno = 0;
        ++lineno;
    }
    ++charno;
    return c;
}

void ungetcc(int c, FILE *f) {
    if(c == '\n') {
        --lineno;
        charno = prev_charno;
    } else {
        --charno;
    }
    ungetc(c, f);
}

TOKEN_TYPE lex(FILE *f, const char *fname) {
    char *p = token.content;
    uint64_t dotcount = 0;
    register char c;
    while((c = fgetcc(f)) == ' ' || c == '\t' || c == '\n')
        ;

    token.cno = charno;
    token.lno = lineno;

    if(isalpha(c)) {
        for(*p++ = c; (isalnum(c = fgetcc(f)) || c == '_') && p - token.content < MAX_TOK - 1;)
            *p++ = c;
        *p = '\0';
        if(!isspace(c) && c != ':' && c != EOF) {
            fprintf(stderr, HERE(fname, "Error: non-number token '%s' contains illegal character: '%c'\n", token.lno, token.cno), token.content, c);
            exit(1);
        }
        if(c == ':') {
            return tt = TOK_LABEL;
        } else {
            ungetcc(c, f);
            return tt = TOK_NAME;
        }
    } else if(isdigit(c) || c == '-') {
        for(*p++ = c; (isdigit(c = fgetcc(f)) || c == '.') && p - token.content < MAX_TOK - 1;) {
            if(c == '.')
                ++dotcount;
            *p++ = c;
        }
        *p = '\0';
        if(!isspace(c) && c != EOF) {
            fprintf(stderr, HERE(fname, "Error: number token '%s' cannot contain character that is neither number nor '.': '%c'\n", token.lno, token.cno), token.content, c);
            exit(1);
        }
        ungetcc(c, f);
        if(dotcount) {
            if(dotcount != 1) {
                fprintf(stderr, HERE(fname, "Error: number token '%s' contains too many '.'\n", token.lno, token.cno), token.content);
                exit(1);
            } else return tt = TOK_FLOAT;
        } else return tt = TOK_INT;
    } else if(c == ';') { // comment
        while((c = fgetcc(f)) != '\n' && c != EOF)
            ;
        if(c == EOF)
            return EOF;
        else return lex(f, fname);
    }
    else return tt = c;
}

uint64_t get_label_ip(const char* name, int *err) {
    for(uint64_t i = 0; i < label_sp; ++i) {
        if(!strcmp(name, labels[i].name)) {
            *err = 0;
            return labels[i].ip;
        }
    }
    *err = 1;
    return 0;
}
