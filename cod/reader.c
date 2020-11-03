#include <limits.h>
#include "../bib/reader.h"


char* read_line(FILE *arq) {
    size_t n = 0, r;
    char *buf = NULL;

    r = getline(&buf, &n, arq);
    if(r < 0)
        return(NULL);

    if(buf && r >= UINT_MAX) {
        free(buf);
        return(NULL);
    }

    return(buf);
}

void create_files(char **names, int n) {
    FILE *files[n];
    for(int i = 0; i < n; i++) {
        files[i] = myfopen(names[i], "w");
        fclose(files[i]);
    }
}

FILE* myfopen(char *filename, char *op) {
	FILE *file;

	file = fopen(filename, op);
	if(file == NULL) {
		printf("File %s not found!\n", filename);
		exit(1);
	}
	return file;
}

int strcount(char *str, char c) {
    int r = 0;
    for(int i = 0; str[i]; i++) r += str[i] == c;
    return r;
}

int strcountR(char *str, char c) {
    return *str ? ((c==*str) + strcountR(str+1, c)) : 0;
}