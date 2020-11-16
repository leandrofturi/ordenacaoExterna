#include <string.h>
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

// https://stackoverflow.com/questions/10535379/how-to-create-an-exact-copy-of-file-in-c/10535452
double copy(char *input, char *output) {
    FILE *f_in = fopen(input, "r");
    FILE *f_out = fopen(output, "w");
    if (!f_in || !f_out) {
        fclose(f_in); fclose(f_out);
        return -1;
    }
    int c;
    while ((c = fgetc(f_in)) != EOF)
        fputc(c, f_out);
    fclose(f_in);
    fseek(f_out, 0, SEEK_END);
    long size = ftell(f_out);
    fclose(f_out);
    return (double)(size / 1024 / 1024); // MB
}

// Replace a character in string
// https://stackoverflow.com/questions/32496497/standard-function-to-replace-character-or-substring-in-a-char-array/32496721
char* strrep(char* str, char find, char replace) {
    char *current_pos = strchr(str,find);
    while(current_pos) {
        *current_pos = replace;
        current_pos = strchr(current_pos,find);
    }
    return str;
}