#include "../bib/ext_sort.h"
#include "../bib/reader.h"
#include <string.h>

// Estrutura abstraindo cada linha linha do arquivo
struct line {
    char *data; // linha do arquivo propriamente completa 100%
    size_t *cell; // posições onde têm vírgula
};
typedef struct line Line;

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
        mem->lines[i] = (Line*) malloc(sizeof(Line));
    return mem;
}

void Mem_del(Mem *mem) {
    for(size_t i = 0; i < mem->sizemax; i++)
        free(mem->lines[i]);
    free(mem->lines);
    free(mem);
}

// Criação dos nomes dos arquivos auxiliares
// 0.txt, 1.txt, 2.txt, ..., 2*P.txt
char** make_names(int P) {
    int N_DIG = snprintf(0, 0, "%+d", P)-1;
    char buf[N_DIG+1];
    char** names = (char**) malloc(P*sizeof(char*));
    for(int i = 0; i < P; i++) {
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

void load_memory(FILE *fileR, Mem *mem) {
    size_t i = 0;
    char *line, *pch = NULL;
    for(mem->nrows = 0; (mem->nrows < mem->sizemax) && !feof(fileR); mem->nrows++) {
        line = read_line(fileR);
        mem->lines[mem->nrows]->data = line;
        /*
        if(line) {
            char cpy[strlen(line)+1];
            memcpy(cpy, line, strlen(cpy)+1);
            do {
                pch = (char*) memchr(cpy+i, ',', strlen(cpy+i));
                printf("',' found at position %d.\n", pch-cpy);
            } while(pch);
        printf("%s", line);
        
        }*/
    }
    if(!line) mem->nrows--;
}

void write_memory(Mem *mem, FILE *fileW) {
    size_t m;
    for(m = 0; m < mem->nrows; m++) {
        fprintf(fileW, mem->lines[m]->data);
        free(mem->lines[m]->data);
    }
    if(m) fprintf(fileW, "#\n");
}

static int less(char *a, char *b, int *idexes) {
    return(strcmp(a, b) < 0);
}

static void exch(int i, int j, Mem *mem) {
    Line *t = mem->lines[i];
    mem->lines[i] = mem->lines[j];
    mem->lines[j] = t;
}

int partition(Mem *mem, int *idexes, int lo, int hi) {
    int i = lo, j = hi+1;
    Line *v = mem->lines[lo];

    while(1) {
        while(less(mem->lines[++i]->data, v->data, idexes))
            if(i == hi)
                break;
        while(less(v->data, mem->lines[--j]->data, idexes))
            if(j == lo)
                break;
        if(i >= j)
            break;
        exch(i, j, mem);
    }
    exch(lo, j, mem);

    return(j);
}

static void insert_sort(Mem *mem, int *idexes, int lo, int hi) {
    int i, j;
    Line *v;
    for(i = hi; i > lo; i--)
        if(less(mem->lines[i]->data, mem->lines[i-1]->data, idexes))
            exch(i, i-1, mem);
    for(i = lo+2; i <= hi; i++) {
        j = i;
        v = mem->lines[i];
        for(; less(v->data, mem->lines[j-1]->data, idexes); j--)
            mem->lines[j] = mem->lines[j-1];
        mem->lines[j] = v;
    }
}

static void shuffle(Mem *mem) {
    int i, j;
    for(i = 0; i < mem->nrows; i++) {
        j = rand() % mem->nrows;
        exch(i, j, mem);
    }
}

void quick_sort(Mem *mem, int *idexes, int lo, int hi) {
    if(hi <= lo)
        return;
    if(hi <= lo + CUTOFF - 1) {
        insert_sort(mem, idexes, lo, hi);
        return;
    }
    int j = partition(mem, idexes, lo, hi);
    quick_sort(mem, idexes, lo, j-1);
    quick_sort(mem, idexes, j+1, hi);
}

void sort_memory(Mem *mem, int *idexes) {
    if(mem->nrows <= 1)
        return;
    insert_sort(mem, idexes, 0, mem->nrows-1);
    if(mem->nrows > CUTOFF)
        shuffle(mem);
    quick_sort(mem, idexes, 0, mem->nrows-1);
}

/*
 disp -> todos os dispositivos
 block_R -> o bloco que está sendo usado para leitura
*/
int lin_search(FILE **disp, int *idexes, int block_R, int P) {
    int w, r, // w = iteração de escrita, r =  iteração de leitura
        min = 0, // posição do mínimo
        block_W = !block_R ? 0 : P, // Bloco de escrita e leitura são inversos
        disp_writed = 0; // Quantidade de dispositivos que foram escritos. == 1, BMM deve terminar
    char *line[P]; // linha lida do arquivo
    w = block_W;
    int tabu_disp[P], left_devices = P, left_to_read = P;
    for(size_t i = 0; i < P; i++) {
        line[i] = NULL;
        tabu_disp[i] = 0;
    }
    int jj = 20;
    while(jj > 0) {
        jj--;

        for(r = block_R+1; r < P; r++) {
            if(feof(disp[r])) {
                tabu_disp[r] = 1;
                left_devices--;
                left_to_read--;
            }

            if(tabu_disp[r]) // caso seja tabu, não deve ser lido 
                continue;
            if(!line[r-block_R]) // apenas leio se estiver vazio
                line[r-block_R] = read_line(disp[r]); // P-block_R = mapeamento para 0:P

            if(strcmp(line[r-block_R], "#") == 0) { // acabou o bloco?
                printf("ENTROU!\n");
                free(line[r-block_R]);
                tabu_disp[r] = 1; // disp passa a ser tabu
                left_devices--; // decrementa a qnt de disp restantes
                continue;
            }
            if(less(line[r-block_R], line[min], idexes)) // cc, é comparado com o mínimo
                min = r-block_R;
        } // após ler todos os dispositivos,
        fprintf(disp[w], line[min]);
        free(line[min]);
        line[min] = NULL; // essa linha deve ser lida
        if(!left_devices) { // Se tiver lido tudo do bloco, reseta e passa pro próximo
            w = ((w+1)%P)+block_W;
            disp_writed++;
            left_devices = P;
            for(size_t i = 0; i < P; i++) { // Reseta o sistema
                line[i] = NULL;
                tabu_disp[i] = 0;
            }
        }
        if(!left_to_read)
            break;
    }
    return disp_writed;
}

void BMM(int M, int P, char *filename, int *idexes) {
    char *line, **file_names = make_names(2*P);
    FILE *disp[2*P], *fileR;

    fileR = myfopen(filename, "r");
    disp[0] = myfopen(file_names[0], "w");
    while(!feof(fileR)) {
        line = read_line(fileR);
        fprintf(disp[0], line);
        free(line);
    }
    fclose(fileR); fclose(disp[0]);

    int p, exit = 1, block = P;
    Mem *mem = Mem_init(M);
    
    while(exit) {
        // Quebras em blocos de tamanho M
        fileR = myfopen(file_names[P-block], "r");
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

        // Leitura dos valores em ordem em cada bloco
        fopen_block(disp, block, P+block, file_names, "w");
        block = !block ? 0 : P;
        fopen_block(disp, block, P+block, file_names, "r");
        exit = lin_search(disp, idexes, block, P) != 1;
        fclose_block(disp, 0, 2*P);
    }
    Mem_del(mem);
    free_names(file_names, 2*P);
}