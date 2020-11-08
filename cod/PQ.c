#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../bib/PQ.h"

#define key(A) (A)
#define greater(A, B) (strcmp(A, B) > 0)
#define exch(A, B) { Data *t = A; A = B; B = t; }

struct data {
    char *data;
    int pos;
};
struct pq {
    Data **datum;
    int maxN, N;
};


PQ* PQ_create(int maxN) {
    PQ *pq = (PQ*) malloc(sizeof(PQ));
    pq->maxN = maxN;
    pq->N = -1;
    pq->datum = (Data**) malloc(maxN*sizeof(Data*));

    return pq;
}

void PQ_destroy(PQ *pq) {
    for(int i = 0; i <= pq->N; i++)
        Data_del(pq->datum[i]);
    free(pq->datum);
    free(pq);
}

static void fix_up(PQ *pq, int k) { // swim up
    while(k > 0 && greater(Data_getdata(pq->datum[k/2]), Data_getdata(pq->datum[k]))) {
        exch(pq->datum[k], pq->datum[k/2]);
        k = k/2;
    }
}

void PQ_insert(PQ *pq, Data *data) {
    if(!data || pq->N == pq->maxN-1)
        return;

    pq->N++;
    pq->datum[pq->N] = data;
    fix_up(pq, pq->N);
}

static void fix_down(PQ *pq, int sz, int k){
    while(2*k <= sz) {
        int j = 2*k;
        if(j < sz && greater(Data_getdata(pq->datum[j]), Data_getdata(pq->datum[j+1])))
            j++;
        if(!greater(Data_getdata(pq->datum[k]), Data_getdata(pq->datum[j])))
            break;
        exch(pq->datum[k], pq->datum[j]);
        k = j;
    }
}

Data* PQ_delmin(PQ *pq) {
    if(PQ_is_empty(pq))
        return NULL;

    Data *min = pq->datum[0];
    exch(pq->datum[0], pq->datum[pq->N]);
    pq->N--;
    fix_down(pq, pq->N, 0);
    return min;
}

int PQ_is_empty(PQ *pq) {
    return PQ_size(pq) == 0;
}

int PQ_size(PQ *pq) {
    return pq->N+1;
}


Data* Data_create(char *data, int pos) {
    Data *new = (Data*) malloc(sizeof(Data));
    new->data = (char*) malloc((strlen(data)+1)*sizeof(char));
    memcpy(new->data, data, strlen(data)+1);
    new->pos = pos;
    return new;
}

char* Data_getdata(Data *data) {
    return data->data;
}

int Data_getpos(Data *data) {
    return data->pos;
}

void Data_del(Data *data) {
    if(!data) return;

    free(data->data);
    free(data);
}
