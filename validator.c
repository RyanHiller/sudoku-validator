/*
 * Author: Ryan Hiller
 * Assignment: Homework 3
 * Class: CS3600-001
 * Due Date: 9/17/2019 - 9/19/2019
 * Submission Date:
 *
 * A multithreaded sudoku solution validator.
 *
 * Current Issues:
 * -The output from the main function does not generate properly for unknown reasons.
 * --All validation and output from the validation functions work correctly
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_CHILD_THREADS 27

/* Thread function */
void *columnValidator (void *param);
void *rowValidator (void *param);
void *subgridValidator (void *param);

typedef int bool;
#define TRUE 1
#define FALSE 0

/* Shared data structures (1.2) */
int sudokuPuzzle[9][9];
bool columnValidity[9];
bool rowValidity[9];
bool subgridValidity[9];

/* Index structure (1.3) */
typedef struct {
int topRow;
int bottomRow;
int leftColumn;
int rightColumn;
} index_range;

int main(int argc, char *argv[]) {
  int i, j, k;
  bool x = TRUE, y = FALSE;

  FILE *sudokuFile;
  char fileLine[25];
  index_range colParams[9]; /* Index parameters for each column */
  index_range rowParams[9]; /* Index parameters for each row */
  index_range subgridParams[9]; /* Index parameters for each subgrid */
  pthread_t tid_column[9]; /* Thread identifier for column child threads */
  pthread_t tid_row[9]; /* Thread identifier for row child threads */
  pthread_t tid_subgrid[9]; /* Thread identifier for subgrid child threads */
  pthread_attr_t colAttr[9];
  pthread_attr_t rowAttr[9];
  pthread_attr_t subgridAttr[9];

  /* Read sudoku solution file */
  sudokuFile = fopen("puzzle1.txt", "r"); /* CHANGE THIS LINE TO VALIDATE A DIFFERENT SUDOKU */

  if (sudokuFile == NULL) {
    printf( "Could not open file puzzle1.txt");
    return 1;
  }

  i = 0;

  /* Read each line from the file and store in sudokuPuzzle[][] */
  while (fgets(fileLine, 25, sudokuFile) != NULL) {
    for (j = 0; j < 17; j = j + 2) {
      sudokuPuzzle[i][j / 2] = fileLine[j] - '0'; /* Subtracting '0' converts from char to int */
    }

    i++;
  }

  fclose(sudokuFile);


  /* Initialize structure arrays (2.1, 2.2)*/
  for (i = 0; i < 9; i++) {
    colParams[i].topRow = 0;
    colParams[i].bottomRow = 8;
    colParams[i].leftColumn = i;
    colParams[i].rightColumn = i;

    rowParams[i].topRow = i;
    rowParams[i].bottomRow = i;
    rowParams[i].leftColumn = 0;
    rowParams[i].rightColumn = 8;
  }

  /* Initialize structure array for subgrids (2.3) */
  i = 0;
  for (j = 0; j < 3; j++) {
    for(k = 0; k < 3; k++) {
      subgridParams[i].topRow = j * 3;
      subgridParams[i].bottomRow = subgridParams[i].topRow + 2;
      subgridParams[i].leftColumn = k * 3;
      subgridParams[i].rightColumn = subgridParams[i].leftColumn + 2;
      i++;
    }
  }

  /* Get default attributes */
  for (i = 0; i < 9; i++) {
    pthread_attr_init(&(colAttr[i]));
    pthread_attr_init(&(rowAttr[i]));
    pthread_attr_init(&(subgridAttr[i]));
  }

  /* Create threads for each subgroup */
  for (i = 0; i < 9; i++) {
    pthread_create(&(tid_column[i]), &(colAttr[i]), columnValidator, &(colParams[i]));
    pthread_create(&(tid_row[i]), &(rowAttr[i]), rowValidator, &(rowParams[i]));
    pthread_create(&(tid_subgrid[i]), &(subgridAttr[i]), subgridValidator, &(subgridParams[i]));
  }

  /* Wait for threads to exit */
  for (i = 0; i < 9; i++) {
    pthread_join(tid_column[i], NULL);
    pthread_join(tid_row[i], NULL);
    pthread_join(tid_subgrid[i], NULL);
  }

  /* Output and formatting */
  char *validity = "valid";
  char *sudokuValid = "valid";

  for (i = 0; i < 9; i++) {
    if (columnValidity[i] != TRUE) {
      validity = "invalid";
      sudokuValid = "invalid";
    }

    printf("\nColumn: %lx %s", (unsigned long)tid_column[i], validity);
    validity = "valid";
  }

  for (i = 0; i < 9; i++) {
    if (rowValidity[i] != TRUE) {
      validity = "invalid";
      sudokuValid = "invalid";
    }

    printf("\nRow: %lx %s", (unsigned long)tid_row[i], validity);
    validity = "valid";
  }

  for (i = 0; i < 9; i++) {
    if (subgridValidity[i] != TRUE) {
      validity = "invalid";
      sudokuValid = "invalid";
    }

    printf("\nSubgrid: %lx %s", (unsigned long)tid_subgrid[i], validity);
    validity = "valid";
  }

  printf("\nSudoku Puzzle: %s\n", sudokuValid);
}

/* Column validation function */
void *columnValidator (void *param) {
  index_range *params = (index_range*) param;
  int col = params->leftColumn;
  int topRow = params->topRow;
  int bottomRow= params->bottomRow;

  pthread_t self = pthread_self();
  int isValid[9];
  int i, num;

  /* Check that each number only appears once */
  for (i = 0; i < 9; i++) {
    num = sudokuPuzzle[i][col];
    if (isValid[num - 1] == 1) {
      columnValidity[i] = FALSE;
      printf("\n%lx TRow: %d, BRow: %d, LCol: %d, RCol: %d invalid!",(unsigned long)self, topRow, bottomRow, col, col);
      pthread_exit(NULL);
    } else {
      columnValidity[i] = TRUE;
      isValid[num - 1] = 1;
    }
  }

  printf("\n%lx TRow: %d, BRow: %d, LCol: %d, RCol: %d valid!",(unsigned long)self, topRow, bottomRow, col, col);

  pthread_exit(NULL);
}

/* Row validation function */
void *rowValidator (void *param) {
  index_range *params = (index_range*) param;
  int leftColumn = params->leftColumn;
  int rightColumn = params->rightColumn;
  int row = params->topRow;

  pthread_t self = pthread_self();
  int isValid[9];
  int i, num;

  for (i = 0; i < 9; i++) {
    num = sudokuPuzzle[row][i];
    printf("\n\n%d, %d", num, isValid[num]);
    if (isValid[num - 1] == 1) {
      rowValidity[i] = FALSE;
      printf("\n%lx TRow: %d, BRow: %d, LCol: %d, RCol: %d invalid!",(unsigned long)self, row, row, leftColumn, rightColumn);
      pthread_exit(NULL);
    } else {
      rowValidity[i] = TRUE;
      isValid[num - 1] = 1;
    }
  }

  printf("\n%lx TRow: %d, BRow: %d, LCol: %d, RCol: %d valid!",(unsigned long)self, row, row, leftColumn, rightColumn);

  pthread_exit(NULL);
}

/* Subgrid validation function */
void *subgridValidator (void *param) {
  index_range *params = (index_range*) param;
  int leftColumn = params->leftColumn;
  int rightColumn = params->rightColumn;
  int topRow = params->topRow;
  int bottomRow= params->bottomRow;

  pthread_t self = pthread_self();
  int isValid[9];
  int i, j, num;

  for (i = topRow; i < topRow + 3; i++) {
    for (j = leftColumn; j < leftColumn + 3; j++) {
      num = sudokuPuzzle[i][j];
      if (isValid[num - 1] == 1) {
        subgridValidity[i] = FALSE;
        printf("\n%lx TRow: %d, BRow: %d, LCol: %d, RCol: %d invalid!", (unsigned long)self, topRow, bottomRow, leftColumn, rightColumn);
        pthread_exit(NULL);
      } else {
        subgridValidity[i] = TRUE;
        isValid[num - 1] = 1;
      }
    }
  }

  printf("\n%lx TRow: %d, BRow: %d, LCol: %d, RCol: %d valid!", (unsigned long)self, topRow, bottomRow, leftColumn, rightColumn);

  pthread_exit(NULL);
}
