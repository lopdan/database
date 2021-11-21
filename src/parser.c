#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h> 

#include "../include/tokenizer.h"
#include "../include/parser.h"


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

/** Command Prompt read row */
void PrintRow(Row* row) { printf("(%d, %s, %s)\n", row->id, row->username, row->email); }

/** Handle non-sql commands. */
MetaCommandResult ExecuteMetaCommand(InputBuffer* input_buffer, Table* table) {
  if (strcmp(input_buffer->buffer, ".exit") == 0) {
    //CloseBuffer(input_buffer);
    CloseDatabase(table);
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


/** 
 * Tokenize SQL queries. 
 * Returns the type of statement that was typed.
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
  Cursor* cursor = TableStartCursor(table);
  while (!(cursor->end_of_table)) {
    // Inserts block data into row structure
    DeserializeRow(CursorValue(cursor), &row);
    PrintRow(&row);
    AdvanceCursor(cursor);
  }
  free(cursor);
  return EXECUTE_SUCCESS;
}

/** Inserts row into table */
ExecuteResult ExecuteInsert(Statement* statement, Table* table) {
  // In case the table is full return an error
  if (table->num_rows >= TABLE_MAX_ROWS) {
    return EXECUTE_TABLE_FULL;
  }
  Row* row_to_insert = &(statement->row_to_insert);
  Cursor* cursor = TableEndCursor(table);
  SerializeRow(row_to_insert, CursorValue(cursor));
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

/** Gets the page address from the file */
void* GetPage(Pager* pager, uint32_t page_num) {
  if (page_num > TABLE_MAX_PAGES) {
    printf("Tried to fetch page number out of bounds. %d > %d\n", page_num,
    TABLE_MAX_PAGES);
    exit(EXIT_FAILURE);
  }
  if (pager->pages[page_num] == NULL) {
    // Cache miss. Allocate memory and load from file.
    void* page = malloc(PAGE_SIZE);
    uint32_t num_pages = pager->file_length / PAGE_SIZE;
    // We might save a partial page at the end of the file
    if (pager->file_length % PAGE_SIZE) {
      num_pages += 1;
    }

    if (page_num <= num_pages) {
      lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
      ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
      if (bytes_read == -1) {
        printf("Error reading file: %d\n", errno);
        exit(EXIT_FAILURE);
      }
    }
    pager->pages[page_num] = page;
  }
  return pager->pages[page_num];
}

/** Store row in table */
void* CursorValue(Cursor* cursor) {
  uint32_t row_num = cursor->row_num;
  uint32_t page_num = row_num / ROWS_PER_PAGE;
  void *page = GetPage(cursor->table->pager, page_num);
  uint32_t row_offset = row_num % ROWS_PER_PAGE;
  uint32_t byte_offset = row_offset * ROW_SIZE;
  return page + byte_offset;
}

/** Open a database file */
Pager* OpenPager(const char* filename) {
  /**
   * O_RDWR   -> Read/Write mode
   * O_CREAT  -> Create file if it does not exist
   * S_IWUSR  -> User write permission
   * S_IRUSR  -> User read permission
   */
  int fd = open(filename, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR );
  if (fd == -1) {
    printf("Unable to open file.\n");
    exit(EXIT_FAILURE);
  }
  off_t file_length = lseek(fd, 0, SEEK_END);
  Pager* pager = malloc(sizeof(Pager));
  pager->file_descriptor = fd;
  pager->file_length = file_length;
  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
    pager->pages[i] = NULL;
  }
  return pager;
}

/** 
 * Open connection to database (file), initializing
 * the pager and loading all existing data.
 */
Table* OpenDatabase(const char* filename) {
  Pager* pager = OpenPager(filename);
  uint32_t num_rows = pager->file_length / ROW_SIZE;
  Table* table = malloc(sizeof(Table));
  table->pager = pager;
  table->num_rows = num_rows;

  return table;
}

/** Sync the temporary state with the permanent state of the data on disk */
void FlushPager(Pager* pager, uint32_t page_num, uint32_t size) {
  if (pager->pages[page_num] == NULL) {
    printf("Tried to flush null page.\n");
    exit(EXIT_FAILURE);
  }
  off_t offset = lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
  if (offset == -1) {
    printf("Error seeking: %d\n", errno);
    exit(EXIT_FAILURE);
  }
  ssize_t bytes_written = write(pager->file_descriptor, pager->pages[page_num], size);
  if (bytes_written == -1) {
    printf("Error writing: %d\n", errno);
    exit(EXIT_FAILURE);
  }
}

/** Release the table from memory */
void CloseDatabase(Table* table) {
  Pager* pager = table->pager;
  uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;

  for (uint32_t i = 0; i < num_full_pages; i++) {
    if (pager->pages[i] == NULL) {
      continue;
    }
    FlushPager(pager, i, PAGE_SIZE);
    free(pager->pages[i]);
    pager->pages[i] = NULL;
  }

  // There may be a partial page to write to the end of the file
  // This should not be needed after we switch to a B-tree
  uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;
  if (num_additional_rows > 0) {
    uint32_t page_num = num_full_pages;
    if (pager->pages[page_num] != NULL) {
      FlushPager(pager, page_num, num_additional_rows * ROW_SIZE);
      free(pager->pages[page_num]);
      pager->pages[page_num] = NULL;
    }
  }

  int result = close(pager->file_descriptor);
  if (result == -1) {
    printf("Error closing database file.\n");
    exit(EXIT_FAILURE);
  }
  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
    void* page = pager->pages[i];
    if (page) {
      free(page);
      pager->pages[i] = NULL;
    }
  }
  free(pager);  
  free(table);
}

/** Cursor at the beginning of a table */
Cursor* TableStartCursor(Table* table) {
  Cursor* cursor = malloc(sizeof(Cursor));
  cursor->table = table;
  cursor->row_num = 0;
  cursor->end_of_table = (table->num_rows == 0);
  return cursor;
}

/** Cursor at the end of a table */
Cursor* TableEndCursor(Table* table) {
  Cursor* cursor = malloc(sizeof(Cursor));
  cursor->table = table;
  cursor->row_num = table->num_rows;
  cursor->end_of_table = true;
  return cursor;
}

/** Advance the cursor through the table */
void AdvanceCursor(Cursor* cursor) {
  cursor->row_num += 1;
  if (cursor->row_num >= cursor->table->num_rows) {
    cursor->end_of_table = true;
  }
}