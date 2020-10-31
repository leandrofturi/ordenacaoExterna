#include "../bib/ext_sort.h"
#include "../bib/reader.h"
#include "../bib/item.h"


FILE** open_files(int N) {
    FILE **disp = malloc(sizeof(FILE*)*N);
    // http://www.cplusplus.com/reference/cstdio/snprintf/
    int N_DIG = snprintf(0, 0, "%+d", N)-1;
    char name[N_DIG+5];
    char buf[N_DIG+1];
    for(int i = 0; i < N; i++) {
        sprintf(buf, "%d", i);
        strcpy(name, buf);
        strcat(name, ".txt");
        disp[i] = myfopen(name, "wb+");
    }
    return(disp);
}

void close_files(FILE **files, int N) {
    for(size_t i = 0; i < N; i++)
        fclose(files[i]);
    free(files);
}

void BMM(int M, int P, char *filename, int l) {
    FILE **disp = open_files(2*P);
    FILE *in = myfopen(filename, "r");

    size_t p, m;
    int block = 1, exit = 1;
    while(exit) {
        if(block) {
            for(p = P; p < 2*P; p++)
                for(m = 0; m < M; m++) {
                    fprintf(disp[p], read_line(in));
                    if(feof(in)) exit = 0;
                }
            block = 0;
        }
        else {
            for(p = 0; p < P; p++)
                for(m = 0; m < M; m++) {
                    fprintf(disp[p], read_line(in));
                    if(feof(in)) exit = 0;
                }
            block = 1;
        }
    }

    close_files(disp, 2*P);
    fclose(in);
}