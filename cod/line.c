#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../bib/line.h"

// Estrutura abstraindo cada linha linha do arquivo
struct line {
    char *data; // linha do arquivo propriamente completa 100%
    int *commas; // posições onde têm vírgula
    int pos; // auxiliar para ordenação
};


static int* pos_commas(char *line) {
    size_t i = 0, j = 0;
    int step = 30;
    int n = step;
    int *more, *commas = (int*) malloc(n*sizeof(int));

    char *pch = strchr(line, ',');
    while(1) {
        if(!pch) break;
        if(i == step) {
            more = (int*) realloc(commas, (n += step)*sizeof(int));
            if(!more) exit(1); // erros de realloc
            else commas = more;
        }
        for(i = 0; i < step && pch; i++, j++) {
            commas[j] = pch-line;
            pch = strchr(pch+1,',');
        }
    }
    return commas;
}

Line* Line_create(char *data) {
    if(!data) return NULL;

    Line *line = (Line*) malloc(sizeof(Line));
    line->data = data;
    line->commas = pos_commas(data);
    return line;
}

void Line_del(Line *line) {
    if(!line) return;

    free(line->commas);
    free(line);
}

char* Line_getdata(Line *line) {
    return line->data;
}

int* Line_getcommas(Line *line) {
    return line->commas;
}

void Line_setpos(Line *line, int pos) {
    line->pos = pos;
}

int Line_getpos(Line *line) {
    return line->pos;
}

int Line_less(Line *a, Line *b, int *idexes) {
    if(!b) return 1; // maior de todos é NULL
    if(!a) return 0;

    if(!b->data) return 1;
    if(!a->data) return 0;

    return(strcmp(a->data, b->data) < 0);
}

int Line_greater(Line *a, Line *b, int *idexes) {
    if(!b) return 0; // maior de todos é NULL
    if(!a) return 1;

    if(!b->data) return 0;
    if(!a->data) return 1;

    return(strcmp(a->data, b->data) < 0);
}
