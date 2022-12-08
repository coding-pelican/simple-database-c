#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASSERT(condition, msg)                                   \
    if (!(condition)) {                                          \
        fprintf(stderr, "%s(%s: %d)\n", msg, __FILE__, __LINE__);\
        __builtin_trap();                                        \
    }                                                            \

#define ALLOC_SIZE 64

typedef struct {
    char* Buffer;
    size_t BufferLength;
    ssize_t InputLength;
} InputBuffer;

InputBuffer* NewInputBuffer() {
    InputBuffer* inputBuffer = (InputBuffer*)malloc(sizeof(InputBuffer));
    inputBuffer->Buffer = NULL;
    inputBuffer->BufferLength = 0;
    inputBuffer->InputLength = 0;

    return inputBuffer;
}

void PrintPrompt() { printf("db > "); }

size_t GetLine(char** lineptr, size_t* n, FILE* stream) {
    ASSERT(lineptr != NULL && n != NULL && stream != NULL, "NULLs in parameters are not allowed");
    if (!*lineptr) {
        *lineptr = (char*)malloc(ALLOC_SIZE);
        if (!*lineptr) return -1;
        else *n = ALLOC_SIZE;
    }
    char* cur = NULL;
    size_t len = 0;
    while (!feof(stream)) {
        if (!fgets(cur = *lineptr + len, (int)(*n - len), stream)) return -1;
        len += strlen(cur);
        if ((*lineptr)[len - 1] != '\n') { // 개행 문자 확인
            char* new = (char*)realloc(*lineptr, *n += ALLOC_SIZE);
            if (!new) return -1;
            else *lineptr = new;
        } else return (*lineptr)[--len] = 0, len;
    }
    return -1;
}

void ReadLine(InputBuffer* inputBuffer) {
    ssize_t bytesRead = GetLine(&(inputBuffer->Buffer), &(inputBuffer->BufferLength), stdin);
    if (bytesRead <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }
    //// Ignore trailing newline
    // inputBuffer->InputLength = bytesRead - 1;
    // inputBuffer->Buffer[bytesRead - 1] = 0;
}

void CloseInputBuffer(InputBuffer* inputBuffer) {
    free(inputBuffer->Buffer);
    free(inputBuffer);
}

int main(int argc, char const* argv[]) {
    InputBuffer* inputBuffer = NewInputBuffer();
    while (true) {
        PrintPrompt();
        ReadLine(inputBuffer);

        if (strcmp(inputBuffer->Buffer, ".exit") == 0) {
            CloseInputBuffer(inputBuffer);
            exit(EXIT_SUCCESS);
        } else {
            printf("Unrecognized command '%s'.\n", inputBuffer->Buffer);
        }
    }
    return 0;
}