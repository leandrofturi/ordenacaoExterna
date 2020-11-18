#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bib/reader.h"
#include "bib/ext_sort.h"
#include "bib/line.h"

void merge(char *fileName1, char *fileName2, char *finalFileName, int *idexes1, int *idexes2) {
    FILE *file1 = fopen(fileName1, "r");
    FILE *file2 = fopen(fileName2, "r");
    FILE *finalFile = fopen(finalFileName, "w");

    Line *lineFile1, *lineFile2;

    char *readLine1, *readLine2;

    readLine1 = read_line(file1);
    readLine2 = read_line(file2);

    lineFile1 = Line_create(readLine1);
    lineFile2 = Line_create(readLine2);
    int compare;

    while ( !feof(file1) && !feof(file2) ) {
        // Caso as linhas sejam iguais, imprime no arquivo final
        compare = Line_less_merge(lineFile1, lineFile2, idexes1, idexes2);
        if( compare == 0 ) {
            Line_print_final_file(lineFile1, idexes1, lineFile2, idexes2, finalFile);
            Line_del(lineFile1);
            Line_del(lineFile2);
            // Ler a próxima linha nos dois arquivos
            readLine1 = read_line(file1);
            readLine2 = read_line(file2);
            if(!readLine1) break;
            if(!readLine2) break;
            if(strcmp(readLine1, "#") != 0) lineFile1 = Line_create(readLine1);
            if(strcmp(readLine2, "#") != 0)  lineFile2 = Line_create(readLine2);
        }
        else if(compare < 0) {
            Line_del(lineFile1);
            readLine1 = read_line(file1);
            if(!readLine1) break;
            if(strcmp(readLine1, "#") != 0) lineFile1 = Line_create(readLine1);

        } else if(compare > 0) {
            Line_del(lineFile2);
            readLine2 = read_line(file2);
            if(!readLine2) break;
            if(strcmp(readLine2, "#") != 0) lineFile2 = Line_create(readLine2);
        }
    }
    fclose(file1);
    fclose(file2);
    fclose(finalFile);
}

int main(int argc, char *argv[]) {
    int P = atoi(argv[1]);
    int M = atoi(argv[2]);

    int k = strcount(argv[3], ',') + 1;
    int L1[k+1];

    char *tok = strtok(argv[3], ",");
    for(size_t i = 0; i < k; i++, tok = strtok(NULL, ","))
        L1[i] = atoi(tok);
    L1[k] = -1;

    int L2[k+1];
    tok = strtok(argv[4], ",");
    for(size_t i = 0; i < k; i++, tok = strtok(NULL, ","))
        L2[i] = atoi(tok);
    L2[k] = -1;

    char *in1 = argv[5];
    char *in2 = argv[6];
    char *out = argv[7];
//
    BMM(M, P, in1, L1, "sorted1.txt");
    BMM(M, P, in2, L2, "sorted2.txt");

    merge("sorted1.txt", "sorted2.txt", out, L1, L2);

//    Line *a = Line_create("H6,JD,O,RV,OB");
//    Line *b = Line_create("JD,OB,W,XK,H6,KG");
//
//    Line_less_merge(a, b, L1, L2);

    return(0);
}