#include "../../include/utils.h"
#include "../../include/shell.h"

// String utilities
char* trim_whitespace(char* str) {
    char* end;
    
    // Trim leading space
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r') {
        str++;
    }
    
    if (*str == 0) return str; // All spaces
    
    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        end--;
    }
    
    // Write new null terminator
    *(end + 1) = '\0';
    
    return str;
}

char** split_string(char* str, char delimiter, int* count) {
    *count = 0;
    if (!str) return NULL;
    
    // Count tokens
    char* temp = str;
    while (*temp) {
        if (*temp == delimiter) (*count)++;
        temp++;
    }
    (*count)++; // Add one for the last token
    
    char** result = safe_malloc((*count) * sizeof(char*));
    
    char* token = strtok(str, &delimiter);
    int i = 0;
    while (token && i < *count) {
        result[i] = safe_strdup(token);
        token = strtok(NULL, &delimiter);
        i++;
    }
    
    *count = i;
    return result;
}

void free_string_array(char** arr, int count) {
    if (!arr) return;
    
    for (int i = 0; i < count; i++) {
        if (arr[i]) {
            free(arr[i]);
        }
    }
    free(arr);
}

int string_compare(const void* a, const void* b) {
    return strcmp(*(char**)a, *(char**)b);
}

// Memory utilities
void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) {
        perror("malloc");
        exit(1);
    }
    return ptr;
}

char* safe_strdup(char* str) {
    if (!str) return NULL;
    
    char* copy = safe_malloc(strlen(str) + 1);
    strcpy(copy, str);
    return copy;
}
