#pragma once
#include <stdlib.h>
#include <stdio.h>


// Open and close a array of files in wr+ mode
FILE** open_files(int);
void close_files(FILE**, int);

// Balanced Multiway Merge
void BMM(int, int, char*, int);