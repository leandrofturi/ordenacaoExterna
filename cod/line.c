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
            commas[j] = pch-line;
            pch = strchr(pch+1,',');
        }
    }
    commas[j] = strlen(line)-1;
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
    
    int n, r = 0;
    for(size_t i = 0; idexes[i] >= 0; i++) {
        int qtd_car1 = a->commas[idexes[i]] == 0 ? a->commas[idexes[i]+1] - a->commas[idexes[i]] : a->commas[idexes[i]+1] - (a->commas[idexes[i]] + 1);
        int qtd_car2 = b->commas[idexes[i]] == 0 ? b->commas[idexes[i]+1] - b->commas[idexes[i]] : b->commas[idexes[i]+1] - (b->commas[idexes[i]] + 1);
        n = MIN( qtd_car1, qtd_car2);
        if(!idexes[i]) n++;
        char *string1 = a->commas[idexes[i]] == 0 ? a->data + a->commas[idexes[i]] : a->data + a->commas[idexes[i]] + 1 ;
        char *string2 = b->commas[idexes[i]] == 0 ? b->data + b->commas[idexes[i]] : b->data + b->commas[idexes[i]] + 1 ;
        r = strncmp(string1, string2, n);
        if(r != 0) break;
    }
    return(r < 0);
}

int Line_less_merge(Line *a, Line *b, int *idexes1, int *idexes2) {
    if(!b) return 1; // maior de todos é NULL
    if(!a) return 0;

    if(!b->data) return 1;
    if(!a->data) return 0;

    int n, r = 0;
    for(size_t i = 0; idexes1[i] >= 0; i++) {
        int qtd_car1 = idexes1[i] == 0 ? a->commas[idexes1[i]+1] - a->commas[idexes1[i]] : a->commas[idexes1[i]+1] - (a->commas[idexes1[i]] + 1);
        int qtd_car2 = idexes2[i] == 0 ? b->commas[idexes2[i]+1] - b->commas[idexes2[i]] : b->commas[idexes2[i]+1] - (b->commas[idexes2[i]] + 1);
        n = MIN( qtd_car1, qtd_car2);
        if(!idexes1[i]) n++;
        char *string1 = a->commas[idexes1[i]] == 0 ? a->data + a->commas[idexes1[i]] : a->data + a->commas[idexes1[i]] + 1 ;
        char *string2 = b->commas[idexes2[i]] == 0 ? b->data + b->commas[idexes2[i]] : b->data + b->commas[idexes2[i]] + 1 ;
        r = strncmp(string1, string2, n);
        if(r != 0) break;
    }
    return r;
}

int Line_greater(Line *a, Line *b, int *idexes) {
    if(!b) return 0; // maior de todos é NULL
    if(!a) return 1;

    if(!b->data) return 0;
    if(!a->data) return 1;

    int n, r = 0;
    for(size_t i = 0; idexes[i] >= 0; i++) {
        n = MIN(a->commas[idexes[i]+1] - a->commas[idexes[i]] - 1,
                b->commas[idexes[i]+1] - b->commas[idexes[i]] - 1);
        if(!idexes[i]) n++;
        r = strncmp(a->data + a->commas[idexes[i]], 
                    b->data + b->commas[idexes[i]], n);
        if(r != 0) break;
    }
    return(r > 0);
}

void Line_print_final_file(Line *a, int *idexes, Line *b, int *idexes2, FILE* arq) {
    int char_quantity;
    for(size_t i = 0; idexes[i] >= 0; i++) {
        char_quantity = a->commas[idexes[i] + 1] - a->commas[idexes[i]];
        fprintf(arq, "%.*s", char_quantity, a->data + a->commas[idexes[i]]);
    }

    // Printar o final do arquivo 1
    int total_commas = sizeof(a->commas);
    int is_joint_column = 0;
    for(size_t i = 0; i < total_commas; i++) {
        for(size_t j = 0; idexes[j] >= 0; j++ ) {
            if(i == idexes[j]) is_joint_column = 1;
        }
        if(is_joint_column == 0) {
            char_quantity = a->commas[i + 1] - a->commas[i];
            fprintf(arq, "%.*s", char_quantity, a->data + a->commas[i]);
        }
        is_joint_column = 0;
    }

    // Printar o final do arquivo 2
    total_commas = sizeof(b->commas);
    is_joint_column = 0;
    for(size_t i = 0; i < total_commas; i++) {
        for(size_t j = 0; idexes2[j] >= 0; j++ ) {
            if(i == idexes2[j]) is_joint_column = 1;
        }
        if(is_joint_column == 0) {
            char_quantity = b->commas[i + 1] - b->commas[i];
            fprintf(arq, "%.*s", char_quantity, b->data + b->commas[i]);
        }
        is_joint_column = 0;
    }
//    fprintf(arq, "\n");
}
