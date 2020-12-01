#include "../bib/ext_sort.h"
#include "../bib/reader.h"
#include "../bib/PQ.h"
#include "../bib/line.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// Estrutura abstraindo a memória
struct Mem {
    Line **lines; // Vetor contendo ponteiros para cada linha
    size_t sizemax; // quantidade máxima de linhas
    size_t nrows; // quantidade de linhas (Line)
    int ncols; // quantidade de colunas (ou posições)
};


Mem* Mem_init(int M) {
    Mem *mem = (Mem*) malloc(sizeof(Mem));
    mem->sizemax = M;
    mem->lines = (Line**) malloc(M*sizeof(Line*));
    for(size_t i = 0; i < M; i++)
        mem->lines[i] = NULL;
    
    return mem;
}

void Mem_del(Mem *mem) {
    for(size_t i = 0; i < mem->sizemax; i++)
        Line_del(mem->lines[i]);
    free(mem->lines);
    free(mem);
}

// Criação dos nomes dos arquivos auxiliares
// 0.txt, 1.txt, 2.txt, ..., 2*P.txt
char** make_names(int P) {
    int N_DIG = snprintf(0, 0, "%+d", P)-1;
    char buf[N_DIG+5];
    char** names = (char**) malloc(P*sizeof(char*));
    for(int i = 0; i < P; i++) {
        names[i] = (char*) malloc((N_DIG+10)*sizeof(char));
        sprintf(buf, "tmp/%d", i);
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
    for(int i = lo; i < hi; i++)
        disp[i] = myfopen(names[i], op);
}

void fclose_block(FILE **disp, int lo, int hi) {
    for(int i = lo; i < hi; i++)
        fclose(disp[i]);
}

void load_memory(FILE *fileR, Mem *mem) {
    char *line;
    Line *data;
    for(mem->nrows = 0; (mem->nrows < mem->sizemax) && !feof(fileR); mem->nrows++) {
        line = read_line(fileR);
        data = Line_create(line);
        if(data) mem->lines[mem->nrows] = data;
    }
    if(!line) mem->nrows--;
}

void write_memory(Mem *mem, FILE *fileW) {
    size_t m;
    for(m = 0; m < mem->nrows; m++) {
        fprintf(fileW, Line_getdata(mem->lines[m]));
        free(Line_getdata(mem->lines[m]));
        Line_del(mem->lines[m]);
        mem->lines[m] = NULL;
    }
    if(m) fprintf(fileW, "#\n");
}

int partition(Mem *mem, int *idexes, int lo, int hi) {
    int i = lo, j = hi+1;
    Line *v = mem->lines[lo];

    while(1) {
        while(Line_less(mem->lines[++i], v, idexes))
            if(i == hi)
                break;
        while(Line_less(v, mem->lines[--j], idexes))
            if(j == lo)
                break;
        if(i >= j)
            break;
        Line_exch(mem->lines[i], mem->lines[j]);
    }
    Line_exch(mem->lines[lo], mem->lines[j]);

    return(j);
}

static void insert_sort(Mem *mem, int *idexes, int lo, int hi) {
    int i, j;
    Line *v;
    for(i = hi; i > lo; i--)
        if(Line_less(mem->lines[i], mem->lines[i-1], idexes))
            Line_exch(mem->lines[i], mem->lines[i-1]);
    for(i = lo+2; i <= hi; i++) {
        j = i;
        v = mem->lines[i];
        for(; Line_less(v, mem->lines[j-1], idexes); j--)
            mem->lines[j] = mem->lines[j-1];
        mem->lines[j] = v;
    }
}

static void shuffle(Mem *mem) {
    int i, j;
    for(i = 0; i < mem->nrows; i++) {
        j = rand() % mem->nrows;
        Line_exch(mem->lines[i], mem->lines[j]);
    }
}

void quick_sort(Mem *mem, int *idexes, int lo, int hi) {
    if(hi <= lo)
        return;
//    if(hi <= lo + CUTOFF - 1) {
//        insert_sort(mem, idexes, lo, hi);
//        return;
//    }
    int j = partition(mem, idexes, lo, hi);
    quick_sort(mem, idexes, lo, j-1);
    quick_sort(mem, idexes, j+1, hi);
}

void sort_memory(Mem *mem, int *idexes) {
    if(mem->nrows <= 1)
        return;
//    insert_sort(mem, idexes, 0, mem->nrows-1);
    if(mem->nrows > CUTOFF)
        shuffle(mem);
    quick_sort(mem, idexes, 0, mem->nrows-1);
}

static int all_block_feof(FILE **disp, int block, int P) {
    int c = 0;
    for(size_t i = block; i < (block+P); i++)
        if(feof(disp[i])) c++;
    return c == P;
}

// Push a valid line in heap
static int push_line(FILE *disp, int r, PQ *pq, int *idexes) {
    Line *data;
    char *line = read_line(disp);
    if(line && strcmp(line, "#\n") == 0) {
        free(line);
        return 1;
    }
    if(line) {
        data = Line_create(line);
        Line_setpos(data, r);
        PQ_insert(pq, data, idexes);
        return 0;
    }
    return 1;
}

int lin_search(FILE **disp, int *idexes, int block_R, int P) {
    int block_W = block_R == P ? 0 : P;
    int r, w = block_W, readed = 0, 
        tabu[2*P];
    Line *min;
    PQ *pq = PQ_create(P);

    while(1) {
        tabu[w] = 0; // Counter of files "tabu"
        for(r = block_R; r < block_R+P; r++) {
            tabu[r] = push_line(disp[r], r, pq, idexes); // insert non-null line in heap
            tabu[w] += tabu[r]; // if file is not tabu, tabu[r] = 0
        }
        while(!PQ_is_empty(pq)) {
            min = PQ_delmin(pq, idexes); // remove min value
            fprintf(disp[w], Line_getdata(min)); // write in a file
            r = Line_getpos(min); // take the file when minimum occors
            free(Line_getdata(min));
            Line_del(min);

            if(!tabu[r]) { // insert new element, as we remove one
                tabu[r] = push_line(disp[r], r, pq, idexes);
                tabu[w] += tabu[r];
            }
        }
        if(tabu[w] == P) { // all files is "tabu"?
            if(all_block_feof(disp, block_R, P)) break; // all files ended? break.

            fprintf(disp[w], "#\n");
            readed++; // numbers of files readed
            w = ((w+1)%P)+block_W; // next file to write
        }
    }
    PQ_destroy(pq);
    return readed;
}

// Note that the idexes with the column order need to ends with -1
void BMM(int M, int P, char *filename, int *idexes, char *filename_sorted) {
    struct stat st = {0};
    if(stat("tmp", &st) != 0)
        mkdir("tmp", 0777);
    else system("exec rm -r tmp/*");

    int p, readed = 0, block = P;
    char **file_names = make_names(2*P);
    FILE *disp[2*P], *fileR;
    Mem *mem = Mem_init(M);
    
    // É realizada uma primeira passada sobre o arquivo,
    // quebrando-o em blocos do tamanho M. Cada bloco é
    // ordenado com um método para memória interna
    // - Os blocos são salvos nos dispositivos P, P+1, ..., 2P-1
    fileR = myfopen(filename, "r");
    fopen_block(disp, block, P+block, file_names, "w");
    for(p = block; ; p = ((p+1)%P)+block) {
        load_memory(fileR, mem);
        sort_memory(mem, idexes);
        write_memory(mem, disp[p]);
        if(feof(fileR)) {
            fclose(fileR);
            fclose_block(disp, block, P+block);
            break;
        }
    }
    // Os P primeiros blocos ordenados são intercalados
    // - Os resultados são blocos maiores, distribuídos nos
    // dispositivos seguintes
    while(readed != 1) {
        fopen_block(disp, block, P+block, file_names, "r");
        block = block == P ? 0 : P;
        fopen_block(disp, block, P+block, file_names, "w");
        readed = lin_search(disp, idexes, block == P ? 0 : P, P);
        fclose_block(disp, 0, 2*P);
    }

    // Create a copy of sorted file in an knew place
    copy(file_names[block], filename_sorted);

    Mem_del(mem);
    free_names(file_names, 2*P);

    if(stat("tmp", &st) == 0)
        system("exec rm -r tmp");
}

void merge(char *filename1, char *filename2, int *idexes1, int *idexes2, char *filenameout) {
    int cmp;
    char *line1, *line2, *lineF;
    Line *L1, *L2;
    FILE *f1 = myfopen(filename1, "r");
    FILE *f2 = myfopen(filename2, "r");
    FILE *fout = myfopen(filenameout, "w");

    line1 = read_line(f1);
    line2 = read_line(f2);
    L1 = Line_create(line1);
    L2 = Line_create(line2);
    while (!feof(f1) && !feof(f2)) {
        cmp = Line_cmp(L1, L2, idexes1, idexes2);
        if(cmp == 0) {
            lineF = Line_formated(L1, L2, idexes1, idexes2);
            fprintf(fout, lineF);
            free(lineF);

            if(L1) { Line_del(L1); L1 = NULL; }
            if(line1) { free(line1); line1 = NULL; }
            if(L2) { Line_del(L2); L2 = NULL; }
            if(line2) { free(line2); line2 = NULL; }
            
            line1 = read_line(f1); L1 = Line_create(line1);
            line2 = read_line(f2); L2 = Line_create(line2);
        }
        else if(cmp > 0) { // line1 maior
            if(L2) Line_del(L2);
            if(line2) free(line2);
            line2 = read_line(f2); L2 = Line_create(line2);
        }
        else { // line2 maior
            if(L1) { Line_del(L1); L1 = NULL; }
            if(line1) { free(line1); line1 = NULL; }
            line1 = read_line(f1); L1 = Line_create(line1);
        }

        if(line1 && strcmp(line1, "#\n") == 0) {
            Line_del(L1); free(line1); line1=NULL; L1=NULL;
            read_line(f1);
        }
        if(line2 && strcmp(line2, "#\n") == 0) {
            Line_del(L2); free(line2); line2=NULL; L2=NULL;
            read_line(f2);
        }
    }
    if(line1) free(line1);
    if(line2) free(line2);
    if(L1) Line_del(L1);
    if(L2) Line_del(L2);
    fclose(f1);
    fclose(f2);
    fclose(fout);
}