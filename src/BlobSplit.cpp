#include "BlobSplit.h"


void BlobSplit::SplitBlobs(cv::Mat img, cv::Mat& markers, std::vector<cv::Point2f> massCenters)
{
	int blob=1;


	cv::Mat imgYCbCr;
	cv::cvtColor(img, imgYCbCr, CV_BGR2YCrCb);

	cv::Mat channels[3];
	cv::split(imgYCbCr, channels);

	// Pegar valores de Y, Cb e Cr nas 8 direções de cada região encontrada
	for (int i = 0; i < massCenters.size(); i++)
	{
		// Para ignorar regiões com poucos pixels
		if ((int)massCenters[i].x < 0 || (int)massCenters[i].y < 0)
			continue;


//      8 direções    canais ycbcr      valores
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
			if ( (int)massCenters[i].y - k >= 0 && markers.at<cv::Vec3b>((int)massCenters[i].y - k, (int)massCenters[i].x) != cv::Vec3b(0,0,0) )
			{
				values[0][0].push_back( (int)channels[0].at<uchar>((int)massCenters[i].y - k, (int)massCenters[i].x) );
				values[0][1].push_back( (int)channels[1].at<uchar>((int)massCenters[i].y - k, (int)massCenters[i].x) );
				values[0][2].push_back( (int)channels[2].at<uchar>((int)massCenters[i].y - k, (int)massCenters[i].x) );
				if (k > 0)					// Derivada dos pontos para cima
				{
					variation[0][0].push_back( values[0][0][k] - values[0][0][k-1] );
					variation[0][1].push_back( values[0][1][k] - values[0][1][k-1] );
					variation[0][2].push_back( values[0][2][k] - values[0][2][k-1] );
				}
				inRegion = true;
			}

			// Plotted points in upright direction
			if ( (int)massCenters[i].y - k >= 0 && (int)massCenters[i].x + k < img.cols && 
				markers.at<cv::Vec3b>((int)massCenters[i].y - k, (int)massCenters[i].x) != cv::Vec3b(0,0,0) )
			{
				values[1][0].push_back( (int)channels[0].at<uchar>((int)massCenters[i].y - k, (int)massCenters[i].x + k) );
				values[1][1].push_back( (int)channels[1].at<uchar>((int)massCenters[i].y - k, (int)massCenters[i].x + k) );
				values[1][2].push_back( (int)channels[2].at<uchar>((int)massCenters[i].y - k, (int)massCenters[i].x + k) );
				if (k > 0)					// Derivada dos pontos para cima e direita
				{
					variation[1][0].push_back( values[1][0][k] - values[1][0][k-1] );
					variation[1][1].push_back( values[1][1][k] - values[1][1][k-1] );
					variation[1][2].push_back( values[1][2][k] - values[1][2][k-1] );
				}
				inRegion = true;
			}

			// Plotted points in right direction
			if ( (int)massCenters[i].x + k < img.cols && markers.at<cv::Vec3b>((int)massCenters[i].y, (int)massCenters[i].x + k) != cv::Vec3b(0,0,0) )
			{
				values[2][0].push_back( (int)channels[0].at<uchar>((int)massCenters[i].y, (int)massCenters[i].x + k) );
				values[2][1].push_back( (int)channels[1].at<uchar>((int)massCenters[i].y, (int)massCenters[i].x + k) );
				values[2][2].push_back( (int)channels[2].at<uchar>((int)massCenters[i].y, (int)massCenters[i].x + k) );
				if (k > 0)					// Derivada dos pontos para a direita
				{
					variation[2][0].push_back( values[2][0][k] - values[2][0][k-1] );
					variation[2][1].push_back( values[2][1][k] - values[2][1][k-1] );
					variation[2][2].push_back( values[2][2][k] - values[2][2][k-1] );
				}
				inRegion = true;
			}

			// Plotted points in downright direction
			if ( (int)massCenters[i].x + k < img.cols  && (int)massCenters[i].y + k < img.rows && 
				markers.at<cv::Vec3b>((int)massCenters[i].y + k, (int)massCenters[i].x + k) != cv::Vec3b(0,0,0) )
			{
				values[3][0].push_back( (int)channels[0].at<uchar>((int)massCenters[i].y + k, (int)massCenters[i].x + k) );
				values[3][1].push_back( (int)channels[1].at<uchar>((int)massCenters[i].y + k, (int)massCenters[i].x + k) );
				values[3][2].push_back( (int)channels[2].at<uchar>((int)massCenters[i].y + k, (int)massCenters[i].x + k) );
				if (k > 0)					// Derivada dos pontos para baixo e direita
				{
					variation[3][0].push_back( values[3][0][k] - values[3][0][k-1] );
					variation[3][1].push_back( values[3][1][k] - values[3][1][k-1] );
					variation[3][2].push_back( values[3][2][k] - values[3][2][k-1] );
				}
				inRegion = true;
			}

			// Plotted points in down direction
			if ( (int)massCenters[i].y + k < img.rows && markers.at<cv::Vec3b>((int)massCenters[i].y + k, (int)massCenters[i].x) != cv::Vec3b(0,0,0) )
			{
				values[4][0].push_back( (int)channels[0].at<uchar>((int)massCenters[i].y + k, (int)massCenters[i].x) );
				values[4][1].push_back( (int)channels[1].at<uchar>((int)massCenters[i].y + k, (int)massCenters[i].x) );
				values[4][2].push_back( (int)channels[2].at<uchar>((int)massCenters[i].y + k, (int)massCenters[i].x) );
				if (k > 0)					// Derivada dos pontos para baixo
				{
					variation[4][0].push_back( values[4][0][k] - values[4][0][k-1] );
					variation[4][1].push_back( values[4][1][k] - values[4][1][k-1] );
					variation[4][2].push_back( values[4][2][k] - values[4][2][k-1] );
				}
				inRegion = true;
			}

			// Plotted points in downleft direction
			if ( (int)massCenters[i].y + k < img.rows && (int)massCenters[i].x - k >= 0 && 
				markers.at<cv::Vec3b>((int)massCenters[i].y + k, (int)massCenters[i].x - k) != cv::Vec3b(0,0,0) )
			{
				values[5][0].push_back( (int)channels[0].at<uchar>((int)massCenters[i].y + k, (int)massCenters[i].x - k) );
				values[5][1].push_back( (int)channels[1].at<uchar>((int)massCenters[i].y + k, (int)massCenters[i].x - k) );
				values[5][2].push_back( (int)channels[2].at<uchar>((int)massCenters[i].y + k, (int)massCenters[i].x - k) );
				if (k > 0)					// Derivada dos pontos para baixo e esquerda
				{
					variation[5][0].push_back( values[5][0][k] - values[5][0][k-1] );
					variation[5][1].push_back( values[5][1][k] - values[5][1][k-1] );
					variation[5][2].push_back( values[5][2][k] - values[5][2][k-1] );
				}
				inRegion = true;
			}

			// Plotted points in left direction
			if ( (int)massCenters[i].x - k >= 0 && markers.at<cv::Vec3b>((int)massCenters[i].y, (int)massCenters[i].x - k) != cv::Vec3b(0,0,0) )
			{
				values[6][0].push_back( (int)channels[0].at<uchar>((int)massCenters[i].y, (int)massCenters[i].x - k) );
				values[6][1].push_back( (int)channels[1].at<uchar>((int)massCenters[i].y, (int)massCenters[i].x - k) );
				values[6][2].push_back( (int)channels[2].at<uchar>((int)massCenters[i].y, (int)massCenters[i].x - k) );
				if (k > 0)					// Derivada dos pontos para a esquerda
				{
					variation[6][0].push_back( values[6][0][k] - values[6][0][k-1] );
					variation[6][1].push_back( values[6][1][k] - values[6][1][k-1] );
					variation[6][2].push_back( values[6][2][k] - values[6][2][k-1] );
				}
				inRegion = true;
			}

			// Plotted points in upleft direction
			if ( (int)massCenters[i].x - k >= 0 && (int)massCenters[i].y - k >= 0 && 
				markers.at<cv::Vec3b>((int)massCenters[i].y - k, (int)massCenters[i].x - k) != cv::Vec3b(0,0,0) )
			{
				values[7][0].push_back( (int)channels[0].at<uchar>((int)massCenters[i].y - k, (int)massCenters[i].x - k) );
				values[7][1].push_back( (int)channels[1].at<uchar>((int)massCenters[i].y - k, (int)massCenters[i].x - k) );
				values[7][2].push_back( (int)channels[2].at<uchar>((int)massCenters[i].y - k, (int)massCenters[i].x - k) );
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

		// // Calculo dos valores minimos e maximos e seus pontos nas quatro direcoes
		// for (int j = 0; j < N_DIRECTIONS; j++)
		// {
		// 	for (int l = 0; l < 3; l++)
		// 	{
		// 		cv::minMaxLoc(variation[j][l], &minVal[j][l], &maxVal[j][l], &minLoc[j][l], &maxLoc[j][l], cv::Mat());
		// 	}
		// }

		int tresh = 8;			// Patamar para testar as derivadas. Definir um valor para cada componente Y, Cb e Cr?
		
		system("clear");
		std::cout << "- Blob " << i << " -" << std::endl;

		std::string plotName, label;

		std::vector<double> minOfMax, maxOfMax;
		std::vector<cv::Point> minOfMaxLoc, maxOfMaxLoc;

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
			std::cout << plotName << ':' << std::endl;
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
				cv::minMaxLoc( variation[l][j], &minVal[l][j], &maxVal[l][j], &minLoc[l][j], &maxLoc[l][j], cv::Mat() );

				if(maxVal[l][j] > tresh || minVal[l][j] < -tresh)
				{
					std::cout << '\t' << label << ':' << std::endl;
					std::cout << "minVal: " << minVal[l][j] << " maxVal: " << maxVal[l][j] << " minLoc: " <<
							 minLoc[l][j] << " maxLoc: " << maxLoc[l][j] << std::endl;
				}
			}
			std::cout << std::endl;

			// cv::minMaxLoc( minVal[j], &minOfMax[j], NULL, &minOfMaxLoc[j], NULL, cv::Mat() );
			// cv::minMaxLoc( maxVal[j], NULL, &maxOfMax[j], NULL, &maxOfMaxLoc[j], cv::Mat() );
		}

		// 1. Escolher um range de valores para checar se o maximo esta dentro de um patamar
		// 2. Localizado um ponto acima do patamar, marcar na imagem a regiao de divisao das frutas observando as quatro direcoes

		cv::waitKey();

		CvPlot::clear("Y");
		CvPlot::clear("Cr");
		CvPlot::clear("Cb");
		CvPlot::clear("Y variation");
		CvPlot::clear("Cr variation");
		CvPlot::clear("Cb variation");
	}


}