#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../bib/line.h"

#define MIN(A, B) A < B ? A : B

// Estrutura abstraindo cada linha linha do arquivo
struct line {
    char *data; // linha do arquivo propriamente completa 100%
    int *commas; // posições onde têm vírgula
    int pos; // auxiliar para ordenação
};


static int* pos_commas(char *line) {
    size_t i = 0, j = 1;
    int step = 30;
    int n = step+2; // first position & last position
    int *more, *commas = (int*) malloc(n*sizeof(int));

    commas[0] = 0;
    char *pch = strchr(line, ',');
    while(1) {
        if(!pch)
            break;
        if(i == step) {
            more = (int*) realloc(commas, (n += step)*sizeof(int));
            if(!more) exit(1); // erros de realloc
            else commas = more;
        }
        for(i = 0; i < step && pch; i++, j++) {
            commas[j] = pch-line+1;
            pch = strchr(pch+1,',');
        }
    }
    commas[j] = strlen(line)+1;
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
    return(Line_cmp(a, b, idexes, idexes) < 0);
}

int Line_greater(Line *a, Line *b, int *idexes) {
    return(Line_cmp(a, b, idexes, idexes) > 0);
}

int Line_cmp(Line *a, Line *b, int *idexes1, int *idexes2) {
    if(!b && !a) return 0;
    if(!b) return 1; // maior de todos é NULL
    if(!a) return -1;

    if(!b->data && !a->data) return 0;
    if(!b->data) return 1;
    if(!a->data) return -1;

    int n, r = 0;
    for(size_t i = 0; idexes1[i] >= 0 && idexes2[i] >= 0; i++) {
        n = MIN(a->commas[idexes1[i]+1] - a->commas[idexes1[i]] - 1, 
                b->commas[idexes2[i]+1] - b->commas[idexes2[i]] - 1);
        // printf("%d\n%s\n%s\n\n", n, a->data + a->commas[idexes1[i]], b->data + b->commas[idexes2[i]]);
        r = strncmp(a->data + a->commas[idexes1[i]], 
                    b->data + b->commas[idexes2[i]], n);
        if(r != 0) break;
    }
    return r;
}