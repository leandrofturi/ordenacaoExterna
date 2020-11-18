#ifndef LINE_H
#define LINE_H

typedef struct line Line;


Line* Line_create(char *data); // data by reference

void Line_del(Line*);

char* Line_getdata(Line*);

int* Line_getcommas(Line*);

void Line_setpos(Line*, int);

int Line_getpos(Line*);

int Line_less(Line*, Line*, int*);

int Line_less_merge(Line *a, Line *b, int *idexes1, int *idexes2);

int Line_cmp(Line *a, Line *b, int *idexes1, int *idexes2);

int Line_less_merge(Line *a, Line *b, int *idexes1, int *idexes2);

int Line_greater(Line*, Line*, int*);

void Line_print_final_file(Line *a, int *idexes, Line *b, int *idexes2, FILE* arq);

#define Line_exch(a, b) { Line *t = a; a = b; b = t; }


#endif