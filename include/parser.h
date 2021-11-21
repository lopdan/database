#ifndef PARSER_H 
#define PARSER_H

// Include section
#include <stdio.h>  /* Standard C library defining input/output tools. */
#include <stdint.h> /* Standard C library defining sets of integer types having specified widths. */
#include "./tokenizer.h"

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
  PREPARE_SYNTAX_ERROR,
  PREPARE_STRING_TOO_LONG,
  PREPARE_NEGATIVE_ID 
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
 * Data structure for a row in the table.
 */
typedef struct {
  uint32_t id;
  char username[COLUMN_USERNAME_SIZE + 1];
  char email[COLUMN_EMAIL_SIZE + 1];
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
 * Memory block to store tables.
 */
typedef struct {
  int file_descriptor;
  uint32_t file_length;
  void* pages[TABLE_MAX_PAGES];
} Pager;

/**
 * Data structure representation of a table.
 */
typedef struct {
  Pager* pager;
  uint32_t num_rows;
} Table;

/**
 * Represents the location in the table
 */
typedef struct {
  Table* table;
  uint32_t row_num;
  bool end_of_table;
} Cursor;

// Interpret user input functions
MetaCommandResult ExecuteMetaCommand(InputBuffer* input_buffer, Table* table);
PrepareResult PrepareStatement(InputBuffer* input_buffer,Statement* statement);
ExecuteResult ExecuteSelect(Statement* statement, Table* table);
/** Checks if the SQL insert command wont overflow the row limit size.*/
PrepareResult PrepareInsert(InputBuffer* input_buffer, Statement* statement);
ExecuteResult ExecuteInsert(Statement* statement, Table* table);
ExecuteResult ExecuteStatement(Statement* statement, Table* table);

// Handle row data functions
void SerializeRow(Row* source, void* destination);
void DeserializeRow(void* source, Row* destination);
void* CursorValue(Cursor* cursor);

// Handle column data functions
void* GetPage(Pager* pager, uint32_t page_num);
Pager* OpenPager(const char* filename);
void FlushPager(Pager* pager, uint32_t page_num, uint32_t size);
Table* OpenDatabase(const char* filename);
void CloseDatabase(Table* table);
Cursor* TableEndCursor(Table* table);
Cursor* TableStartCursor(Table* table);
void AdvanceCursor(Cursor* cursor);

// Read data
void PrintRow(Row* row); 

#endif