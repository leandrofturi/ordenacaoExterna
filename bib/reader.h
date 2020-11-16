#pragma once
#include <stdio.h>
#include <stdlib.h>


// Leitura de uma linha de tamanho arbitrário.
// https://www.gnu.org/software/libc/manual/html_node/Line-Input.html
char* read_line(FILE*);

// Criar arquivos a partir de uma lista de nomes
void create_files(char**, int);

// Versão de abertura do arquivo
FILE* myfopen(char*, char*);

// Contar as ocorrências de um caracter específico em uma string.
int strcount(char*, char);
int strcountR(char*, char); // Recursivo

// Create an exact copy of file
// https://stackoverflow.com/questions/10535379/how-to-create-an-exact-copy-of-file-in-c/10535452
double copy(char*, char*);

// Replace a character in string
// https://stackoverflow.com/questions/32496497/standard-function-to-replace-character-or-substring-in-a-char-array/32496721
char* strrep(char*, char, char);