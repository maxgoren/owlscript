#include "util.h"

char *slurp_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    // Determine file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char *)malloc(sizeof(char)*(file_size + 1));
    if (buffer == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    size_t bytes_read = fread(buffer, 1, file_size, file);
    if (bytes_read != file_size) {
        perror("Error reading file");
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[file_size] = '\0';

    fclose(file);
    return buffer;
}