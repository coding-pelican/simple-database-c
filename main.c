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
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL
} EExecuteResult;

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} EMetaCommandResult;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_SYNTAX_ERROR,
    PREPARE_UNRECOGNIZED_STATEMENT
} EPrepareResult;

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} EStatementType;

typedef struct {
    uint32_t ID;
    char Username[COLUMN_USERNAME_SIZE + 1];
    char Email[COLUMN_EMAIL_SIZE + 1];
} Row;

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

typedef struct {
    uint32_t NumRows;
    void* Pages[TABLE_MAX_PAGES];
} Table;

void PrintRow(Row* row) {
    printf("%d, %s, %s\n", row->ID, row->Username, row->Email);
}

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

Table* NewTable() {
    Table* table = (Table*)malloc(sizeof(Table));
    table->NumRows = 0;
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        table->Pages[i] = NULL;
    }
    return table;
}

void FreeTable(Table* table) {
    for (int i = 0; table->Pages[i]; i++) {
        free(table->Pages[i]);
    }
    free(table);
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

EMetaCommandResult DoMetaCommand(InputBuffer* inputBuffer, Table* table) {
    if (strcmp(inputBuffer->Buffer, ".exit") == 0) {
        CloseInputBuffer(inputBuffer);
        FreeTable(table);
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

EExecuteResult ExecuteInsert(Statement* statement, Table* table) {
    if (table->NumRows >= TABLE_MAX_ROWS) {
        return  EXECUTE_TABLE_FULL;
    }
    Row* rowToInsert = &(statement->RowToInsert);
    SerializeRow(rowToInsert, RowSlot(table, table->NumRows));
    table->NumRows += 1;
    return EXECUTE_SUCCESS;
}

EExecuteResult ExecuteSelect(Statement* statement, Table* table) {
    Row row;
    for (uint32_t i = 0; i < table->NumRows; i++) {
        DeserializeRow(RowSlot(table, i), &row);
        PrintRow(&row);
    }
    return EXECUTE_SUCCESS;
}

EExecuteResult ExecuteStatement(Statement* statement, Table* table) {
    switch (statement->Type) {
    case (STATEMENT_INSERT):
        // printf("This is where we would do an insert.\n");
        return ExecuteInsert(statement, table);
    case (STATEMENT_SELECT):
        // printf("This is where we would do a select.\n");
        return ExecuteSelect(statement, table);
    }
}

int main(int argc, char const* argv[]) {
    Table* table = NewTable();
    InputBuffer* inputBuffer = NewInputBuffer();
    while (true) {
        PrintPrompt();
        ReadLine(inputBuffer);

        if (inputBuffer->Buffer[0] == '.') {
            switch (DoMetaCommand(inputBuffer, table)) {
            case (META_COMMAND_SUCCESS):
                continue;
            case (META_COMMAND_UNRECOGNIZED_COMMAND):
                printf("Unrecognized command '%s'.\n", inputBuffer->Buffer);
                continue;
            }
        }

        Statement statement;
        switch (PrepareStatement(inputBuffer, &statement)) {
        case (PREPARE_SUCCESS):
            break;
        case (PREPARE_SYNTAX_ERROR):
            printf("Syntax error. Could not parse statement.\n");
            continue;
        case (PREPARE_UNRECOGNIZED_STATEMENT):
            printf("Unrecognized keyword at start of '%s'.\n", inputBuffer->Buffer);
            continue;
        }

        switch (ExecuteStatement(&statement, table)) {
        case (EXECUTE_SUCCESS):
            printf("Executed.\n");
            break;
        case (EXECUTE_TABLE_FULL):
            printf("Error: Table full.\n");
            break;
        }
    }
    return 0;
}

// Test Case : 40422, 김광호, unieye07@gmail.com