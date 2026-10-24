#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int b;  // number non-blank lines
    int e;  // display $ at end of line (and show non-printing like -v)
    int n;  // number all lines
    int s;  // squeeze blank lines
    int t;  // display TAB as ^I (and show non-printing like -v)
    int v;  // show non-printing
} Flags;

void print_non_printing(unsigned char ch) {
    if (ch >= 128) {
        printf("M-");
        ch -= 128;
    }
    if (ch < 32 && ch != '\n' && ch != '\t') {
        printf("^%c", ch + 64);
    } else if (ch == 127) {
        printf("^?");
    } else {
        putchar(ch);
    }
}

void process_file(FILE *fp, Flags flags) {
    int line_number = 1;
    int blank_count = 0;
    int prev_char = '\n';
    int ch;

    while ((ch = fgetc(fp)) != EOF) {
        if (prev_char == '\n') {
            if (ch == '\n') {
                blank_count++;
                if (flags.s && blank_count > 1) {
                    continue;
                }
            } else {
                blank_count = 0;
            }

            if (flags.b) {
                if (ch != '\n') {
                    printf("%6d\t", line_number++);
                }
            } else if (flags.n) {
                printf("%6d\t", line_number++);
            }
        }

        if (ch == '\n') {
            if (flags.e) {
                putchar('$');
            }
            putchar('\n');
        } else if (ch == '\t') {
            if (flags.t) {
                printf("^I");
            } else {
                putchar('\t');
            }
        } else if (flags.v) {
            print_non_printing((unsigned char)ch);
        } else {
            putchar(ch);
        }

        prev_char = ch;
    }
}

int parse_flags(int argc, char *argv[], Flags *flags, int *file_index) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "--number-nonblank") == 0) {
                flags->b = 1;
            } else if (strcmp(argv[i], "--number") == 0) {
                flags->n = 1;
            } else if (strcmp(argv[i], "--squeeze-blank") == 0) {
                flags->s = 1;
            } else {
                for (size_t j = 1; j < strlen(argv[i]); j++) {
                    switch (argv[i][j]) {
                        case 'b': flags->b = 1; break;
                        case 'e': flags->e = 1; flags->v = 1; break;
                        case 'E': flags->e = 1; break;
                        case 'n': flags->n = 1; break;
                        case 's': flags->s = 1; break;
                        case 't': flags->t = 1; flags->v = 1; break;
                        case 'T': flags->t = 1; break;
                        case 'v': flags->v = 1; break;
                        default:
                            fprintf(stderr, "s21_cat: invalid option -- '%c'\n", argv[i][j]);
                            return -1;
                    }
                }
            }
        } else {
            *file_index = i;
            return 0;
        }
    }
    *file_index = argc;
    return 0;
}

int main(int argc, char *argv[]) {
    Flags flags = {0};
    int file_index = 1;

    if (parse_flags(argc, argv, &flags, &file_index) != 0) {
        return 1;
    }

    if (file_index >= argc) {
        process_file(stdin, flags);
    } else {
        for (int i = file_index; i < argc; i++) {
            FILE *fp = fopen(argv[i], "rb");
            if (!fp) {
                fprintf(stderr, "s21_cat: %s: No such file or directory\n", argv[i]);
                continue;
            }
            process_file(fp, flags);
            fclose(fp);
        }
    }

    return 0;
}
