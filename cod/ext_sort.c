#include "../bib/ext_sort.h"
#include "../bib/reader.h"
#include "../bib/item.h"


char** make_names(int N) {
    int N_DIG = snprintf(0, 0, "%+d", N)-1;
    char buf[N_DIG+1];
    char** names = (char**) malloc(N*sizeof(char*));
    for(int i = 0; i < N; i++) {
        names[i] = (char*) malloc((N_DIG+5)*sizeof(char));
        sprintf(buf, "%d", i);
        strcpy(names[i], buf);
        strcat(names[i], ".txt");
    }
    return(names);
}

void free_names(char **names, int N) {
    for(size_t i = 0; i < N; i++)
        free(names[i]);
    free(names);
}

void fopen_block(FILE **disp, int lo, int hi, char **names, char *op) {
    for(; lo < hi; lo++)
        disp[lo] = myfopen(names[lo], op);
}

void fclose_block(FILE **disp, int lo, int hi) {
    for(; lo < hi; lo++)
        fclose(disp[lo]);
}

void BMM(int M, int P, char *filename, int l) {
    char **nms = make_names(2*P);
    FILE *disp[2*P], *fileR;

    fileR = myfopen(filename, "r");
    disp[0] = myfopen(nms[0], "w");
    while(!feof(fileR))
        fprintf(disp[0], read_line(fileR));
    fclose(fileR); fclose(disp[0]);

    size_t p, m;
    int block = 1, exit = 1;
    while(exit) {
        // Quebras em blocos de tamanho M
        if(!block) {
            fileR = myfopen(nms[P], "r");
            fopen_block(disp, 0, P, nms, "w");
            for(p = 0; exit; p = (p+1)%P) {
                for(m = 0; m < M; m++) {
                    fprintf(disp[p], read_line(fileR));
                    if(feof(fileR)) {
                        fclose(fileR);
                        exit = 0;
                        break;
                    }
                }
                fprintf(disp[p], "#\n");
            }
            fclose_block(disp, 0, P);
            block = 1;
        }
        else {
            fileR = myfopen(nms[0], "r");
            fopen_block(disp, P, 2*P, nms, "w");
            for(p = P; exit; p = ((p+1)%P) + P) {
                for(m = 0; m < M; m++) {
                    read_line(fileR);
                    fprintf(disp[p], read_line(fileR));
                    if(feof(fileR)) {
                        fclose(fileR);
                        exit = 0;
                        break;
                    }
                }
                fprintf(disp[p], "#\n");
            }
            fclose_block(disp, P, 2*P);
            block = 0;
        }
    }
}