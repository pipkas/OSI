#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

const off_t BUFFER_SIZE = 1000;
const char MY_ERROR = 1;
const int SYS_ERROR = -1;

void reverse_string(char *str, int str_len) 
{
    for (int step = 0; step < str_len / 2; step++) {
        char save = str[step];
        str[step] = str[str_len - step - 1];
        str[str_len - step - 1] = save;
    }
}

static char reverse_file(int src_file, int dst_file, off_t size)
{
    char buffer[BUFFER_SIZE];
    off_t bytes_read, writed_bytes;
    off_t offset = size;
    while (offset > 0){
        off_t chunk_size;
        chunk_size = (offset >= BUFFER_SIZE) ? BUFFER_SIZE : offset;
        offset -= chunk_size; //new src file position
        int is_error = lseek(src_file, offset, SEEK_SET);
        if (is_error == SYS_ERROR){
            perror("Error setting cursor in source file\n");
            return MY_ERROR;
        }

        bytes_read = read(src_file, buffer, chunk_size);
        if (bytes_read == SYS_ERROR) {
            perror("Error reading source file\n");
            return MY_ERROR;
        }
        reverse_string(buffer, bytes_read);
        writed_bytes = write(dst_file, buffer, bytes_read);
        if (writed_bytes == SYS_ERROR) {
            perror("Error writing in destination file\n");
            return MY_ERROR;
        }
    }
    return 0;
}


char copy_file_reversed(char *src_path, char *dst_path) 
{
    int src_file = open(src_path, O_RDONLY);
    
    if (src_file == SYS_ERROR) {
        perror("Error opening file ");
        fprintf(stderr, "%s\n", src_path);
        return MY_ERROR;
    }
    struct stat st;
    int is_errors = fstat(src_file, &st);
    if (is_errors == SYS_ERROR) {
        perror("Error getting info about file ");
        fprintf(stderr, "%s\n", src_path);
        close(src_file);
        return MY_ERROR;
    }
    off_t size = st.st_size;
    int dst_file = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_file == SYS_ERROR) {
        perror("Error creating file ");
        fprintf(stderr, "%s\n", dst_path);
        close(src_file);
        return MY_ERROR;
    }
    is_errors = reverse_file(src_file, dst_file, size);
    close(src_file);
    close(dst_file);
    if (is_errors == MY_ERROR){
        fprintf(stderr, "Error in such files: %s %s\n", src_path, dst_path);
        return MY_ERROR;
    }
        
    return 0;
}