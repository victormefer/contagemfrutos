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

	// cv::Mat dist_8u;
	// dst.convertTo(dist_8u, CV_8U);
	markers.convertTo(markers, CV_8U);
	outputMarkers = cv::Mat::zeros(markers.size(), CV_8UC3);

	cv::cvtColor(img, imgYCbCr, CV_BGR2YCrCb);
	cv::split(imgYCbCr, channels);

	cv::namedWindow("Marcadores de regiao", cv::WINDOW_NORMAL);

	// cv::findContours(dist_8u, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	// // Create the marker image for the watershed algorithm
	// markers = cv::Mat::zeros(dst.size(), CV_8U);

	// std::vector<cv::Moments> mu(contours.size());
	// // Inicializa vetor de centros de massa
	// massCenters = std::vector<cv::Point2f>(contours.size());

	// // Generate random colors
	// for (size_t i = 0; i < contours.size(); i++)
	// {
	// 	int b = cv::theRNG().uniform(1, 254);
	// 	int g = cv::theRNG().uniform(1, 254);
	// 	int r = cv::theRNG().uniform(1, 254);
	// 	colors.push_back(cv::Vec3b((uchar)b, (uchar)g, (uchar)r));
	// }

	// // Draw the foreground markers
	// for (int i = 0; i < contours.size(); i++)
	// {
	// 	mu[i] = cv::moments(contours[i], false);

	// 	// Calcula centro de massa da área
	// 	massCenters[i] = cv::Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
	// 	cv::drawContours(markers, contours, i, cv::Scalar(colors[i])/*::all(i+1))*/, -1);
	// }

	// BeginBlobSplit();

	// outputMarkers.release();
	// markers.copyTo(outputMarkers);

	// for (int i = 0; i < massCenters.size(); i++)
	// {
	// 	// Desenha ponto no centro de massa
	// 	cv::circle( outputMarkers, massCenters[i], 4, cv::Scalar(255, 255, 255), -1, 8, 0 );

	// 	// Desenha texto com o indice da região
	// 	cv::putText( outputMarkers, std::to_string(i), massCenters[i] + cv::Point2f(-10, -10), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255) );
	// }


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

		cv::imshow("Marcadores de regiao", outputMarkers);
		#ifdef _DEBUG
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

/*
void FruitFinder::BeginBlobSplit()
{
	cv::split(imgYCbCr, channels);

	bool hasSplit;

	cv::namedWindow("Marcadores de regiao", cv::WINDOW_NORMAL);

	// // // Pegar valores de Y, Cb e Cr nas 8 direções de cada região encontrada
	// for (int i = 0; i < massCenters.size(); i++)
	// {
	// 	// Para ignorar regiões com poucos pixels
	// 	if ((int)massCenters[i].x < 0 || (int)massCenters[i].y < 0)
	// 		continue;

	// 	if( SplitBlobs((int)massCenters[i].y, (int)massCenters[i].x) )
	// 		hasSplit = true;
	// }

	// while (hasSplit)
	// {
	// 	hasSplit = false;

	// 	cv::Mat markers8u;
	// 	markers.convertTo(markers8u, CV_8UC1);

	// 	contours.clear();
	// 	cv::findContours(markers8u, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	// 	std::vector<cv::Moments> mu(contours.size());
	// 	// Inicializa vetor de centros de massa
	// 	std::vector<cv::Point2f> newMassCenters(contours.size());

	// 	for (int i = 0; i < contours.size(); i++)
	// 	{
	// 		// Calcula centro de massa da área
	// 		mu[i] = cv::moments(contours[i], false);
	// 		newMassCenters[i] = cv::Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );

	// 		// Para ignorar regiões com poucos pixels
	// 		if ((int)newMassCenters[i].x < 0 || (int)newMassCenters[i].y < 0)
	// 			continue;

	// 		int j;
	// 		// Procurar novo centro de massa na lista antiga
	// 		for (j = 0; j < massCenters.size(); j++)
	// 		{
	// 			if (massCenters[j] == newMassCenters[i])
	// 				break;
	// 		}

	// 		// Nao achou, nova regiao, tenta dividir ela
	// 		if (j == massCenters.size())
	// 		{
	// 			if ( SplitBlobs((int)newMassCenters[i].y, (int)newMassCenters[i].x) )
	// 				hasSplit = true;
	// 		}
	// 	}

	// 	massCenters = newMassCenters;

	// 	cv::imshow("Marcadores de regiao", markers);
	// 	#ifdef _DEBUG
	// 	cv::waitKey();
	// 	#endif

	// }

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
			cv::drawContours(markers, contours, i, cv::Scalar::all(255)), -1);

			// Desenhar contornos para visualizacao
			cv::drawContours(outputMarkers, contours, i, cv::Scalar( cv::theRNG().uniform(1, 254), cv::theRNG().uniform(1, 254), cv::theRNG().uniform(1, 254) ), -1);
			cv::circle( outputMarkers, massCenters[i], 4, cv::Scalar(255, 255, 255), -1, 8, 0 );	// desenha centro de massa
			cv::putText( outputMarkers, std::to_string(i), massCenters[i] + cv::Point2f(-10, -10), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255) );	// desenha texto com índice da regiao
		}

		cv::imshow("Marcadores de regiao", outputMarkers);
		#ifdef _DEBUG
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
				if ( SplitBlobs((int)newMassCenters[i].y, (int)newMassCenters[i].x) )
					hasSplit = true;
			}
		}

		massCenters = newMassCenters;

	} while (hasSplit);

}*/


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
	if (absoluteMax > thresh)
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
		cv::Point2i cutPoint ( massCenterCol + (incX * absMaxPos), massCenterRow + (incY * absMaxPos) );
		cv::Point2i auxPoint = cutPoint;

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

		// distancia os dois pontos enquanto estiverem dentro da regiao
		while ( cutPoint.x >= 0 && cutPoint.y >= 0 && cutPoint.x < markers.cols && cutPoint.y < markers.cols &&
			markers.at<uchar>(cutPoint.y, cutPoint.x) != 0 )
		{
			cutPoint.x += (float)incX;
			cutPoint.y += (float)incY;
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

/*	bool hasSplit = false;

//  8 direções    canais ycbcr      valores
	std::vector < std::vector < std::vector<int> > > values(N_DIRECTIONS);			// Valores dos 3 canais nas 8 direções
	std::vector < std::vector < std::vector<int> > > variation(N_DIRECTIONS);		// Derivada dos valores
	std::vector< std::vector<double> > minVal(N_DIRECTIONS);						// Valor minimo
	std::vector< std::vector<double> > maxVal(N_DIRECTIONS);						// Valor maximo
	std::vector< std::vector<cv::Point> > minLoc(N_DIRECTIONS);						// Minimo local 
	std::vector< std::vector<cv::Point> > maxLoc(N_DIRECTIONS);						// Maximo local

	// Inicializar vetores de valores, derivadas, ranges, minimos e maximos locais
	for (int j = 0; j < N_DIRECTIONS; j++)
	{
		values[j] = std::vector<std::vector<int>>(3);
		variation[j] = std::vector<std::vector<int>>(3);
		minVal[j] = std::vector<double>(3);
		maxVal[j] = std::vector<double>(3);
		minLoc[j] = std::vector<cv::Point>(3);
		maxLoc[j] = std::vector<cv::Point>(3);
	}

	int k = 0;
	while(1)
	{
		bool inRegion = false;
		// Plotted points in up direction (values[0])
		if ( massCenterRow - k >= 0 && markers.at<uchar>(massCenterRow - k, massCenterCol) != 0 )
		{
			values[0][0].push_back( (int)channels[0].at<uchar>(massCenterRow - k, massCenterCol) );
			values[0][1].push_back( (int)channels[1].at<uchar>(massCenterRow - k, massCenterCol) );
			values[0][2].push_back( (int)channels[2].at<uchar>(massCenterRow - k, massCenterCol) );
			if (k > 0)					// Derivada dos pontos para cima
			{
				variation[0][0].push_back( values[0][0][k] - values[0][0][k-1] );
				variation[0][1].push_back( values[0][1][k] - values[0][1][k-1] );
				variation[0][2].push_back( values[0][2][k] - values[0][2][k-1] );
			}
			inRegion = true;
		}
		// Plotted points in upright direction
		if ( massCenterRow - k >= 0 && massCenterCol + k < imgYCbCr.cols && 
			markers.at<uchar>(massCenterRow - k, massCenterCol) != 0 )
		{
			values[1][0].push_back( (int)channels[0].at<uchar>(massCenterRow - k, massCenterCol + k) );
			values[1][1].push_back( (int)channels[1].at<uchar>(massCenterRow - k, massCenterCol + k) );
			values[1][2].push_back( (int)channels[2].at<uchar>(massCenterRow - k, massCenterCol + k) );
			if (k > 0)					// Derivada dos pontos para cima e direita
			{
				variation[1][0].push_back( values[1][0][k] - values[1][0][k-1] );
				variation[1][1].push_back( values[1][1][k] - values[1][1][k-1] );
				variation[1][2].push_back( values[1][2][k] - values[1][2][k-1] );
			}
			inRegion = true;
		}
		// Plotted points in right direction
		if ( massCenterCol + k < imgYCbCr.cols && markers.at<uchar>(massCenterRow, massCenterCol + k) != 0 )
		{
			values[2][0].push_back( (int)channels[0].at<uchar>(massCenterRow, massCenterCol + k) );
			values[2][1].push_back( (int)channels[1].at<uchar>(massCenterRow, massCenterCol + k) );
			values[2][2].push_back( (int)channels[2].at<uchar>(massCenterRow, massCenterCol + k) );
			if (k > 0)					// Derivada dos pontos para a direita
			{
				variation[2][0].push_back( values[2][0][k] - values[2][0][k-1] );
				variation[2][1].push_back( values[2][1][k] - values[2][1][k-1] );
				variation[2][2].push_back( values[2][2][k] - values[2][2][k-1] );
			}
			inRegion = true;
		}
		// Plotted points in downright direction
		if ( massCenterCol + k < imgYCbCr.cols  && massCenterRow + k < imgYCbCr.rows && 
			markers.at<uchar>(massCenterRow + k, massCenterCol + k) != 0 )
		{
			values[3][0].push_back( (int)channels[0].at<uchar>(massCenterRow + k, massCenterCol + k) );
			values[3][1].push_back( (int)channels[1].at<uchar>(massCenterRow + k, massCenterCol + k) );
			values[3][2].push_back( (int)channels[2].at<uchar>(massCenterRow + k, massCenterCol + k) );
			if (k > 0)					// Derivada dos pontos para baixo e direita
			{
				variation[3][0].push_back( values[3][0][k] - values[3][0][k-1] );
				variation[3][1].push_back( values[3][1][k] - values[3][1][k-1] );
				variation[3][2].push_back( values[3][2][k] - values[3][2][k-1] );
			}
			inRegion = true;
		}
		// Plotted points in down direction
		if ( massCenterRow + k < imgYCbCr.rows && markers.at<uchar>(massCenterRow + k, massCenterCol) != 0 )
		{
			values[4][0].push_back( (int)channels[0].at<uchar>(massCenterRow + k, massCenterCol) );
			values[4][1].push_back( (int)channels[1].at<uchar>(massCenterRow + k, massCenterCol) );
			values[4][2].push_back( (int)channels[2].at<uchar>(massCenterRow + k, massCenterCol) );
			if (k > 0)					// Derivada dos pontos para baixo
			{
				variation[4][0].push_back( values[4][0][k] - values[4][0][k-1] );
				variation[4][1].push_back( values[4][1][k] - values[4][1][k-1] );
				variation[4][2].push_back( values[4][2][k] - values[4][2][k-1] );
			}
			inRegion = true;
		}
		// Plotted points in downleft direction
		if ( massCenterRow + k < imgYCbCr.rows && massCenterCol - k >= 0 && 
			markers.at<uchar>(massCenterRow + k, massCenterCol - k) != 0 )
		{
			values[5][0].push_back( (int)channels[0].at<uchar>(massCenterRow + k, massCenterCol - k) );
			values[5][1].push_back( (int)channels[1].at<uchar>(massCenterRow + k, massCenterCol - k) );
			values[5][2].push_back( (int)channels[2].at<uchar>(massCenterRow + k, massCenterCol - k) );
			if (k > 0)					// Derivada dos pontos para baixo e esquerda
			{
				variation[5][0].push_back( values[5][0][k] - values[5][0][k-1] );
				variation[5][1].push_back( values[5][1][k] - values[5][1][k-1] );
				variation[5][2].push_back( values[5][2][k] - values[5][2][k-1] );
			}
			inRegion = true;
		}
		// Plotted points in left direction
		if ( massCenterCol - k >= 0 && markers.at<uchar>(massCenterRow, massCenterCol - k) != 0 )
		{
			values[6][0].push_back( (int)channels[0].at<uchar>(massCenterRow, massCenterCol - k) );
			values[6][1].push_back( (int)channels[1].at<uchar>(massCenterRow, massCenterCol - k) );
			values[6][2].push_back( (int)channels[2].at<uchar>(massCenterRow, massCenterCol - k) );
			if (k > 0)					// Derivada dos pontos para a esquerda
			{
				variation[6][0].push_back( values[6][0][k] - values[6][0][k-1] );
				variation[6][1].push_back( values[6][1][k] - values[6][1][k-1] );
				variation[6][2].push_back( values[6][2][k] - values[6][2][k-1] );
			}
			inRegion = true;
		}
		// Plotted points in upleft direction
		if ( massCenterCol - k >= 0 && massCenterRow - k >= 0 && 
			markers.at<uchar>(massCenterRow - k, massCenterCol - k) != 0 )
		{
			values[7][0].push_back( (int)channels[0].at<uchar>(massCenterRow - k, massCenterCol - k) );
			values[7][1].push_back( (int)channels[1].at<uchar>(massCenterRow - k, massCenterCol - k) );
			values[7][2].push_back( (int)channels[2].at<uchar>(massCenterRow - k, massCenterCol - k) );
			if (k > 0)					// Derivada dos pontos para cima e esquerda
			{
				variation[7][0].push_back( values[7][0][k] - values[7][0][k-1] );
				variation[7][1].push_back( values[7][1][k] - values[7][1][k-1] );
				variation[7][2].push_back( values[7][2][k] - values[7][2][k-1] );
			}
			inRegion = true;
		}
		k++;
		if (!inRegion)
			break;
	}
	
	system("clear");

	// #ifdef _DEBUG
	// std::cout << "*** BLOB " << i << " ***" << std::endl;
	// #endif

	std::string plotName, label;
	std::vector<double> minOfMax(3), maxOfMax(3);
	std::vector<cv::Point> minOfMaxLoc(3), maxOfMaxLoc(3);
	double absoluteMax;
	int absMaxPos, absMaxDir;
	bool overThresh;

	// Plotar gráficos e imprimir maximos encontrados

	// for(int j = 0; j < 3; j++)
	// {
	int j = 0; // CORTAR SEMPRE NO Y
		switch(j)
		{
			case 0:
			plotName = "Y";
			break;
			case 1:
			plotName = "Cr";
			break;
			case 2:
			plotName = "Cb";
			break;
		}

		#ifdef _DEBUG
		std::cout << " - CANAL " << plotName << ':' << std::endl;
		#endif

		overThresh = false;

		for(int l = 0; l < N_DIRECTIONS; l++)
		{
			switch(l)
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
			if ( values[l][j].size() <= 0 || variation[l][j].size() <= 0 )
				continue;
			CvPlot::plot(plotName, &values[l][j][0], values[l][j].size(), 1);
			CvPlot::label(label);
			CvPlot::plot(plotName + " variation", &variation[l][j][0], variation[l][j].size(), 1);
			CvPlot::label(label);

			// Encontrar maximos e minimos das derivadas
			std::vector<int> absVariation (variation[l][j].size());
			for (int k = 0; k < variation[l][j].size(); k++)
				absVariation.push_back(abs(variation[l][j][k]));
			cv::minMaxLoc( absVariation, NULL, &maxVal[l][j], NULL, &maxLoc[l][j] );

			if(maxVal[l][j] > tresh || minVal[l][j] < -tresh)
			{
				#ifdef _DEBUG
				std::cout << '\t' << label << ':' << std::endl;
				std::cout << "\tminVal: " << minVal[l][j] << " maxVal: " << maxVal[l][j] << " minLoc: " <<
				minLoc[l][j] << " maxLoc: " << maxLoc[l][j] << std::endl;
				#endif
				overThresh = true;
			}
		}
		#ifdef _DEBUG
		std::cout << std::endl;
		#endif

		// Encontrar direcao q mais variou
		// cv::minMaxLoc( minVal[j], &minOfMax[j], NULL, &minOfMaxLoc[j], NULL );
		cv::minMaxLoc( maxVal[j], NULL, &maxOfMax[j], NULL, &maxOfMaxLoc[j] );
		// if (abs(minOfMax[j]) > abs(maxOfMax[j]))
		// {
		// 	absoluteMax = minOfMax[j];
		// 	absMaxPos = minLoc[ minOfMaxLoc[j].x ][j].x;
		// 	absMaxDir = minOfMaxLoc[j].x;
		// }
		// else
		// {
			absoluteMax = maxOfMax[j];
			absMaxPos = maxLoc[ maxOfMaxLoc[j].x ][j].x;
			absMaxDir = maxOfMaxLoc[j].x;
		// }

		// Tem uma direcao que muda drasticamente de valor, acima do threshold
		// Cortar região a partir desse ponto de maior mudança
		if (overThresh)
		{
			#ifdef _DEBUG
			std::cout << "Canal " << plotName << " vai cortar na direcao " << absMaxDir << ", ponto " << absMaxPos << std::endl;
			#endif

			// definir direçoes de distanciamento dos dois pontos dependendo da direção q vai ser cortada
			int incX, incY;
			if (absMaxDir == 2 || absMaxDir == 6) // para a esquerda ou direita
				incX = 0;
			else
				incX = 1;

			if (absMaxDir == 0 || absMaxDir == 4) // para cima ou para baixo
				incY = 0;
			else if (absMaxDir == 3 || absMaxDir == 7) // baixo e direita ou cima e esquerda
				incY = -1;
			else
				incY = 1;

			// ponto de corte
			cv::Point2f cutPoint ( massCenterCol + (incX * absMaxPos), massCenterRow + (incY * absMaxPos) );
			cv::Point2f auxPoint = cutPoint;

			// distancia os dois pontos enquanto estiverem dentro da regiao
			while ( cutPoint.x >= 0 && cutPoint.y >= 0 && cutPoint.x < markers.cols && cutPoint.y < markers.cols &&
				markers.at<uchar>(cutPoint.y, cutPoint.x) != 0 )
			{
				cutPoint.x += (float)incX;
				cutPoint.y += (float)incY;
			}
			while ( auxPoint.x >= 0 && auxPoint.y >= 0 && auxPoint.x < markers.cols && auxPoint.y < markers.cols &&
				markers.at<uchar>(auxPoint.y, auxPoint.x) != 0 )
			{
				auxPoint.x -= (float)incX;
				auxPoint.y -= (float)incY;
			}

			// desenha linha separatória
			cv::line(markers, cutPoint, auxPoint, cv::Scalar::all(0), 2);

			hasSplit = true;
		}
	// }

	CvPlot::clear("Y");
	CvPlot::clear("Cr");
	CvPlot::clear("Cb");
	CvPlot::clear("Y variation");
	CvPlot::clear("Cr variation");
	CvPlot::clear("Cb variation");

	return hasSplit;*/
}


