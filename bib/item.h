#pragma once
#include <string.h>
#include <stdio.h>

typedef char* item;
#define key(A) (A)
#define less(A, B) strcmp(key(A), key(B)) < 0
#define exch(A, B) { item t = A; A = B; B = t; }
#define compexch(A, B) if (less(B, A)) exch(A, B)
#define char_2Item(A) (A)
#define printItem(A) printf("%s", A)
