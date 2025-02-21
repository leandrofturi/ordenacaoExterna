#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bib/reader.h"
#include "bib/ext_sort.h"


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
    
    BMM(M, P, in1, L1, "sorted1.txt");
    BMM(M, P, in2, L2, "sorted2.txt");
    merge("sorted1.txt", "sorted2.txt", L1, L2, out);

    return(0);
}