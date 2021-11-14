#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/tokenizer.h"

/** Creates Buffer for user input data. */
InputBuffer* CreateBuffer() {
  InputBuffer* input_buffer = malloc(sizeof(InputBuffer));
  input_buffer->buffer = NULL;
  input_buffer->buffer_length = 0;
  input_buffer->input_length = 0;

  return input_buffer;
}

/** Command Prompt input line. */
void PrintPrompt() { printf("Database > "); }

/** Gets the user input. */
void ReadInput(InputBuffer* input_buffer) {
  size_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);
  if (bytes_read <= 0) {
    printf("Error reading input.\n");
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