#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/compiler.h"

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