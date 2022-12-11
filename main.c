#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASSERT(condition, msg)                                   \
    if (!(condition)) {                                          \
        fprintf(stderr, "%s(%s: %d)\n", msg, __FILE__, __LINE__);\
        __builtin_trap();                                        \
    }                                                            \

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

enum {
    ALLOC_SIZE = 64,
    COLUMN_USERNAME_SIZE = 32,
    COLUMN_EMAIL_SIZE = 255,
    TABLE_MAX_PAGES = 100
};

typedef struct {
    char* Buffer;
    size_t BufferLength;
    ssize_t InputLength;
} InputBuffer;

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} EMetaCommandResult;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT
} EPrepareResult;

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} EStatementType;

typedef struct {
    uint32_t ID;
    char Username[COLUMN_USERNAME_SIZE];
    char Email[COLUMN_EMAIL_SIZE];
} Row;

typedef struct {
    uint32_t NumRows;
    void* Pages[TABLE_MAX_PAGES];
} Table;

typedef struct {
    EStatementType Type;
    Row RowToInsert;
} Statement;

const uint16_t ID_SIZE = size_of_attribute(Row, ID);
const uint16_t USERNAME_SIZE = size_of_attribute(Row, Username);
const uint16_t EMAIL_SIZE = size_of_attribute(Row, Email);
const uint16_t ID_OFFSET = 0;
const uint16_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint16_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint16_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;
const uint32_t PAGE_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

void SerializeRow(Row* source, void* destination) {
    memcpy(destination + ID_OFFSET, &(source->ID), ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, &(source->Username), USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, &(source->Email), EMAIL_SIZE);
}

void DeserializeRow(void* source, Row* destination) {
    memcpy(&(destination->ID), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->Username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->Email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

void* RowSlot(Table* table, uint32_t rowNum) {
    uint32_t pageNum = rowNum / ROWS_PER_PAGE;
    void* page = table->Pages[pageNum];
    if (page == NULL) {
        // Allocate memory only when we try to access page
        page = table->Pages[pageNum] = malloc(PAGE_SIZE);
    }
    uint32_t rowOffset = rowNum % ROWS_PER_PAGE;
    uint32_t byteOffset = rowOffset * ROW_SIZE;
    return page + byteOffset;
}

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

EMetaCommandResult DoMetaCommand(InputBuffer* inputBuffer) {
    if (strcmp(inputBuffer->Buffer, ".exit") == 0) {
        exit(EXIT_SUCCESS); // NOLINT
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

EPrepareResult PrepareStatement(InputBuffer* inputBuffer, Statement* statement) {
    if (strncmp(inputBuffer->Buffer, "insert", 6) == 0) { // NOLINT
        statement->Type = STATEMENT_INSERT;
        int argsAssigned = sscanf(
            inputBuffer->Buffer, "insert %d %s %s", &(statement->RowToInsert.ID),
            statement->RowToInsert.Username, statement->RowToInsert.Email);
        if (argsAssigned < 3) {
            return PREPARE_SYNTAX_ERROR;
        }
        return PREPARE_SUCCESS;

    }
    if (strcmp(inputBuffer->Buffer, "select") == 0) {
        statement->Type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void ExecuteStatement(Statement* statement) {
    // TODO: Implement ExecuteStatement::테이블 구조 읽기/쓰기 
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