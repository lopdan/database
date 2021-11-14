#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/tokenizer.h"
#include "../include/parser.h"

int main(int argc, char* argv[]) {
  // Specify a database name to read or create
  if (argc < 2) {
    printf("Must supply a database filename.\n");
    exit(EXIT_FAILURE);
  }
  char* filename = argv[1];
  Table* table = OpenDatabase(filename);

  // Read input from user
  InputBuffer* input_buffer = CreateBuffer();
  while (true) {
    PrintPrompt();
    ReadInput(input_buffer);
    if (strcmp(input_buffer->buffer, ".exit") == 0){
      // Non SQL statements.
      if (input_buffer->buffer[0] == '.') {
        switch (ExecuteMetaCommand(input_buffer, table)) {
          case (META_COMMAND_SUCCESS):
            continue;
          case (META_COMMAND_UNRECOGNIZED_COMMAND):
            printf("Unrecognized command '%s'\n", input_buffer->buffer);
            continue;
        }
      } 
    }
    // SQL statements.
    Statement statement;
    switch (PrepareStatement(input_buffer, &statement)) {
      case (PREPARE_SUCCESS):
        break;
      case (PREPARE_STRING_TOO_LONG):
        printf("String is too long.\n");
        continue;
      case (PREPARE_NEGATIVE_ID):
        printf("ID must be a positive number.\n");
        continue;
      case (PREPARE_SYNTAX_ERROR):
        printf("Syntax error. Statement could not be parsed.\n");
        continue;
      case (PREPARE_UNRECOGNIZED_STATEMENT):
        printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
        continue;
    }

    // Execute the SQL query
    switch (ExecuteStatement(&statement, table)) {
      case (EXECUTE_SUCCESS):
        printf("Executed.\n");
        break;
      case (EXECUTE_TABLE_FULL):
        printf("Error: Table full.\n");
        break;
    }
  }
}