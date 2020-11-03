#pragma once
#include <stdlib.h>
#include <stdio.h>

#define CUTOFF 16

struct tMem {
    char **data;
    size_t sizemax;
    size_t nrows;
};
typedef struct tMem tMem;


// Names of auxiliar files
char** make_names(int);
void free_names(char**, int);

// Open and close blocks of files
void fopen_block(FILE**, int, int, char**, char*);
void fclose_block(FILE**, int, int);

// Load max M lines into memory
void load_memory(FILE*, tMem*);
void write_memory(tMem*, FILE*);
void sort_memory(tMem*, int*);

// Balanced Multiway Merge
void BMM(int, int, char*, int*);