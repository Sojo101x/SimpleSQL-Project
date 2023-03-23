/*execute.c*/

//
// Project: Execution of queries for SimpleSQL
//
// Soji Oduneye
// Northwestern University
// CS 211, Winter 2023
//

#include <assert.h> // assert
#include <string.h> // strcpy, strcat

#include <stdbool.h> // true, false
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
//
// #include any other system <.h> files?
//

#include "analyzer.h"
#include "ast.h"
#include "database.h"
#include "execute.h"
#include "parser.h"
#include "resultset.h"
#include "scanner.h"
#include "tokenqueue.h"
#include "util.h"
//
// #include any other of our ".h" files?
//
//
// implementation of function(s), both private and public
// Execute query UPDATED:
// Modified to
void six_condensed(struct ResultSet *rs, struct QUERY *que) {}

void execute_query(struct Database *db, struct QUERY *query) {

  if (db == NULL)
    panic("db is NULL (execute)");
  if (query == NULL)
    panic("query is NULL (execute)");

  if (query->queryType != SELECT_QUERY) {
    printf("**INTERNAL ERROR: execute() only supports SELECT queries.\n");
    return;
  }

  struct SELECT *select = query->q.select; // alias for less typing:

  //
  // the query has been analyzed and so we know it's correct: the
  // database exists, the table(s) exist, the column(s) exist, etc.
  //

  //
  // (1) we need a pointer to the table meta data, so find it:
  //
  struct TableMeta *tablemeta = NULL;
  struct ResultSet *resultset;
  resultset = resultset_create();

  int i = 0;
  int store;
  for (int t = 0; t < db->numTables; t++) {

    if (icmpStrings(db->tables[t].name, select->table) == 0) // found it:
    {
      tablemeta = &db->tables[t];
      break;
    }
  }

  assert(tablemeta != NULL);
  for (; i < tablemeta->numColumns; i++) {

    store = resultset_insertColumn(resultset, i + 1, tablemeta->name,
                                   tablemeta->columns[i].name, NO_FUNCTION,
                                   tablemeta->columns[i].colType);
    assert(store = i + 1);
  }
  //
  // (2) open the table's data file
  //
  // the table exists within a sub-directory under the executable
  // where the directory has the same name as the database, and with
  // a "TABLE-NAME.data" filename within that sub-directory:
  //
  char path[(2 * DATABASE_MAX_ID_LENGTH) + 10];

  strcpy(path, db->name); // name/name.data
  strcat(path, "/");
  strcat(path, tablemeta->name);
  strcat(path, ".data");

  FILE *datafile = fopen(path, "r");
  if (datafile == NULL) // unable to open:
  {
    printf("**INTERNAL ERROR: table's data file '%s' not found.\n", path);
    panic("execution halted");
  }

  //
  // (3) allocate a buffer for input, and start reading the data:
  //
  int dataBufferSize =
      tablemeta->recordSize + 3; // ends with $\n + null terminator
  char *dataBuffer = (char *)malloc(sizeof(char) * dataBufferSize);
  if (dataBuffer == NULL)
    panic("Out of memory");

  // Breaks once at the end of file
  while (true) {
    fgets(dataBuffer, dataBufferSize, datafile);
    if (feof(datafile)) {
      break;
    }
    // adds rows while updating null terminator
    // NOTICE: Takes strings and removed quotes, (either ' or ")
    int rows = resultset_addRow(resultset);
    char *start = dataBuffer;
    char *cp;

    for (int i = 0; i < tablemeta->numColumns; i++) {
      if (tablemeta->columns[i].colType == COL_TYPE_INT) {

        cp = strchr(start, ' ');
        *cp = '\0';
        resultset_putInt(resultset, rows, i + 1, atoi(start));
        start = cp + 1;
      } else if (tablemeta->columns[i].colType == COL_TYPE_STRING) {
        char quote = *start;
        start++;
        cp = start;
        while (*cp != quote) {
          cp++;
        }
        *cp = '\0';
        resultset_putString(resultset, rows, i + 1, start);
        start = cp + 2;

      } else {
        cp = strchr(start, ' ');
        *cp = '\0';
        resultset_putReal(resultset, rows, i + 1, atof(start));
        start = (cp + 1);
      }
    }
  }

  // Frees databuffer used
  free(dataBuffer);
  fclose(datafile);

  int holder;
  int index = 0;

  struct ColumnMeta foundcolumn;
  if (select->where == NULL) {

  }
  // START OF LINE 6
  else {

    for (int i = resultset->numRows; i > 0; i--)
    // iterating backwards through resulset rows
    {
      // holds index the column w the matching table name and select column name

      holder = resultset_findColumn(resultset, 1, tablemeta->name,
                                    select->where->expr->column->name);
      double diff;
      // checks for colType
      if (tablemeta->columns[holder - 1].colType == COL_TYPE_INT) {
        int colvalue = resultset_getInt(resultset, i, holder);
        int value = atoi(select->where->expr->value);
        diff = colvalue - value;
      } else if (tablemeta->columns[holder - 1].colType == COL_TYPE_REAL) {
        double colvalue = resultset_getReal(resultset, i, holder);
        double value = atof(select->where->expr->value);
        diff = colvalue - value;
      } else if (tablemeta->columns[holder - 1].colType == COL_TYPE_STRING) {
        char *colvalue = resultset_getString(resultset, i, holder);
        char *value = select->where->expr->value;
        diff = strcmp(colvalue, value);
        free(colvalue);
      }
      // Checks for expressions in WHERE

      if (select->where->expr->operator== EXPR_LT && diff >= 0) {
        resultset_deleteRow(resultset, i);
      }
      if (select->where->expr->operator== EXPR_LTE && diff> 0) {
        resultset_deleteRow(resultset, i);
      }
      if (select->where->expr->operator== EXPR_GT && diff <= 0) {
        resultset_deleteRow(resultset, i);
      }
      if (select->where->expr->operator== EXPR_GTE && diff<0) {
        resultset_deleteRow(resultset, i);
      }
      if (select->where->expr->operator== EXPR_EQUAL && diff != 0) {
        resultset_deleteRow(resultset, i);
      }
      if (select->where->expr->operator== EXPR_NOT_EQUAL && diff == 0) {
        resultset_deleteRow(resultset, i);
      }

      // ^ end of for-loop
    }
  }
  // NEW WHILE LOOP FOR # 7
  for (int i = tablemeta->numColumns; i > 0; i--) {
    struct COLUMN *col;
    col = select->columns;
    bool found = false;
    while (col != NULL) {
      int store;
      store = strcasecmp(col->name, tablemeta->columns[i - 1].name);
      if (store == 0) {
        found = true;
      }
      col = col->next;
    }
    if (found == false) {
      resultset_deleteColumn(resultset, i);
    }
  }
  // NEW LOOP FOR 8
  struct COLUMN *col = select->columns;
  int pos = 1;
  while (col != NULL) {
    // increments through column position
    int col_pos;
    col_pos = resultset_findColumn(resultset, 1, select->table, col->name);
    resultset_moveColumn(resultset, col_pos, pos);
    pos++;
    col = col->next;
  }
  col = select->columns;
  while (col != NULL) {
    int stored;
    // stores specifed column pos
    // Checks for specific Function and implements resultset's version of said
    // function
    stored = resultset_findColumn(resultset, 1, select->table, col->name);
    if (col->function != NO_FUNCTION) {
      if (col->function == MIN_FUNCTION) {
        resultset_applyFunction(resultset, MIN_FUNCTION, stored);
      }
      if (col->function == MAX_FUNCTION) {
        resultset_applyFunction(resultset, MAX_FUNCTION, stored);
      }
      if (col->function == AVG_FUNCTION) {
        resultset_applyFunction(resultset, AVG_FUNCTION, stored);
      }
      if (col->function == SUM_FUNCTION) {
        resultset_applyFunction(resultset, SUM_FUNCTION, stored);
      }
      if (col->function == COUNT_FUNCTION) {
        resultset_applyFunction(resultset, COUNT_FUNCTION, stored);
      }
    }
    col = col->next;
  }
  col = select->columns;

  //Checks for an inputted "limit" and caps the number
 // of inputs to the specified amount
  struct LIMIT *lim;
  if (select->limit != NULL) {
    for (int i = resultset->numRows; i > 0; i--) {
      if (i > select->limit->N)
        resultset_deleteRow(resultset, i);
    }
  }
  resultset_print(resultset);
  //Lastly, frees all of the memory allocated through resultset
  resultset_destroy(resultset);
  //
  // done!
  //
}
