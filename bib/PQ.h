#ifndef PQ_H
#define PQ_H

typedef struct pq PQ;
typedef struct data Data;


PQ* PQ_create(int max_N);

void PQ_destroy(PQ *pq);

void PQ_insert(PQ *pq, Data*);

Data* PQ_delmin(PQ *pq);

int PQ_is_empty(PQ *pq);

int PQ_size(PQ *pq);


Data* Data_create(char* line, int pos);

char* Data_getdata(Data*);

int Data_getpos(Data*);

void Data_del(Data*);

#endif
