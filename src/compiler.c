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

/** Tokenize SQL queries. 
 *  Returns the type of statement that was typed.
*/
PrepareResult PrepareStatement(InputBuffer* input_buffer,Statement* statement) {
  if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
    statement->type = STATEMENT_INSERT;
    int args_assigned = sscanf(input_buffer->buffer, "insert %s %s %s",
                               statement->row_to_insert.id,
                               statement->row_to_insert.username,
                               statement->row_to_insert.email);
    if (args_assigned < 3) {
      return PREPARE_SYNTAX_ERROR;
    }
    return PREPARE_SUCCESS;
  }
  if (strcmp(input_buffer->buffer, "select") == 0) {
    statement->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }
  return PREPARE_UNRECOGNIZED_STATEMENT;
}

/** Handle the SQL commands queries */
void ExecuteStatement(Statement* statement) {
  switch (statement->type) {
    case (STATEMENT_INSERT):
      printf("Insert Statement.\n");
      break;
    case (STATEMENT_SELECT):
      printf("Select Statement.\n");
      break;
  }
}

/** Command Prompt input line. */
void PrintPrompt() { printf("Database > "); }

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
  void* page = table->pages[page_num];
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