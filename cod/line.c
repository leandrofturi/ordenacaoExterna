#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../bib/line.h"

#define MAX(A, B) A > B ? A : B

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
    commas[j] = strlen(line);
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
        n = MAX(a->commas[idexes1[i]+1] - a->commas[idexes1[i]] - 1, 
                b->commas[idexes2[i]+1] - b->commas[idexes2[i]] - 1);
        // printf("\n(%lu) %d\n%s%s", i, n, a->data + a->commas[idexes1[i]], b->data + b->commas[idexes2[i]]);
        r = strncmp(a->data + a->commas[idexes1[i]], 
                    b->data + b->commas[idexes2[i]], n);
        if(r != 0) break;
    }
    // printf("=%d\n\n", r);
    return r;
}

char* Line_formated(Line *a, Line *b, int *idexes1, int *idexes2) {
    int i = 0, n, n1, n2;
    n = strlen(a->data) + strlen(b->data) + 1;
    char *lineF = (char*) malloc(n*sizeof(char));

    // Em comum
    n = a->commas[idexes1[0]+1] - a->commas[idexes1[0]] - 1;
    strncpy(lineF, a->data + a->commas[idexes1[0]], n);
    lineF[n] = '\0';
    for(i = 1; idexes1[i] >= 0; i++) {
        n = a->commas[idexes1[i]+1] - a->commas[idexes1[i]] - 1;
        strcat(lineF, ",");
        strncat(lineF, a->data + a->commas[idexes1[i]], n);
    }

    // Apenas de a
    n1 = strcount(a->data, ',') + 1;
    int idx1[n1];
    for(i = 0; i < n1; i++)
        idx1[i] = i;

    n2 = strcount(b->data, ',') + 1;
    int idx2[n2];
    for(i = 0; i < n2; i++)
        idx2[i] = i;

    for(i = 0; idexes1[i] >= 0; i++)
        idx1[idexes1[i]] = idx2[idexes2[i]] = -1;

    for(i = 0; i < n1; i++) {
        if(idx1[i] >= 0) {
            n = a->commas[idx1[i]+1] - a->commas[idx1[i]] - 1;
            strcat(lineF, ",");
            strncat(lineF, a->data + a->commas[idx1[i]], n);
        }
    }

    // Apenas de b
    for(i = 0; i < n2; i++) {
        if(idx2[i] >= 0) {
            n = b->commas[idx2[i]+1] - b->commas[idx2[i]] - 1;
            strcat(lineF, ",");
            strncat(lineF, b->data + b->commas[idx2[i]], n);
        }
    }
    strcat(lineF, "\n");
    return lineF;
}