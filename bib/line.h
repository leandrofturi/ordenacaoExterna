#ifndef LINE_H
#define LINE_H

#include "reader.h"

typedef struct line Line;


Line* Line_create(char *data); // data by reference

void Line_del(Line*);

char* Line_getdata(Line*);

int* Line_getcommas(Line*);

void Line_setpos(Line*, int);

int Line_getpos(Line*);


int Line_less(Line*, Line*, int*);

int Line_greater(Line*, Line*, int*);

int Line_cmp(Line*, Line*, int*, int*);

char* Line_formated(Line*, Line*, int*, int*);

#define Line_exch(a, b) { Line *t = a; a = b; b = t; }


#endif