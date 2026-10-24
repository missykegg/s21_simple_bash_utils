CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11
TARGET = s21_grep

all: $(TARGET)

$(TARGET): s21_grep.c
	$(CC) $(CFLAGS) s21_grep.c -o $(TARGET)

clean:
	rm -f $(TARGET)

rebuild: clean all

.PHONY: all clean rebuild
