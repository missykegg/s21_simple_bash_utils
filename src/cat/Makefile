CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11
TARGET = s21_cat

all: $(TARGET)

$(TARGET): s21_cat.c
	$(CC) $(CFLAGS) s21_cat.c -o $(TARGET)

clean:
	rm -f $(TARGET)

rebuild: clean all

.PHONY: all clean rebuild
