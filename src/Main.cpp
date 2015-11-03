#include "UserInterface.h"
#include <string>

int main(int argc, char** argv)
{
	UserInterface interface;

	std::string arqTreino;
	int fruta, inicio, canal, threshold, cut, distTransf;

	if (argc == 8)		/*Numero de parametros na linha de comando do programa*/
	{
		fruta = stoi(string(argv[1]));
		inicio = stoi(string(argv[2]));
		arqTreino = string(argv[3]);
		canal = stoi(string(argv[4]));
		threshold = stoi(string(argv[5]));
		cut = stoi(string(argv[6]));
		distTransf = stoi(string(argv[7]));
		interface.TesteBatch(fruta, inicio, arqTreino, canal, threshold, cut, distTransf);
	}
	else
		interface.MainMenu();
	return 0;
}
