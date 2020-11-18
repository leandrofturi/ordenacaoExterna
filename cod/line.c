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
    int size;
};

int vectorSize(char *line) {
    int size = 1;
    for(int i=0; i<strlen(line); i++) {
        if(line[i]== ',') {
            size++;
        }
    }
    return size;
}


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
    line->size = vectorSize(data);
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

int Line_less_merge(Line *a, Line *b, int *idexes1, int *idexes2) {
    return(Line_cmp(a, b, idexes1, idexes2));
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
        int qtd_car1 = idexes1[i] == 0 ? a->commas[idexes1[i]+1] - a->commas[idexes1[i]] : a->commas[idexes1[i]+1] - (a->commas[idexes1[i]] + 1);
        int qtd_car2 = idexes2[i] == 0 ? b->commas[idexes2[i]+1] - b->commas[idexes2[i]] : b->commas[idexes2[i]+1] - (b->commas[idexes2[i]] + 1);
        n = MAX( qtd_car1, qtd_car2);
        char *string1 = idexes1[i] == 0 ? a->data + a->commas[idexes1[i]] : a->data + a->commas[idexes1[i]] + 1 ;
        char *string2 = idexes2[i] == 0 ? b->data + b->commas[idexes2[i]] : b->data + b->commas[idexes2[i]] + 1 ;
        r = strncmp(string1,
                    string2, n);
//         printf("%s\n%s \nr: %d \n\n", string1, string2, r);
        if(r != 0) break;
    }
    return r;
}

//int Line_cmp(Line *a, Line *b, int *idexes1, int *idexes2) {
//    if(!b && !a) return 0;
//    if(!b) return 1; // maior de todos é NULL
//    if(!a) return -1;
//
//    if(!b->data && !a->data) return 0;
//    if(!b->data) return 1;
//    if(!a->data) return -1;
//
//    int n, r = 0;
//    for(size_t i = 0; idexes1[i] >= 0 && idexes2[i] >= 0; i++) {
//        int qtd_car1 = idexes1[i] == 0 ? a->commas[idexes1[i]+1] - a->commas[idexes1[i]] : a->commas[idexes1[i]+1] - (a->commas[idexes1[i]] + 1);
//        int qtd_car2 = idexes2[i] == 0 ? b->commas[idexes2[i]+1] - b->commas[idexes2[i]] : b->commas[idexes2[i]+1] - (b->commas[idexes2[i]] + 1);
//        n = MIN(qtd_car1,
//                qtd_car2);
//        char *string1 = a->data + a->commas[idexes1[i]];
//        char *string2 = b->data + b->commas[idexes2[i]];
//        r = strncmp(a->data + a->commas[idexes1[i]],
//                    b->data + b->commas[idexes2[i]], n);
//        printf("%d\n%s\n%s \nr: %d \n\n", n, a->data + a->commas[idexes1[i]], b->data + b->commas[idexes2[i]], r);
//        if(r != 0) break;
//    }
//    return r;
//}

void Line_print_final_file(Line *a, int *idexes1, Line *b, int *idexes2, FILE* arq) {
    int char_quantity;
    int len = strlen(a->data);
    if( a->data[len-1] == '\n' )
        a->data[len-1] = 0;
    len = strlen(b->data);
    if( b->data[len-1] == '\n' )
        b->data[len-1] = 0;
    char *stringToPrint;

    for(size_t i = 0; idexes1[i] >= 0; i++) {
        char_quantity = idexes1[i] == 0 ? a->commas[idexes1[i]+1] - a->commas[idexes1[i]] : a->commas[idexes1[i]+1] - (a->commas[idexes1[i]]);
        stringToPrint = idexes1[i] == 0 ? a->data : a->data + a->commas[idexes1[i]] + 1;
        fprintf(arq, "%.*s,", char_quantity, stringToPrint );
    }
    int k = 0;

    // Printar o final do arquivo 1
    int is_joint_column = 0;
    for(size_t i = 0; i < a->size; i++) {
        for(size_t j = 0; j < a->size; j++ ) {
            if(i == idexes1[j]) is_joint_column = 1;
        }
        if(is_joint_column == 0) {
            char_quantity = a->commas[i+1] - (a->commas[i] + 1);
            stringToPrint = a->data + a->commas[i] + 1;
            fprintf(arq, "%.*s,", char_quantity, stringToPrint );
//            printf( "%.*s,", char_quantity, stringToPrint );
        }
        is_joint_column = 0;
    }

    // Printar o final do arquivo 2
    is_joint_column = 0;
    for(size_t i = 0; i < b->size; i++) {
        for(size_t j = 0; j < b->size ; j++ ) {
            if(i == idexes2[j]) is_joint_column = 1;
        }
        if(is_joint_column == 0) {
            char_quantity = b->commas[i + 1] - (a->commas[i] + 1);
            stringToPrint = b->data + b->commas[i] + 1;
            fprintf(arq, "%.*s,", char_quantity, stringToPrint );
        }
        is_joint_column = 0;
    }
    fprintf(arq, "\n");
}
