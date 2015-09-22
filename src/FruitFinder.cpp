#include "FruitFinder.h"

cv::Mat FruitFinder::imgYCbCr, FruitFinder::markers, FruitFinder::channels[3];
std::vector<cv::Point2f> FruitFinder::massCenters;
std::vector<std::vector<cv::Point>> FruitFinder::contours;


int FruitFinder::FindFruits(cv::Mat img, cv::Mat mask, cv::Mat& outputMarkers)
{
	cv::cvtColor(img, imgYCbCr, CV_BGR2YCrCb);
	cv::Mat dst;
	cv::distanceTransform(mask, dst, CV_DIST_L2, CV_DIST_MASK_PRECISE);		// Aplicar em fg e bg para ver os resultados?
	cv::normalize(dst, dst, 0, 1, cv::NORM_MINMAX,-1);

	cv::threshold(dst, dst, .4, 1., CV_THRESH_BINARY);

	// cv::Mat kernel1 = cv::Mat::ones(3, 3, CV_8UC1);
	// dilate(dst, dst, kernel1);
	// imshow("Peaks", dst);

	cv::Mat dist_8u;
	dst.convertTo(dist_8u, CV_8U);

	cv::findContours(dist_8u, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	// Create the marker image for the watershed algorithm
	markers = cv::Mat::zeros(dst.size(), CV_8UC3);

	std::vector<cv::Moments> mu(contours.size());
	// Inicializa vetor de centros de massa
	massCenters = std::vector<cv::Point2f>(contours.size());

	// Generate random colors
	vector<cv::Vec3b> colors;
	for (size_t i = 0; i < contours.size(); i++)
	{
		int b = cv::theRNG().uniform(1, 254);
		int g = cv::theRNG().uniform(1, 254);
		int r = cv::theRNG().uniform(1, 254);
		colors.push_back(cv::Vec3b((uchar)b, (uchar)g, (uchar)r));
	}

	// Draw the foreground markers
	for (int i = 0; i < contours.size(); i++)
	{
		mu[i] = cv::moments(contours[i], false);

		// Calcula centro de massa da área
		massCenters[i] = cv::Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
		cv::drawContours(markers, contours, i, cv::Scalar(colors[i])/*::all(i+1))*/, -1);
	}

	BeginBlobSplit();

	outputMarkers.release();
	markers.copyTo(outputMarkers);

	for (int i = 0; i < massCenters.size(); i++)
	{
		// Desenha ponto no centro de massa
		cv::circle( outputMarkers, massCenters[i], 4, cv::Scalar(255, 255, 255), -1, 8, 0 );

		// Desenha texto com o indice da região
		cv::putText( outputMarkers, std::to_string(i), massCenters[i] + cv::Point2f(-10, -10), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255) );
	}
	
	return massCenters.size();
}


void FruitFinder::BeginBlobSplit()
{
	cv::split(imgYCbCr, channels);

	bool hasSplit = false;;

	// // Pegar valores de Y, Cb e Cr nas 8 direções de cada região encontrada
	for (int i = 0; i < massCenters.size(); i++)
	{
		// Para ignorar regiões com poucos pixels
		if ((int)massCenters[i].x < 0 || (int)massCenters[i].y < 0)
			continue;

		if( SplitBlobs((int)massCenters[i].y, (int)massCenters[i].x) )
			hasSplit = true;
	}

	while (hasSplit)
	{
		hasSplit = false;

		cv::Mat markers8u;
		markers.convertTo(markers8u, CV_8U);

		contours.clear();
		cv::findContours(markers8u, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		std::vector<cv::Moments> mu(contours.size());
		// Inicializa vetor de centros de massa
		std::vector<cv::Point2f> newMassCenters(contours.size());

		for (int i = 0; i < contours.size(); i++)
		{
			// Calcula centro de massa da área
			mu[i] = cv::moments(contours[i], false);
			newMassCenters[i] = cv::Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );

			// Para ignorar regiões com poucos pixels
			if ((int)newMassCenters[i].x < 0 || (int)newMassCenters[i].y < 0)
				continue;

			int j;
			// Procurar novo centro de massa na lista antiga
			for (j = 0; j < massCenters.size(); j++)
			{
				if (massCenters[j] == newMassCenters[i])
					break;
			}

			// Nao achou, nova regiao, tenta dividir ela
			if (j == massCenters.size())
			{
				if ( SplitBlobs((int)newMassCenters[i].y, (int)newMassCenters[i].x) )
					hasSplit = true;
			}
		}

		massCenters = newMassCenters;
	}
}


bool FruitFinder::SplitBlobs(int massCenterRow, int massCenterCol)
{
	bool hasSplit = false;

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
		if ( massCenterRow - k >= 0 && markers.at<cv::Vec3b>(massCenterRow - k, massCenterCol) != cv::Vec3b(0,0,0) )
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
			markers.at<cv::Vec3b>(massCenterRow - k, massCenterCol) != cv::Vec3b(0,0,0) )
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
		if ( massCenterCol + k < imgYCbCr.cols && markers.at<cv::Vec3b>(massCenterRow, massCenterCol + k) != cv::Vec3b(0,0,0) )
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
			markers.at<cv::Vec3b>(massCenterRow + k, massCenterCol + k) != cv::Vec3b(0,0,0) )
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
		if ( massCenterRow + k < imgYCbCr.rows && markers.at<cv::Vec3b>(massCenterRow + k, massCenterCol) != cv::Vec3b(0,0,0) )
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
			markers.at<cv::Vec3b>(massCenterRow + k, massCenterCol - k) != cv::Vec3b(0,0,0) )
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
		if ( massCenterCol - k >= 0 && markers.at<cv::Vec3b>(massCenterRow, massCenterCol - k) != cv::Vec3b(0,0,0) )
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
			markers.at<cv::Vec3b>(massCenterRow - k, massCenterCol - k) != cv::Vec3b(0,0,0) )
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

	int tresh = 8;			// Patamar para testar as derivadas. Definir um valor para cada componente Y, Cb e Cr?
	
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
	for(int j = 0; j < 3; j++)
	{
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
		if (overThresh)
		{
			// hasSplit = true;

			#ifdef _DEBUG
			std::cout << "Canal " << plotName << " vai cortar na direcao " << absMaxDir << ", ponto " << absMaxPos << std::endl;
			#endif

			// pega maximo entre minimo e maximo das 2 retas imediatamente proximas à reta do maximo
			
		}
	}
	// 1. Escolher um range de valores para checar se o maximo esta dentro de um patamar
	// 2. Localizado um ponto acima do patamar, marcar na imagem a regiao de divisao das frutas observando as quatro direcoes
	#ifdef _DEBUG
	cv::waitKey();
	#endif

	CvPlot::clear("Y");
	CvPlot::clear("Cr");
	CvPlot::clear("Cb");
	CvPlot::clear("Y variation");
	CvPlot::clear("Cr variation");
	CvPlot::clear("Cb variation");

	return hasSplit;
}


