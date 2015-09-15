#include "Arquivos.h"

using namespace std;								// Funcoes da biblioteca iostream

void OrdenarArquivos(vector<string> &files)			// BubbleSort para ordenar o vetor com os nomes dos arquivos
{
    unsigned int i, troca;
    string temp;

    troca = 1;
    while(troca == 1)
    {
        troca = 0;
        for(i = 0;i <= files.size() - 2;i++)
        {
            if(files[i].compare(files[i+1]) > 0)
            {
                temp = files[i];
                files[i] = files[i+1];
                files[i+1] = temp;
                troca = 1;
            }
        }
    }
}

int BuscarArquivos(string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;

    if((dp = opendir(dir.c_str())) == NULL)
    {
        cout << "Erro (" << errno << ") ao abrir " << dir << endl;
        return errno;
    }
    while ((dirp = readdir(dp)) != NULL) files.push_back(string(dirp->d_name));
    closedir(dp);
    OrdenarArquivos(files);

    return 0;
}

void ListarArquivos(int fruit, string dir)
{
    ifstream file;
    string path,linha;
    vector<string> files = vector<string>();
    int busca;

    busca = BuscarArquivos(dir,files);
    if(busca != 0)
    {
        if(fruit == 1) cout << endl << "Erro ao buscar os arquivos de rotulamento de Acerolas!!";
        else if(fruit == 2) cout << endl << "Erro ao buscar os arquivos de rotulamento de Laranjas!!";
        else cout << endl << "Erro ao buscar os arquivos de rotulamento de Morangos!!";
        return;
    }

    if(fruit == 1) cout << endl << "- Acerolas -" << endl << endl;
    else if(fruit == 2) cout << endl << "- Laranjas -" << endl << endl;
    else cout << endl << "- Morangos -" << endl << endl;

    for (unsigned int i = 0;i < files.size();i++)
    {
        if( (files[i].compare(".") != 0) && (files[i].compare("..") != 0) )		// Filtra o '.' e o '..'
        {
            cout << files[i] << ": ";											// Imprime o nome do arquivo

            if(fruit == 1) path = dir + files[i];							// Concatena o diretorio com o nome do arquivo
            else if(fruit == 2) path = dir + files[i];
            else path = dir + files[i];
            file.open(path.c_str());
            if(file.is_open())
            {
                getline(file,linha);
                cout << linha << endl;											// Imprimir o numero no arquivo
                file.close();
            }
        }
    }
}
