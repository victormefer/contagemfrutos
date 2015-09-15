#include "UserInterface.h"

using namespace cv;

void UserInterface::MainMenu()
{

	int opcao;
/*	do
	{
		system("clear");

		std::cout << "*** Treinamento da arvore de decisao ***" << std::endl << std::endl
			<< "1. Treino Manual da Arvore" << std::endl
			<< "2. Carregar Arvore de um Arquivo" << std::endl
			<< "3. Salvar Arvore em Arquivo" << std::endl
			<< "4. Carregar Treino de um Arquivo" << std::endl
			<< "5. Salvar Treino em Arquivo" << std::endl
			<< "6. Classificar imagem" << std::endl
			<< "7. Adicionar imagem ao SIFT" << std::endl
			<< "8. Testar SIFT" << std::endl
			<< "9. Teste Batch" << std::endl
			<< "10. Encontrar Extremos Locais" << std::endl
			<< "11. Watershed" << std::endl
			<< "12. Watershed2" << std::endl
			<< "0. Sair" << std::endl;

		std::cout << std::endl << "Escolha uma opcao: ";
		std::cin >> opcao;

		switch(opcao)
		{
			case 1:
				TreinoManual();
				break;
			case 2:
				CarregarArvore();
				break;
			case 3:
				SalvarArvore();
				break;
			case 4:
				CarregarTreino();
				break;
			case 5:
				SalvarTreino();
				break;
			case 6:
				Classificar();
				break;
			case 7:
				AddSIFT();
				break;
			case 8:
				TestarSIFT();
				break;
			case 9:
				TesteBatch();
				break;
			case 10:
				ExtremosLocais();
				break;
			case 11:
				Watershed(0);
				break;
			case 12:
				Watershed(1);
				break;
			case 0:
				break;
			default:
				std::cout << "Opcao invalida" << std::endl;
				std::cin.get();
				break;
		}

	} while (opcao != 0);*/

	cv::namedWindow("Imagem original", WINDOW_NORMAL);
	cv::namedWindow("Resultado classificacao", WINDOW_NORMAL);
	cv::namedWindow("Resultado blobs", WINDOW_NORMAL);

	do
	{
		system("clear");

		std::cout << "*** Menu principal ***" << std::endl << std::endl
			<< "1. Treino Manual da Arvore" << std::endl
			<< "2. Carregar Treino de um Arquivo" << std::endl
			<< "3. Classificar imagem" << std::endl
			<< "4. Watershed" << std::endl
			<< "5. Procedimento completo" << std::endl
			<< "6. Superpixel" << std::endl
			<< "7. Teste Batch" << std::endl
			<< "0. Sair" << std::endl;

		std::cout << std::endl << "Escolha uma opcao: ";
		std::cin >> opcao;

		switch(opcao)
		{
			case 1:
				TreinoManual();
				break;
			case 2:
				CarregarTreino();
				break;
			case 3:
				Classificar();
				break;
			case 4:
				Watershed();
				break;
			case 5:
				CarregarTreino();
				Classificar();
				Watershed();
				break;
			case 6:
				Superpixel(30,5);
				break;
			case 7:
				TesteBatch();
				break;
			case 0:
				break;
			default:
				std::cout << "Opcao invalida" << std::endl;
				std::cin.get();
				break;
		}

	} while (opcao != 0);
}


Mat UserInterface::CarregarImagem()
{
	std::string nomeImagem;
	Mat img;

	system("clear");
	while (1)
	{
		std::cout << "Informe o nome da imagem: ";
		std::cin >> nomeImagem;

		img = imread(nomeImagem);
		if (img.data == NULL)
			std::cout << std::endl << "Tente novamente. ";
		else
			break;
	}
	return img;
}


void UserInterface::TreinoManual()
{
	char opcao;
	Mat dadosTreino = Mat();
	Mat classesTreino = Mat();

	do
	{
		Mat img = CarregarImagem();
		system("clear");

		int nClasses;
		std::cout << std::endl << "Numero de classes: ";
		std::cin >> nClasses;
		trainer.SetNumberClasses(nClasses);

		bool flag = false;
		do
		{
			std::cout << std::endl << "1. RGB + Lab" << std::endl 
				<< "2. RGB + RGB equalizado + Lab" << std::endl 
				<< "3. RGB + Lab + RGB equal + Lab equal" << std::endl;
			std::cout << std::endl << "Escolha uma opção para os dados de treino: ";
			std::cin >> canaisTreino;

			switch (canaisTreino)
			{
				case 1:
					trainer.SetColorSpaces(RGB | LAB);
					break;
				case 2:
					trainer.SetColorSpaces(RGB | LAB | RGB_EQ);
					break;
				case 3:
					trainer.SetColorSpaces(RGB | LAB | RGB_EQ | LAB_EQ);
					break;
				default:
					flag = true;
			}
		} while (flag);

		SeletorROIs(img, &dadosTreino, &classesTreino, nClasses);
		trainer.AddTrainingData(&dadosTreino, &classesTreino);
		img.release();

		std::cout << std::endl << "Deseja adicionar outra imagem ao treino? (S/N): ";
		std::cin >> opcao;

	} while (opcao != 'n' && opcao != 'N');

	trainer.Train();

	char salvar;
	std::cout << "Deseja salvar o treino realizado? (S/N): ";
	std::cin >> salvar;
	if (salvar == 's' || salvar == 'S')
		SalvarTreino();
}


void UserInterface::SeletorROIs(Mat img, Mat* dadosTreino, Mat* classesTreino, int nClasses)
{
	int nPixels = 0, x, y, width = 10, height = 10, classe = 0;
	Mat imgRect, imgAux;

	img.copyTo(imgRect);

	// Converter imagem para os espaços de cor
	Mat imgLab, imgRgbEq, imgLabEq;
	cvtColor(img, imgLab, CV_BGR2Lab);
	if (canaisTreino >= 2)
	{
		std::vector<Mat> RGBChannels;
		split(img, RGBChannels);
		equalizeHist(RGBChannels[0], RGBChannels[0]);
		equalizeHist(RGBChannels[1], RGBChannels[1]);
		equalizeHist(RGBChannels[2], RGBChannels[2]);
		merge(RGBChannels, imgRgbEq);
		if (canaisTreino >= 3)
		{
			std::vector<Mat> LABChannels;
			split(imgLab, LABChannels);
			equalizeHist(LABChannels[0], LABChannels[0]);
			equalizeHist(LABChannels[1], LABChannels[1]);
			equalizeHist(LABChannels[2], LABChannels[2]);
			merge(LABChannels, imgLabEq);
		}
	}

	namedWindow("Seletor de dados de treino", WINDOW_NORMAL);
	moveWindow("Seletor de dados de treino", 0, 0);

	createTrackbar("Classe", "Seletor de dados de treino", &classe, nClasses - 1);

	x = img.cols / 2;
	y = img.rows / 2;

	dadosTreino->release();
	classesTreino->release();

	while(1)
	{
		imgRect.copyTo(imgAux);
		rectangle( imgAux, Rect( x, y, width, height ), Scalar(0, 0, 255), 2 );

		imshow("Seletor de dados de treino", imgAux);
		char key = waitKey();

		if (key == 27) // ESC
			break;
		else if (key == 10) // ENTER
		{
			for (int i = 0; i < height; i++)
				for (int j = 0; j < width; j++)
				{
					Mat auxAttr = Mat( 1, (canaisTreino + 1) * 3, CV_32FC1 );
					Mat auxClasses = Mat( 1, 1, CV_32S );
					auxClasses.at<int>(0) = classe;

					auxAttr.at<float>(0, 0) = (float)img.at<Vec3b>(y + i, x + j)[0];
					auxAttr.at<float>(0, 1) = (float)img.at<Vec3b>(y + i, x + j)[1];
					auxAttr.at<float>(0, 2) = (float)img.at<Vec3b>(y + i, x + j)[2];
					auxAttr.at<float>(0, 3) = (float)imgLab.at<Vec3b>(y + i, x + j)[0];
					auxAttr.at<float>(0, 4) = (float)imgLab.at<Vec3b>(y + i, x + j)[1];
					auxAttr.at<float>(0, 5) = (float)imgLab.at<Vec3b>(y + i, x + j)[2];
					if (canaisTreino >= 2)
					{
						auxAttr.at<float>(0, 6) = (float)imgRgbEq.at<Vec3b>(y + i, x + j)[0];
						auxAttr.at<float>(0, 7) = (float)imgRgbEq.at<Vec3b>(y + i, x + j)[1];
						auxAttr.at<float>(0, 8) = (float)imgRgbEq.at<Vec3b>(y + i, x + j)[2];
						if (canaisTreino == 3)
						{
							auxAttr.at<float>(0, 9) = (float)imgLabEq.at<Vec3b>(y + i, x + j)[0];
							auxAttr.at<float>(0, 10) = (float)imgLabEq.at<Vec3b>(y + i, x + j)[1];
							auxAttr.at<float>(0, 11) = (float)imgLabEq.at<Vec3b>(y + i, x + j)[2];
						}
					}

					dadosTreino->push_back(auxAttr);
					classesTreino->push_back(auxClasses);

					nPixels++;
				}
			rectangle( imgRect, Rect( x, y, width, height ), Scalar(255, 0, 0), 2 ); // Pinta retangulo azul para referencia pros proximos
		}
		else
		{
			switch(key)
			{
				case 'R': // cima
					if (y > 0) y--;
					break;
				case 'T': // baixo
					if (y < img.rows - 11) y++;
					break;
				case 'S': // direita
					if (x < img.cols - 11) x++;
					break;
				case 'Q': // esquerda
					if (x > 0) x--;
					break;
				case 'w': // cima
					if (y > 10) y-=10;
					else y = 0;
					break;
				case 's': // baixo
					if (y < img.rows - 21) y+=10;
					else y = img.rows - 11;
					break;
				case 'd': // direita
					if (x < img.cols - 21) x+=10;
					else x = img.cols - 11;
					break;
				case 'a': // esquerda
					if (x > 10) x-=10;
					else x = 0;
					break;
				default:
					break;
			}
		}
	}

	destroyWindow("Seletor de dados de treino");
}


void UserInterface::Classificar()
{
	std::string nomeImagem;
	int espacosCores;

	system("clear");

	if (trainer.GetTree() == NULL)
	{
		std::cout << "A arvore ainda nao foi treinada. Carregue um treino ou treine manualmente a arvore." << std::endl;
		std::cin.get();
		return;
	}

	switch(trainer.GetNAttributes())
	{
		case 6:
			espacosCores = 3;
			break;
		case 9:
			espacosCores = 7;
			break;
		case 12:
			espacosCores = 15;
			break;
	}

	while (1)
	{
		std::cout << "Informe o nome da imagem: ";
		std::cin >> nomeImagem;

		imgOriginal = imread(nomeImagem);
		if (imgOriginal.data == NULL)
			std::cout << std::endl << "Tente novamente. ";
		else
			break;
	}

	Classifier classifier = Classifier(nomeImagem, trainer.GetTree(), trainer.GetNClasses(), espacosCores);
	classifier.Classify(&resultClassif);

	imshow("Imagem original", imgOriginal);
	imshow("Resultado classificacao", resultClassif);
	waitKey();

	char salvar;
	std::cout << "Deseja salvar a imagem resultante? (S/N): ";
	std::cin >> salvar;
	if (salvar == 's' || salvar == 'S')
	{
		std::string nomeIm;
		std::cout << "Digite o nome da imagem a ser salva: ";
		std::cin >> nomeIm;
		imwrite(nomeIm, resultClassif);
	}
}



void UserInterface::CarregarArvore()
{
	std::string nomeArq;

	system("clear");
	std::cout << "Informe o nome do arquivo: ";
	std::cin >> nomeArq;

	try
	{
		trainer.LoadTree(nomeArq);
	}
	catch (std::string& ex)
	{
		std::cout << "Nao foi possivel abrir essa imagem.";
		std::cin.get();
	}
}



void UserInterface::SalvarArvore()
{
	std::string nomeArq;

	system("clear");
	std::cout << "Informe o nome do arquivo sem a extensao: ";
	std::cin >> nomeArq;

	nomeArq += ".xml";

	trainer.SaveTree(nomeArq);
}


void UserInterface::CarregarTreino()
{
	std::string nomeArq;

	system("clear");
	std::cout << "Informe o nome do arquivo: ";
	std::cin >> nomeArq;

	try
	{
		trainer.LoadTrainingData(nomeArq);
	}
	catch (std::string& ex)
	{
		std::cout << "Nao foi possivel abrir esse treino.";
		std::cin.get();
	}
	trainer.Train();
}



void UserInterface::SalvarTreino()
{
	std::string nomeArq;

	system("clear");
	std::cout << "Informe o nome do arquivo sem a extensao: ";
	std::cin >> nomeArq;

	nomeArq += ".xml";

	trainer.SaveTrainingData(nomeArq);
}


void UserInterface::AddSIFT()
{
	std::string nomeArq;
	char opcao;

	do
	{
		system("clear");
		std::cout << "Informe o nome da imagem do objeto a ser acrescentado na base de dados: ";
		std::cin >> nomeArq;

		try
		{
			sift.AddImageToDatabase(nomeArq);
		}
		catch (std::string& ex)
		{
			std::cout << "Nao foi possivel abrir a imagem." << std::endl;
		}

		std::cout << "Deseja acrescentar outra imagem a base? (S/N): ";
		std::cin >> opcao;
	} while (opcao == 's' || opcao == 'S');

}



void UserInterface::TestarSIFT()
{
	std::string nomeArq;
	//char opcao;
	//SiftMatcher sift;
/*
	do
	{
		system("clear");
		std::cout << "Informe o nome da imagem do objeto a ser acrescentado na base de dados: ";
		std::cin >> nomeArq;

		try
		{
			sift.AddImageToDatabase(nomeArq);
		}
		catch (std::string& ex)
		{
			std::cout << "Nao foi possivel abrir a imagem." << std::endl;
		}

		std::cout << "Deseja acrescentar outra imagem a base? (S/N): ";
		std::cin >> opcao;
	} while (opcao == 's' || opcao == 'S');

*/
	system("clear");

	Mat img;
	while (1)
	{
		std::cout << "Informe o nome da imagem de teste a ser detectada: ";
		std::cin >> nomeArq;

		img = imread(nomeArq);
		if (img.data == NULL)
			std::cout << std::endl << "Tente novamente. ";
		else
			break;
	}
	Mat saida = sift.TestSift(img);

	namedWindow("Resultado SIFT", WINDOW_NORMAL);
	moveWindow("Resultado SIFT", 0, 0);

	imshow("Resultado SIFT", saida);
	waitKey();

	destroyWindow("Resultado SIFT");


	char salvar;
	std::cout << "Deseja salvar a imagem resultante? (S/N): ";
	std::cin >> salvar;
	if (salvar == 's' || salvar == 'S')
	{
		std::string nomeIm;
		std::cout << "Digite o nome da imagem a ser salva: ";
		std::cin >> nomeIm;
		imwrite(nomeIm, saida);
	}
}


int UserInterface::Comparar(std::string nomeArq, Mat saida)
{
	Mat img = imread(nomeArq);
	int positives = 0;

	for (int i = 0; i < img.rows; i++)
		for (int j = 0; j < img.cols; j++)
		{
			if (saida.at<Vec3b>(i, j) == img.at<Vec3b>(i, j))
			{
				positives++;
			}
		}
	return positives;
}


void UserInterface::ExtremosLocais()
{
	cv::Mat saida, imgGray, maskGray;
	cv::cvtColor(imgOriginal, imgGray, CV_BGR2GRAY);
	cv::cvtColor(resultClassif, maskGray, CV_BGR2GRAY);

	// cv::namedWindow("Mascara", cv::WINDOW_NORMAL);
	// cv::moveWindow("Mascara", 500, 500);
	cv::namedWindow("Extremos locais", cv::WINDOW_NORMAL);
	cv::moveWindow("Extremos locais", 500, 0);

	// cv::imshow("Mascara", maskGray);

	LocalExtrema::LocalMaxima(imgGray, saida, 3);

	saida = saida & maskGray;

	resultExtrem = cv::Mat(saida.size(), CV_32SC1);

	threshold(saida, resultExtrem, 0, 255, THRESH_BINARY);

	cv::imshow("Extremos locais", resultExtrem);

	cv::waitKey();
}

void UserInterface::Watershed()
{
	// Mat markers = Mat::zeros(imgOriginal.size(), imgOriginal.type());

	cv::cvtColor(resultClassif, resultClassif, CV_BGR2GRAY);

	// resultClassif.convertTo(resultClassif, CV_8U);

	numFruits = Watershed::FindWatershed(imgOriginal, resultClassif, resultBlobs, blobMassCenters);

	cv::imshow("Resultado blobs", resultBlobs/* * 10000*/);
	cv::waitKey();

	BlobSplit::SplitBlobs(imgOriginal, resultBlobs, blobMassCenters);

}

void UserInterface::Superpixel(int nr_superpixels,int nc)
{
	Mat img,img2,lab_img;
	double step;

	img = CarregarImagem();
	system("clear");

	cv::cvtColor(img,lab_img,COLOR_BGR2Lab);
	step = sqrt((img.total()) / (double) nr_superpixels);

	/* Perform the SLIC superpixel algorithm. */
	Slic slic;
	slic.generate_superpixels(lab_img,step,nc);
	slic.create_connectivity(lab_img);

	/* Display the contours and show the result. */
	slic.display_contours(img2,cv::Vec3b(0,0,255));
	imshow("result", img2);
	waitKey(0);
}


void UserInterface::TesteBatch()
{
	int opcao;
	std::string nomeArqTreino, imgDir, groundTruthDir;
	int espacosCores;
	int TP = 0, FP = 0, FN = 0;
	double precision, recall, fmeasure;

	system("clear");

	std::cout << "Bases de frutas" << std::endl << std::endl
		<< "1. Acerola" << std::endl
		<< "2. Laranja" << std::endl
		<< "3. Morango" << std::endl;

	std::cout << std::endl << "Escolha uma opcao: ";
	std::cin >> opcao;

	switch(opcao)
	{
		case 1:
			nomeArqTreino = "treinos/acerola_01.xml";
			imgDir = "fruit-database/frutas/Acerola/";
			groundTruthDir = "fruit-database/rotulamento/Acerola/";
			break;
		case 2:
			nomeArqTreino = "treinos/laranja_01.xml";
			imgDir = "fruit-database/frutas/Laranja/";
			groundTruthDir = "fruit-database/rotulamento/Laranja/";
			break;
		case 3:
			nomeArqTreino = "treinos/morango_03.xml";
			imgDir = "fruit-database/frutas/Morango/";
			groundTruthDir = "fruit-database/rotulamento/Morango/";
			break;
		default:
			return;
	}

	// Carregar arquivo de treino
	try
	{
		trainer.LoadTrainingData(nomeArqTreino);
	}
	catch (std::string& ex)
	{
		std::cout << "Nao foi possivel abrir esse treino.";
		getchar();
	}

	trainer.Train();

	switch(trainer.GetNAttributes())
	{
		case 6:
			espacosCores = 3;
			break;
		case 9:
			espacosCores = 7;
			break;
		case 12:
			espacosCores = 15;
			break;
	}

	std::vector<std::string> filenames;
	std::ifstream ifs;

	// Retornar nomes dos arquivos
	if (BuscarArquivos(imgDir, filenames))
	{
		std::cout << "Erro ao abrir os arquivos no diretorio." << std::endl;
		getchar();
		return;
	}

	for (int i = 0; i < filenames.size(); i++)
	{
		if( (filenames[i].compare(".") != 0) && (filenames[i].compare("..") != 0) )		// Filtra o '.' e o '..'
		{
			imgOriginal = imread(imgDir + filenames[i]);
			if (imgOriginal.data == NULL)
			{
				std::cout << "Nao foi possivel abrir a imagem " << filenames[i] << std::endl;
				continue;
			}
			imshow("Imagem original", imgOriginal);

			Classifier classifier = Classifier(imgDir + filenames[i], trainer.GetTree(), trainer.GetNClasses(), espacosCores);
			classifier.Classify(&resultClassif);

			imshow("Resultado classificacao", resultClassif);

			Watershed();

			ifs.open( groundTruthDir + filenames[i].substr(0, filenames[i].find('.')) + ".txt" );
			if (!ifs.is_open())
			{
				std::cout << "Nao foi possivel abrir o arquivo de ground truth da imagem " << filenames[i] << std::endl;
				continue;
			}

			int groundTruth;
			ifs >> groundTruth;

			ifs.close();

			if (groundTruth == numFruits)
				TP += numFruits;
			else if (groundTruth > numFruits)
			{
				FN += groundTruth - numFruits;
				TP += numFruits;
			}
			else
			{
				FP += numFruits - groundTruth;
				TP += groundTruth;
			}
		}
	}

	precision = (double)TP / ((double)TP + (double)FP);
	recall = (double)TP / ((double)TP + (double)FN);
	fmeasure = 2 * precision * recall / (precision + recall);

	system("clear");

	std::ofstream ofs;
	ofs.open("saida.txt");

	ofs << std::endl << "True positives: " << TP << std::endl
		<< "False positives: " << FP << std::endl
		<< "False negatives: " << FN << std::endl << std::endl
		<< "Precision: " << precision << std::endl
		<< "Recall: " << recall << std::endl
		<< "F-measure: " << fmeasure << std::endl;
	getchar();

	ofs.close();
}