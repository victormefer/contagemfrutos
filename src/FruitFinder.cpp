#include "FruitFinder.h"

cv::Mat FruitFinder::imgYCbCr, FruitFinder::markers, FruitFinder::channels[3];
int FruitFinder::thresh = 8;
// std::vector<cv::Vec3b> FruitFinder::colors;

int FruitFinder::FindFruits(cv::Mat img, cv::Mat mask, cv::Mat& outputMarkers)
{
	std::vector<cv::Point2f> massCenters;
	std::vector<std::vector<cv::Point>> contours;
	bool hasSplit;

	cv::distanceTransform(mask, markers, CV_DIST_L2, CV_DIST_MASK_PRECISE);		// Aplicar em fg e bg para ver os resultados?
	cv::normalize(markers, markers, 0, 1, cv::NORM_MINMAX,-1);

	cv::threshold(markers, markers, .4, 1., CV_THRESH_BINARY);

	// cv::Mat kernel1 = cv::Mat::ones(3, 3, CV_8UC1);
	// dilate(dst, dst, kernel1);
	// imshow("Peaks", dst);

	markers.convertTo(markers, CV_8U);
	outputMarkers = cv::Mat::zeros(markers.size(), CV_8UC3);

	cv::cvtColor(img, imgYCbCr, CV_BGR2YCrCb);
	cv::split(imgYCbCr, channels);

	cv::namedWindow("Marcadores de regiao", cv::WINDOW_NORMAL);

	do
	{
		hasSplit = false;

		contours.clear();
		cv::findContours(markers, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		// Encontrar centros de massa
		std::vector<cv::Moments> mu(contours.size());
		std::vector<cv::Point2f> newMassCenters(contours.size());

		for (int i = 0; i < contours.size(); i++)
		{
			// Calcula centro de massa da área
			mu[i] = cv::moments(contours[i], false);
			newMassCenters[i] = cv::Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
			cv::drawContours(markers, contours, i, cv::Scalar::all(255), -1);

			// Desenhar contornos para visualizacao
			cv::drawContours(outputMarkers, contours, i, cv::Scalar( cv::theRNG().uniform(1, 254), cv::theRNG().uniform(1, 254), cv::theRNG().uniform(1, 254) ), -1);
			cv::circle( outputMarkers, newMassCenters[i], 4, cv::Scalar(255, 255, 255), -1, 8, 0 );	// desenha centro de massa
			cv::putText( outputMarkers, std::to_string(i), newMassCenters[i] + cv::Point2f(-10, -10), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255) );	// desenha texto com índice da regiao
		}

		#ifdef _DEBUG
		cv::imshow("Marcadores de regiao", outputMarkers);
		cv::waitKey();
		#endif

		for (int i = 0; i < contours.size(); i++)
		{
			// Para ignorar regiões com poucos pixels
			if ((int)newMassCenters[i].x < 0 || (int)newMassCenters[i].y < 0)
				continue;

			// Procurar novo centro de massa na lista antiga
			int j;
			for (j = 0; j < massCenters.size(); j++)
			{
				if (massCenters[j] == newMassCenters[i])
					break;
			}
			// Nao achou o mesmo centro de massa, logo é uma nova regiao, tenta dividir ela
			if (j == massCenters.size())
			{
				#ifdef _DEBUG
				system("clear");
				std::cout << "*** BLOB " << i << " ***" << std::endl;
				#endif

				if ( SplitBlobs((int)newMassCenters[i].y, (int)newMassCenters[i].x) )
				{
					hasSplit = true;
					#ifdef _DEBUG
					cv::imshow("Coisa", markers);
					cv::waitKey();
					#endif
				}
			}
		}

		massCenters = newMassCenters;

	} while (hasSplit);
	
	return massCenters.size();
}


bool FruitFinder::SplitBlobs(int massCenterRow, int massCenterCol)
{
	bool hasSplit = false;

	// int massCenterRow = (int)newMassCenters[i].y; 
	// int massCenterCol = (int)newMassCenters[i].x;

//	8 direções   vetor dos valores
	std::vector< std::vector<int> > values(N_DIRECTIONS);
	std::vector< std::vector<int> > variation(N_DIRECTIONS);

	bool inRegion = true;

	// Preencher vetores de valores e variação (derivada) nas 8 direções enquanto uma das 8 estiver dentro da região
	for (int i = 0; inRegion == true; i++)
	{
		inRegion = false;
		for (int j = 0; j < N_DIRECTIONS; j++)
		{
			// Incrementos para x e y dependendo da direção
			int incX, incY;

			if (j == 7 || j == 0 || j == 1)
				incY = -1;
			else if (j == 2 || j == 6)
				incY = 0;
			else
				incY = 1;

			if (j == 1 || j == 2 || j == 3)
				incX = 1;
			else if (j == 0 || j == 4)
				incX = 0;
			else
				incX = -1;

			if( massCenterRow + (i*incY) >= 0 && massCenterRow + (i*incY) < markers.rows && 
				massCenterCol + (i*incX) >= 0 && massCenterCol + (i*incX) < markers.cols && 
				markers.at<uchar>(massCenterRow + (i*incY), massCenterCol + (i*incX)) != 0 )
			{
				// Insere valor do canal CHANNEL da imagem no vetor da direção i
				values[j].push_back( (int)channels[CHANNEL].at<uchar>(massCenterRow + (i*incY), massCenterCol + (i*incX)) );
				// Se não é o primeiro valor inserido, insere tbm derivada (variação do valor com relação ao anterior)
				if (i > 0)					// Derivada dos pontos para cima
					variation[j].push_back( values[j][i] - values[j][i-1] );
				inRegion = true;
			}
		}
	}

	// Vetor de maximos da derivada para cada direcao
	std::vector<double> maxValues(N_DIRECTIONS);
	std::vector<cv::Point> maxLoc(N_DIRECTIONS);

	double absoluteMax;
	int absMaxPos;
	cv::Point absMaxDir;
	bool overThresh;

	// Plotar gráficos e imprimir maximos encontrados
	std::string label;
	for (int i = 0; i < N_DIRECTIONS; i++)
	{
		switch(i)
		{
			case 0:
				label = "Up";
				break;
			case 1:
				label = "UpRight";
				break;
			case 2:
				label = "Right";
				break;
			case 3:
				label = "DownRight";
				break;
			case 4:
				label = "Down";
				break;
			case 5:
				label = "DownLeft";
				break;
			case 6:
				label = "Left";
				break;
			case 7:
				label = "UpLeft";
				break;
		}

		// Plotar valores e derivadas
		if ( values[i].size() <= 0 || variation[i].size() <= 0 )
			continue;
		CvPlot::plot("Valores", &values[i][0], values[i].size(), 1);
		CvPlot::label(label);
		CvPlot::plot("Derivada", &variation[i][0], variation[i].size(), 1);
		CvPlot::label(label);

		// Encontrar maximos da derivada em cada direção
		std::vector<int> absVariation (variation[i].size());
		for (int k = 0; k < variation[i].size(); k++)
			absVariation.push_back(abs(variation[i][k]));
		cv::minMaxLoc( absVariation, NULL, &maxValues[i], NULL, &maxLoc[i] );

		#ifdef _DEBUG
		std::cout << "Direcao " << label << ':' << std::endl
			<< "\tValor máximo : " << maxValues[i] << "  Local: " << maxLoc[i] << std::endl;
		#endif
	}

	// Encontrar direção que mais variou
	cv::minMaxLoc( maxValues, NULL, &absoluteMax, NULL, &absMaxDir );
	absMaxPos = maxLoc[ absMaxDir.x ].x;

	// Variação máxima acima do threshold, 
	if (abs(absoluteMax) > (double)thresh)
	{
		#ifdef _DEBUG
		std::cout << "Vai cortar na direcao " << absMaxDir.x << ", ponto " << absMaxPos << std::endl;
		#endif

		// definir direçoes de distanciamento dos dois pontos dependendo da direção q vai ser cortada
		int incX, incY;

		// incrementos para andar na direção do maximo valor e encontrar o ponto de corte
		if (absMaxDir.x == 7 || absMaxDir.x == 0 || absMaxDir.x == 1)
			incY = -1;
		else if (absMaxDir.x == 2 || absMaxDir.x == 6)
			incY = 0;
		else
			incY = 1;

		if (absMaxDir.x == 1 || absMaxDir.x == 2 || absMaxDir.x == 3)
			incX = 1;
		else if (absMaxDir.x == 0 || absMaxDir.x == 4)
			incX = 0;
		else
			incX = -1;

		// ponto de corte
		cv::Point2f cutPoint ( (float)(massCenterCol + (incX * absMaxPos)), (float)(massCenterRow + (incY * absMaxPos)) );
		cv::Point2f auxPoint = cutPoint;

		// Incrementos para distanciar os dois pontos na reta perpendicular à direção do valor máximo
		if (absMaxDir.x == 2 || absMaxDir.x == 6) // para a esquerda ou direita
			incX = 0;
		else
			incX = 1;

		if (absMaxDir.x == 0 || absMaxDir.x == 4) // para cima ou para baixo
			incY = 0;
		else if (absMaxDir.x == 3 || absMaxDir.x == 7) // baixo e direita ou cima e esquerda
			incY = -1;
		else
			incY = 1;

		#ifdef _DEBUG
		cv::Mat temp;
		markers.copyTo(temp);
		cv::circle( temp, cutPoint, 4, cv::Scalar(0), -1, 8, 0 );
		cv::imshow("cutPoint", temp);
		cv::waitKey();
		markers.copyTo(temp);

		std::cout << markers.at<uchar>(cutPoint.y, cutPoint.x) << std::endl;
		#endif
		// distancia os dois pontos enquanto estiverem dentro da regiao
		while ( cutPoint.x >= 0 && cutPoint.y >= 0 && cutPoint.x < markers.cols && cutPoint.y < markers.cols &&
			markers.at<uchar>(cutPoint.y, cutPoint.x) != 0 )
		{
			cutPoint.x += (float)incX;
			cutPoint.y += (float)incY;
			#ifdef _DEBUG
			cv::circle( temp, cutPoint, 4, cv::Scalar(0), -1, 8, 0 );
			cv::imshow("cutPoint", temp);
			cv::waitKey();
			markers.copyTo(temp);
			#endif
		}
		while ( auxPoint.x >= 0 && auxPoint.y >= 0 && auxPoint.x < markers.cols && auxPoint.y < markers.cols &&
			markers.at<uchar>(auxPoint.y, auxPoint.x) != 0 )
		{
			auxPoint.x -= (float)incX;
			auxPoint.y -= (float)incY;
		}

		// desenha linha separatória
		cv::line(markers, cutPoint, auxPoint, cv::Scalar(0), 2);

		hasSplit = true;
	}

	CvPlot::clear("Valores");
	CvPlot::clear("Derivada");

	return hasSplit;
}


