#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASSERT(condition, msg)                                   \
    if (!(condition)) {                                          \
        fprintf(stderr, "%s(%s: %d)\n", msg, __FILE__, __LINE__);\
        __builtin_trap();                                        \
    }                                                            \

typedef struct {
    char* Buffer;
    size_t BufferLength;
    ssize_t InputLength;
} InputBuffer;

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} eMetaCommandResult;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT
} ePrepareResult;

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} eStatementType;

typedef struct {
    eStatementType Type;
} Statement;

const int ALLOC_SIZE = 64;

InputBuffer* NewInputBuffer() {
    InputBuffer* inputBuffer = (InputBuffer*)malloc(sizeof(InputBuffer));
    inputBuffer->Buffer = NULL;
    inputBuffer->BufferLength = 0;
    inputBuffer->InputLength = 0;

    return inputBuffer;
}

void PrintPrompt() { printf("db > "); }

ssize_t GetLine(char** lineptr, size_t* n, FILE* stream) {
    ASSERT(lineptr != NULL && n != NULL && stream != NULL, "NULLs in parameters are not allowed");
    if (!*lineptr) {
        *lineptr = (char*)malloc(ALLOC_SIZE);
        if (!*lineptr) {
            return -1;
        } else {
            *n = ALLOC_SIZE;
        }
    }
    char* cur = NULL;
    size_t len = 0;
    while (!feof(stream)) {
        cur = *lineptr + len;
        if (!fgets(cur, (int)(*n - len), stream)) {
            return -1;
        }
        len += strlen(cur);
        if ((*lineptr)[len - 1] != '\n') { // * 개행 문자 확인
            char* new = (char*)realloc(*lineptr, *n += ALLOC_SIZE);
            if (!new) {
                return -1;
            } else {
                *lineptr = new;
            }
        } else {
            return (*lineptr)[--len] = 0, len; // NOLINT
        }
    }
    return -1;
}

void ReadLine(InputBuffer* inputBuffer) {
    ssize_t bytesRead = GetLine(&(inputBuffer->Buffer), &(inputBuffer->BufferLength), stdin);
    if (bytesRead <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE); // NOLINT
    }
    // * Ignore trailing newline
    // inputBuffer->InputLength = bytesRead - 1;
    // inputBuffer->Buffer[bytesRead - 1] = 0;
}

void CloseInputBuffer(InputBuffer* inputBuffer) {
    free(inputBuffer->Buffer);
    free(inputBuffer);
}

eMetaCommandResult DoMetaCommand(InputBuffer* inputBuffer) {
    if (strcmp(inputBuffer->Buffer, ".exit") == 0) {
        exit(EXIT_SUCCESS); // NOLINT
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

ePrepareResult PrepareStatement(InputBuffer* inputBuffer, Statement* statement) {
    if (strncmp(inputBuffer->Buffer, "insert", 6) == 0) { // NOLINT
        statement->Type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }
    if (strcmp(inputBuffer->Buffer, "select") == 0) {
        statement->Type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void ExecuteStatement(Statement* statement) {
    switch (statement->Type) {
    case (STATEMENT_INSERT):
        printf("This is where we would do an insert.\n");
        break;
    case (STATEMENT_SELECT):
        printf("This is where we would do a select.\n");
        break;
    }
}

int main(int argc, char const* argv[]) {
    InputBuffer* inputBuffer = NewInputBuffer();
    while (true) {
        PrintPrompt();
        ReadLine(inputBuffer);

        if (inputBuffer->Buffer[0] == '.') {
            switch (DoMetaCommand(inputBuffer)) {
            case (META_COMMAND_SUCCESS):
                continue;
            case (META_COMMAND_UNRECOGNIZED_COMMAND):
                printf("Unrecognized command '%s'\n", inputBuffer->Buffer);
                continue;
            }
        }

        Statement statement;
        switch (PrepareStatement(inputBuffer, &statement)) {
        case (PREPARE_SUCCESS):
            break;
        case (PREPARE_UNRECOGNIZED_STATEMENT):
            printf("Unrecognized command '%s'\n", inputBuffer->Buffer);
            continue;
        }

        ExecuteStatement(&statement);
        printf("Executed.\n");
    }
    return 0;
}