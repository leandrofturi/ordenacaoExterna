#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../bib/PQ.h"

struct pq {
    Line **datum;
    int maxN, N;
};


PQ* PQ_create(int maxN) {
    PQ *pq = (PQ*) malloc(sizeof(PQ));
    pq->maxN = maxN;
    pq->N = -1;
    pq->datum = (Line**) malloc(maxN*sizeof(Line*));
    for(size_t i = 0; i < maxN; i++)
        pq->datum[i] = NULL;

    return pq;
}

void PQ_destroy(PQ *pq) {
    for(int i = 0; i <= pq->N; i++) {
        free(Line_getdata(pq->datum[i]));
        Line_del(pq->datum[i]);
    }
    free(pq->datum);
    free(pq);
}

static void fix_up(PQ *pq, int k, int *idexes) { // swim up
    while(k > 0 && Line_greater(pq->datum[k/2], pq->datum[k], idexes)) {
        Line_exch(pq->datum[k], pq->datum[k/2]);
        k = k/2;
    }
}

void PQ_insert(PQ *pq, Line *line, int *idexes) {
    if(!line || pq->N == pq->maxN-1)
        return;

    pq->N++;
    pq->datum[pq->N] = line;
    fix_up(pq, pq->N, idexes);
}

static void fix_down(PQ *pq, int sz, int k, int *idexes) {
    while(2*k <= sz) {
        int j = 2*k;
        if(j < sz && Line_greater(pq->datum[j], pq->datum[j+1], idexes))
            j++;
        if(!Line_greater(pq->datum[k], pq->datum[j], idexes))
            break;
        Line_exch(pq->datum[k], pq->datum[j]);
        k = j;
    }
}

Line* PQ_delmin(PQ *pq, int *idexes) {
    if(PQ_is_empty(pq))
        return NULL;

    Line *min = pq->datum[0];
    Line_exch(pq->datum[0], pq->datum[pq->N]);
    pq->N--;
    fix_down(pq, pq->N, 0, idexes);
    return min;
}

int PQ_is_empty(PQ *pq) {
    return PQ_size(pq) == 0;
}

int PQ_size(PQ *pq) {
    return pq->N+1;
}
