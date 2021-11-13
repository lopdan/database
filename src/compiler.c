#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/compiler.h"

// Representation of a row
const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

// Representation of a table
const uint32_t PAGE_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

/** Creates Buffer for user input data. */
InputBuffer* CreateBuffer() {
  InputBuffer* input_buffer = malloc(sizeof(InputBuffer));
  input_buffer->buffer = NULL;
  input_buffer->buffer_length = 0;
  input_buffer->input_length = 0;

  return input_buffer;
}

/** Handle non-sql commands. */
MetaCommandResult ExecuteMetaCommand(InputBuffer* input_buffer) {
  if (strcmp(input_buffer->buffer, ".exit") == 0) {
    CloseBuffer(input_buffer);
    exit(EXIT_SUCCESS);
  } else {
    return META_COMMAND_UNRECOGNIZED_COMMAND;
  }
}

/**
 * Checks if the SQL insert command wont overflow the row limit size.
 */
PrepareResult PrepareInsert(InputBuffer* input_buffer, Statement* statement) {
  statement->type = STATEMENT_INSERT;
  // Break the input into substrings to check the size
  char* keyword = strtok(input_buffer->buffer, " ");
  char* id_string = strtok(NULL, " ");
  char* username = strtok(NULL, " ");
  char* email = strtok(NULL, " ");
  if (id_string == NULL || username == NULL || email == NULL) {
    return PREPARE_SYNTAX_ERROR;
  }
  int id = atoi(id_string);
  // Negative numbers are not allowed
  if (id < 0) {
    return PREPARE_NEGATIVE_ID;
  }
  if (strlen(username) > COLUMN_USERNAME_SIZE) {
    return PREPARE_STRING_TOO_LONG;
  }
  if (strlen(email) > COLUMN_EMAIL_SIZE) {
    return PREPARE_STRING_TOO_LONG;
  }
  statement->row_to_insert.id = id;
  strcpy(statement->row_to_insert.username, username);
  strcpy(statement->row_to_insert.email, email);
  return PREPARE_SUCCESS;
}


/** Tokenize SQL queries. 
 *  Returns the type of statement that was typed.
*/
PrepareResult PrepareStatement(InputBuffer* input_buffer, Statement* statement) {
  // SQL insert queries
  if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
    return PrepareInsert(input_buffer, statement); 
  }
  // SQL select queries
  if (strcmp(input_buffer->buffer, "select") == 0) {
    statement->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }
  return PREPARE_UNRECOGNIZED_STATEMENT;
} 

/** Read row data from table */
ExecuteResult ExecuteSelect(Statement* statement, Table* table) {
  Row row;
  for (uint32_t i = 0; i < table->num_rows; i++) {
    // Inserts block data into row structure
    DeserializeRow(StoreRow(table, i), &row);
    PrintRow(&row);
  }
  return EXECUTE_SUCCESS;
}

/** Inserts row into table */
ExecuteResult ExecuteInsert(Statement* statement, Table* table) {
  // In case the table is full return an error
  if (table->num_rows >= TABLE_MAX_ROWS) {
    return EXECUTE_TABLE_FULL;
  }
  Row* row_to_insert = &(statement->row_to_insert);
  SerializeRow(row_to_insert, StoreRow(table, table->num_rows));
  table->num_rows += 1;
  return EXECUTE_SUCCESS;
} 

/** Handle the SQL commands queries */
ExecuteResult ExecuteStatement(Statement* statement, Table* table) {
  switch (statement->type) {
    case (STATEMENT_INSERT):
      return ExecuteInsert(statement, table);
    case (STATEMENT_SELECT):
      return ExecuteSelect(statement, table);
  }
}

/** Command Prompt input line. */
void PrintPrompt() { printf("Database > "); }

/** Command Prompt read row */
void PrintRow(Row* row) { printf("(%d, %s, %s)\n", row->id, row->username, row->email); }

/** Gets the user input. */
void ReadInput(InputBuffer* input_buffer) {
  size_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);
  if (bytes_read <= 0) {
    printf("Error reading input\n");
    exit(EXIT_FAILURE);
  }
  // Ignore trailing newline
  input_buffer->input_length = bytes_read - 1;
  input_buffer->buffer[bytes_read - 1] = 0;
}

/** Frees Input buffer stored data */
void CloseBuffer(InputBuffer* input_buffer) {
  free(input_buffer->buffer);
  free(input_buffer);
}

/** Store data row in block of memory */
void SerializeRow(Row* source, void* destination) {
  memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
  memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
  memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

/** Read data row from block in memory */
void DeserializeRow(void* source, Row* destination) {
  memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
  memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
  memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

/** Store row in table */
void* StoreRow(Table* table, uint32_t row_num) {
  uint32_t page_num = row_num / ROWS_PER_PAGE;
  void *page = table->pages[page_num];
  if (page == NULL) {
    page = table->pages[page_num] = malloc(PAGE_SIZE);
  }
  uint32_t row_offset = row_num % ROWS_PER_PAGE;
  uint32_t byte_offset = row_offset * ROW_SIZE;
  return page + byte_offset;
}

/** Create a table */
Table* CreateTable() {
  Table* table = malloc(sizeof(Table));
  table->num_rows = 0;
  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
    table->pages[i] = NULL;
  }
  return table;
}

/** Release the table from memory */
void DeleteTable(Table* table) {
  for (int i = 0; table->pages[i]; i++) {
	  free(table->pages[i]);
  }
  free(table);
}