#pragma once
#include <stdlib.h>
#include <stdio.h>

#define CUTOFF 16

typedef struct Mem Mem;

// Init and del memory
Mem* Mem_init(int);
void Mem_del(Mem*);

// Names of auxiliar files
char** make_names(int);
void free_names(char**, int);

// Open and close blocks of files
void fopen_block(FILE**, int, int, char**, char*);
void fclose_block(FILE**, int, int);

// Load max M lines into memory
void load_memory(FILE*, Mem*);
void write_memory(Mem*, FILE*);
void sort_memory(Mem*, int*);

// Part of break in blocks
void break_blocks(FILE**, FILE*, Mem*, int*, int);
// Linear search during the files
int lin_search(FILE**, int*, int, int);

// Balanced Multiway Merge
// Note that the idexes with the column order need to ends with -1
void BMM(int, int, char*, int*, char*);

void merge(char*, char*, int*, int*, char*);
