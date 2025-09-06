PROGRAM = prog

# Компилятор
CC = gcc

# Флаги компиляции
CFLAGS = -Wall -Wextra -pedantic

LDFLAGS = -L. -lstatic_lib

LINK_NAMES = make_dir make_file make_sym_link make_hard_link\
			print_dir print_file print_sym_link print_file_using_sym_link print_perm_and_num_of_hard_links\
			rm_dir rm_file rm_sym_link rm_hard_link\
			change_perm


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

hard_links: $(PROGRAM)
	@for link in $(LINK_NAMES); do \
		if [ ! -e "$$link" ]; then \
			ln -f $(PROGRAM) "$$link"; \
		fi; \
	done

# Очистка временных файлов
clean:
	rm -f $(OBJS) $(PROGRAM)
	@for link in $(LINK_NAMES); do \
        if [ -e "$$link" ]; then \
            rm -f "$$link"; \
        fi; \
    done	
