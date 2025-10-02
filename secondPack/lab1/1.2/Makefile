# Компилятор
CC = gcc

# Флаги компиляции
CFLAGS = -Wall -Wextra -pedantic -lpthread

# Находим все исходные файлы *.c
SRCS = $(wildcard *.c)

# Формируем имена исполняемых файлов: prog.c -> prog
PROGRAMS = $(SRCS:.c=)

# Правило по умолчанию - компилируем все программы
all: $(PROGRAMS)

# Правило для компиляции каждой программы
$(PROGRAMS): %: %.c
	$(CC) $(CFLAGS) $< -o $@

# Очистка всех скомпилированных файлов
clean:
	rm -f $(PROGRAMS)

# Псевдоцель чтобы избежать конфликтов
.PHONY: all clean