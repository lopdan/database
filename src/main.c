#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/compiler.h"

int main(int argc, char* argv[]) {
  InputBuffer* input_buffer = CreateBuffer();
  while (true) {
    PrintPrompt();
    ReadInput(input_buffer);
    // Non SQL statements.
    if (input_buffer->buffer[0] == '.') {
      switch (ExecuteMetaCommand(input_buffer)) 
      {
        case (META_COMMAND_SUCCESS):
          continue;
        case (META_COMMAND_UNRECOGNIZED_COMMAND):
          printf("Unrecognized command '%s'\n", input_buffer->buffer);
          continue;
      }
    }
    // SQL statements.
    Statement statement;
    switch (PrepareStatement(input_buffer, &statement)) {
      case (PREPARE_SUCCESS):
        break;
      case (PREPARE_UNRECOGNIZED_STATEMENT):
        printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
        continue;
    }
    ExecuteStatement(&statement);
    printf("Executed.\n");
  }
}