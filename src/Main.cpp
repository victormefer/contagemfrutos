#include "UserInterface.h"
#include <string>

int main(int argc, char** argv)
{
	UserInterface interface;

	std::string arqTreino;
	int fruta, canal, threshold, cut, distTransf;

	if (argc == 7)
	{
		fruta = stoi(string(argv[1]));
		arqTreino = string(argv[2]);
		canal = stoi(string(argv[3]));
		threshold = stoi(string(argv[4]));
		cut = stoi(string(argv[5]));
		distTransf = stoi(string(argv[6]));
		interface.TesteBatch(fruta, arqTreino, canal, threshold, cut, distTransf);
	}
	else
		interface.MainMenu();
	return 0;
}