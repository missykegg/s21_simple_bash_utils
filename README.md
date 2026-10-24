# s21_simple_bash_utils

A School 21 core curriculum project: developing POSIX-compliant implementations of standard UNIX text utilities in C (`s21_cat` and `s21_grep`).

## 🛠 Features

### `s21_cat`
Supports standard GNU & POSIX flags:
- `-b` (`--number-nonblank`): Number non-empty output lines
- `-e` (implies `-v`): Display `$` at end of each line
- `-n` (`--number`): Number all output lines
- `-s` (`--squeeze-blank`): Suppress repeated empty output lines
- `-t` (implies `-v`): Display TAB characters as `^I`
- `-v`: Display non-printing characters

### `s21_grep`
Supports pattern searching with regular expressions:
- `-e`: Specify multiple search patterns
- `-i`: Ignore case distinctions
- `-v`: Invert match (select non-matching lines)
- `-c`: Output only a count of matching lines
- `-l`: Output only names of files with matches
- `-n`: Prefix each line with its 1-based line number
- `-h`: Suppress prefixing file names on output
- `-s`: Suppress error messages about nonexistent or unreadable files
- `-f file`: Take patterns from file
- `-o`: Print only matched (non-empty) parts of matching line

## 🚀 Building & Testing

```bash
# Build s21_cat
cd src/cat
make

# Build s21_grep
cd ../grep
make
```

### Static Analysis & Memory Checks
- Clang-format (Google style)
- Valgrind memory leak checks
- Cppcheck static analysis
