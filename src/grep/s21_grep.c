#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#define MAX_PATTERNS 256
#define MAX_LINE_LEN 4096

typedef struct {
    int e;  // pattern
    int i;  // ignore case
    int v;  // invert match
    int c;  // count matching lines
    int l;  // matching files only
    int n;  // line number
    int h;  // suppress filename header
    int s;  // suppress error messages
    int f;  // patterns from file
    int o;  // print only matching part
} GrepFlags;

typedef struct {
    char *patterns[MAX_PATTERNS];
    int count;
} PatternList;

void add_pattern(PatternList *pl, const char *pat) {
    if (pl->count < MAX_PATTERNS) {
        pl->patterns[pl->count++] = strdup(pat);
    }
}

void load_patterns_from_file(PatternList *pl, const char *filepath, int suppress_errors) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        if (!suppress_errors) {
            fprintf(stderr, "s21_grep: %s: No such file or directory\n", filepath);
        }
        return;
    }
    char line[MAX_LINE_LEN];
    while (fgets(line, sizeof(line), fp)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        add_pattern(pl, line);
    }
    fclose(fp);
}

void process_grep_file(const char *filename, PatternList *pl, GrepFlags flags, int multiple_files) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        if (!flags.s) {
            fprintf(stderr, "s21_grep: %s: No such file or directory\n", filename);
        }
        return;
    }

    // Compile regexes
    regex_t regexes[MAX_PATTERNS];
    int cflags = REG_EXTENDED;
    if (flags.i) {
        cflags |= REG_ICASE;
    }

    for (int i = 0; i < pl->count; i++) {
        regcomp(&regexes[i], pl->patterns[i], cflags);
    }

    char line[MAX_LINE_LEN];
    int line_num = 0;
    int match_count = 0;

    while (fgets(line, sizeof(line), fp)) {
        line_num++;
        int matched = 0;
        regmatch_t pmatch;

        for (int i = 0; i < pl->count; i++) {
            if (regexec(&regexes[i], line, 1, &pmatch, 0) == 0) {
                matched = 1;
                break;
            }
        }

        if (flags.v) {
            matched = !matched;
        }

        if (matched) {
            match_count++;
            if (flags.l) {
                break;
            }
            if (!flags.c) {
                if (multiple_files && !flags.h) {
                    printf("%s:", filename);
                }
                if (flags.n) {
                    printf("%d:", line_num);
                }
                if (flags.o && !flags.v) {
                    // print only matching part
                    char *cursor = line;
                    while (1) {
                        int best_start = -1;
                        int best_end = -1;
                        for (int i = 0; i < pl->count; i++) {
                            if (regexec(&regexes[i], cursor, 1, &pmatch, 0) == 0) {
                                if (best_start == -1 || pmatch.rm_so < best_start) {
                                    best_start = pmatch.rm_so;
                                    best_end = pmatch.rm_eo;
                                }
                            }
                        }
                        if (best_start == -1 || best_start == best_end) break;
                        for (int k = best_start; k < best_end; k++) {
                            putchar(cursor[k]);
                        }
                        putchar('\n');
                        cursor += best_end;
                    }
                } else {
                    printf("%s", line);
                    size_t len = strlen(line);
                    if (len > 0 && line[len - 1] != '\n') {
                        putchar('\n');
                    }
                }
            }
        }
    }

    if (flags.c) {
        if (multiple_files && !flags.h) {
            printf("%s:", filename);
        }
        if (flags.l) {
            printf("%d\n", match_count > 0 ? 1 : 0);
        } else {
            printf("%d\n", match_count);
        }
    }

    if (flags.l && match_count > 0) {
        printf("%s\n", filename);
    }

    for (int i = 0; i < pl->count; i++) {
        regfree(&regexes[i]);
    }

    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;

    GrepFlags flags = {0};
    PatternList pl = {0};
    int file_start = 1;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            for (size_t j = 1; j < strlen(argv[i]); j++) {
                switch (argv[i][j]) {
                    case 'e':
                        flags.e = 1;
                        if (j + 1 < strlen(argv[i])) {
                            add_pattern(&pl, &argv[i][j + 1]);
                            j = strlen(argv[i]);
                        } else if (i + 1 < argc) {
                            add_pattern(&pl, argv[++i]);
                        }
                        break;
                    case 'i': flags.i = 1; break;
                    case 'v': flags.v = 1; break;
                    case 'c': flags.c = 1; break;
                    case 'l': flags.l = 1; break;
                    case 'n': flags.n = 1; break;
                    case 'h': flags.h = 1; break;
                    case 's': flags.s = 1; break;
                    case 'f':
                        flags.f = 1;
                        if (j + 1 < strlen(argv[i])) {
                            load_patterns_from_file(&pl, &argv[i][j + 1], flags.s);
                            j = strlen(argv[i]);
                        } else if (i + 1 < argc) {
                            load_patterns_from_file(&pl, argv[++i], flags.s);
                        }
                        break;
                    case 'o': flags.o = 1; break;
                    default:
                        fprintf(stderr, "s21_grep: invalid option -- '%c'\n", argv[i][j]);
                        return 1;
                }
            }
        } else {
            if (pl.count == 0) {
                add_pattern(&pl, argv[i]);
                file_start = i + 1;
            } else {
                file_start = i;
            }
            break;
        }
    }

    int file_count = argc - file_start;
    int multiple_files = file_count > 1;

    for (int i = file_start; i < argc; i++) {
        process_grep_file(argv[i], &pl, flags, multiple_files);
    }

    for (int i = 0; i < pl.count; i++) {
        free(pl.patterns[i]);
    }

    return 0;
}
