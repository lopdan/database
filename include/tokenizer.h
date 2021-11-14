#ifndef TOKENIZER_H 
#define TOKENIZER_H

// Include section
#include <stdio.h>  /* Standard C library defining input/output tools. */

typedef struct {
  char* buffer;
  size_t buffer_length;
  ssize_t input_length;
} InputBuffer;

// Read user input functions
InputBuffer* CreateBuffer();
void CloseBuffer(InputBuffer* input_buffer);
void ReadInput(InputBuffer* input_buffer);
void CloseInputBuffer(InputBuffer* input_buffer);
void PrintPrompt();


#endif