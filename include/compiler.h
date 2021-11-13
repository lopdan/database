#ifndef COMPILER
#define COMPILER

// Include section
#include <stdio.h>  /* Standard C library defining input/output tools. */
#include <stdint.h> /* Standard C library defining sets of integer types having specified widths. */

// Struct and const sizes definitions
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
#define TABLE_MAX_PAGES 100
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

/**
 * Enumeration of possible results of non-SQL command parse.
 */
typedef enum {
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

/**
 * Enumeration of possible results of SQL command parse.
 */
typedef enum { 
  PREPARE_SUCCESS, 
  PREPARE_UNRECOGNIZED_STATEMENT,
  PREPARE_SYNTAX_ERROR 
} PrepareResult;

/**
 * Enumeration of possible results of a user input command.
 */
typedef enum { 
  STATEMENT_INSERT, 
  STATEMENT_SELECT
} StatementType;

/**
 * Enumeration of possible results of a SQL command execution.
 */
typedef enum { 
  EXECUTE_SUCCESS, 
  EXECUTE_TABLE_FULL 
} ExecuteResult;

/**
 * Data structure to store the input from the user.
 */
typedef struct {
  char* buffer;
  size_t buffer_length;
  size_t input_length;
} InputBuffer;

/**
 * Data structure for a row in the table.
 */
typedef struct {
  uint32_t id;
  char username[COLUMN_USERNAME_SIZE];
  char email[COLUMN_EMAIL_SIZE];
} Row;

/**
 * Data structure for users' input statement and
 * the row associated with the type of statement.
 */
typedef struct { 
  StatementType type; 
  Row row_to_insert;
} Statement;

/**
 * Data structure representation of a table.
 */
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
ExecuteResult ExecuteSelect(Statement* statement, Table* table);
ExecuteResult ExecuteInsert(Statement* statement, Table* table);
ExecuteResult ExecuteStatement(Statement* statement, Table* table);
void PrintPrompt();
void PrintRow(Row* row); 

// Handle row data functions
void SerializeRow(Row* source, void* destination);
void DeserializeRow(void* source, Row* destination);
void* StoreRow(Table* table, uint32_t row_num);

// Handle column data functions
Table* CreateTable();
void DeleteTable(Table* table);

#endif