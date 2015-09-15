#ifndef ARQUIVOS_H
#define ARQUIVOS_H

#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

void OrdenarArquivos(vector<string> &files);			// BubbleSort para ordenar o vetor com os nomes dos arquivos
int BuscarArquivos(string dir, vector<string> &files);
void ListarArquivos(int fruit, string dir);

#endif // ARQUIVOS_H
