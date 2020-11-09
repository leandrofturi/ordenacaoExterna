#ifndef PQ_H
#define PQ_H

#include "line.h"

typedef struct pq PQ;


PQ* PQ_create(int max_N);

void PQ_destroy(PQ *pq);

void PQ_insert(PQ *pq, Line*, int*);

Line* PQ_delmin(PQ *pq, int*);

int PQ_is_empty(PQ *pq);

int PQ_size(PQ *pq);


#endif
