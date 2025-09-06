PROGRAM = prog

# Компилятор
CC = gcc

# Флаги компиляции
CFLAGS = -Wall -Wextra -pedantic

SRCS = $(wildcard *.c)
# Генерируем список целевых исполняемых файлов (заменяем .c на '')
TARGETS = $(SRCS:.c=)

# Правило по умолчанию - компилируем все цели
all: $(TARGETS)

# Общее правило для компиляции .c файлов в исполняемые
%: %.c
	$(CC) $(CFLAGS) $< -o $@


# Очистка временных файлов
clean:
	rm -f $(TARGETS)
