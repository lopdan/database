#ifndef COMPILER
#define COMPILER
#include <stdio.h>

typedef struct {
  char* buffer;
  size_t buffer_length;
  size_t input_length;
} InputBuffer;

typedef enum {
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum { 
    PREPARE_SUCCESS, 
    PREPARE_UNRECOGNIZED_STATEMENT 
} PrepareResult;

typedef enum { 
    STATEMENT_INSERT, 
    STATEMENT_SELECT
} StatementType;

typedef struct { StatementType type; } Statement;

InputBuffer* CreateBuffer();
void CloseBuffer(InputBuffer* input_buffer);
MetaCommandResult ExecuteMetaCommand(InputBuffer* input_buffer);
PrepareResult PrepareStatement(InputBuffer* input_buffer,Statement* statement);
void ExecuteStatement(Statement* statement);
void PrintPrompt();
void ReadInput(InputBuffer* input_buffer);
void CloseInputBuffer(InputBuffer* input_buffer);

#endif