#ifndef COMPILER
#define COMPILER
#include <stdio.h>
#include <stdint.h>

// Struct and const sizes definitions
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)
#define TABLE_MAX_PAGES 100

// Enumeration structs
typedef enum {
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum { 
  PREPARE_SUCCESS, 
  PREPARE_UNRECOGNIZED_STATEMENT,
  PREPARE_SYNTAX_ERROR 
} PrepareResult;

typedef enum { 
  STATEMENT_INSERT, 
  STATEMENT_SELECT
} StatementType;

// Data structs 
typedef struct {
  char* buffer;
  size_t buffer_length;
  size_t input_length;
} InputBuffer;

typedef struct {
  uint32_t id;
  char username[COLUMN_USERNAME_SIZE];
  char email[COLUMN_EMAIL_SIZE];
} Row;

typedef struct { 
  StatementType type; 
  Row row_to_insert;
} Statement;

typedef struct {
  uint32_t num_rows;
  void* pages[TABLE_MAX_PAGES];
} Table;

// Read user input functions
InputBuffer* CreateBuffer();
void CloseBuffer(InputBuffer* input_buffer);
void ReadInput(InputBuffer* input_buffer);
void CloseInputBuffer(InputBuffer* input_buffer);

// Interpret user input functions
MetaCommandResult ExecuteMetaCommand(InputBuffer* input_buffer);
PrepareResult PrepareStatement(InputBuffer* input_buffer,Statement* statement);
void ExecuteStatement(Statement* statement);
void PrintPrompt();

// Handle row data functions
void SerializeRow(Row* source, void* destination);
void DeserializeRow(void* source, Row* destination);

// Handle column data functions
Table* CreateTable();
void DeleteTable(Table* table);

#endif