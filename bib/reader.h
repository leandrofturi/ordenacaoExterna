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
size_t strcount(char*, char);
size_t strcountR(char*, char); // Recursivo