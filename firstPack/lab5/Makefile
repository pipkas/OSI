PROGRAM = prog

# Компилятор
CC = gcc

# Флаги компиляции
CFLAGS = -Wall -Wextra -pedantic

LDFLAGS = -L. -lstatic_lib

# Исходные файлы
SRCS = $(wildcard *.c)   # все файлы, которые удовлетворяют маске *.c

# Объектные файлы
OBJS = $(SRCS:.c=.o)

# Правило для создания исполняемого файла
$(PROGRAM): $(OBJS)        # PROGRAM зависит от OBJS
	$(CC) $(OBJS) -o $(PROGRAM)  

# Правило для создания объектных файлов
%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@  

# Очистка временных файлов
clean:
	rm -rf $(OBJS) $(PROGRAM)	
