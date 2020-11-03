#include "../bib/ext_sort.h"
#include "../bib/reader.h"
#include <string.h>


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
    for(int i = 0; i < N; i++)
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

void load_memory(FILE *fileR, tMem *mem) {
    char *line;
    for(mem->nrows = 0; (mem->nrows < mem->sizemax) && !feof(fileR); mem->nrows++) {
        line = read_line(fileR);
        mem->data[mem->nrows] = line;
    }
    if(!line) mem->nrows--;
}

void write_memory(tMem *mem, FILE *fileW) {
    size_t m;
    for(m = 0; m < mem->nrows; m++) {
        fprintf(fileW, mem->data[m]);
        free(mem->data[m]);
    }
    if(m) fprintf(fileW, "#\n");
}

static int less(int i, int j, tMem *mem, int *idexes) {
    return(strcmp(mem->data[i], mem->data[j]) < 0);
}

static void exch(int i, int j, tMem *mem) {
    char *t = mem->data[i];
    mem->data[i] = mem->data[j];
    mem->data[j] = t;
}

static void insert_sort(tMem *mem, int *idexes, int lo, int hi) {
    int i, j;
    char *v;
    for(i = hi; i > lo; i--)
        if(less(i, i-1, mem, idexes))
            exch(i, i-1, mem);
    for(i = lo+2; i <= hi; i++) {
        j = i;
        v = mem->data[i];
        while(less(i, j-1, mem, idexes)) {
            mem->data[j] = mem->data[j-1];
            j--;
        }
        mem->data[j] = v;
    }
}

void sort_memory(tMem *mem, int *idexes) {
    if(mem->nrows <= 1)
        return;
    insert_sort(mem, idexes, 0, mem->nrows-1);
    //if(mem->nrows > CUTOFF)
    //    shuffle(mem);
    //quick_sort(mem, 0, mem->nrows-1, idexes);
}

void BMM(int M, int P, char *filename, int *idexes) {
    char *line, **nms = make_names(2*P);
    FILE *disp[2*P], *fileR;

    fileR = myfopen(filename, "r");
    disp[0] = myfopen(nms[0], "w");
    while(!feof(fileR)) {
        line = read_line(fileR);
        fprintf(disp[0], line);
        free(line);
    }
    fclose(fileR); fclose(disp[0]);

    int p, exit = 1, block = P;
    tMem *mem = (tMem*) malloc(sizeof(tMem));
    mem->sizemax = M;
    mem->data = (char**) malloc(M*sizeof(char*));
    while(exit) {
        // Quebras em blocos de tamanho M
        fileR = myfopen(nms[P-block], "r");
        fopen_block(disp, block, P+block, nms, "w");
        for(p = block; ; p = ((p+1)%P)+block) {
            load_memory(fileR, mem);
            sort_memory(mem, idexes);
            write_memory(mem, disp[p]);
            if(feof(fileR)) {
                fclose(fileR);
                fclose_block(disp, block, P+block);
                block = !block ? 0 : P;
                break;
            }
        }
        exit = 0;
    }
    free(mem->data);
    free(mem);
    free_names(nms, 2*P);
}