#pragma once
#include <stdlib.h>
#include <stdio.h>


// Arquivos auxiliares
char** make_names(int);
void free_names(char**, int);

void fopen_block(FILE**, int, int, char**, char*);
void fclose_block(FILE**, int, int);

// Balanced Multiway Merge
void BMM(int, int, char*, int);