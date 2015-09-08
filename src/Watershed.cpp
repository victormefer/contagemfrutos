#include "Watershed.h"

using namespace cv;

template <typename T>
cv::Mat plotGraph(std::vector<T>& vals, int YRange[2])
{

	auto it = minmax_element(vals.begin(), vals.end());
	float scale = 1./ceil(*it.second - *it.first); 
	float bias = *it.first;
	int rows = YRange[1] - YRange[0] + 1;
	cv::Mat image = Mat::zeros( rows, vals.size(), CV_8UC3 );
	image.setTo(0);
	for (int i = 0; i < (int)vals.size()-1; i++)
	{
		cv::line(image, cv::Point(i, rows - 1 - (vals[i] - bias)*scale*YRange[1]), cv::Point(i+1, rows - 1 - (vals[i+1] - bias)*scale*YRange[1]), Scalar(255, 0, 0), 1);
	}

	return image;
}

void Watershed::FindWatershed2(Mat img, Mat mask, Mat& wshed)
{
	Mat dst;
	distanceTransform(mask,dst,CV_DIST_L2,CV_DIST_MASK_PRECISE);		// Aplicar em fg e bg para ver os resultados?
	normalize(dst,dst,0,1, NORM_MINMAX,-1);

	threshold(dst, dst, .4, 1., CV_THRESH_BINARY);

	// Mat kernel1 = Mat::ones(3, 3, CV_8UC1);
	// dilate(dst, dst, kernel1);
	// imshow("Peaks", dst);

	Mat dist_8u;
	dst.convertTo(dist_8u, CV_8U);

	std::vector<std::vector<Point>> contours;
	findContours(dist_8u, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);


	// Create the marker image for the watershed algorithm
	Mat markers = Mat::zeros(dst.size(), CV_32SC1);


	std::vector<Moments> mu(contours.size());
	std::vector<Point2f> massCenters(contours.size());
	// Draw the foreground markers
	for (size_t i = 0; i < contours.size(); i++)
	{
		mu[i] = moments(contours[i], false);
		// Calcula centro de massa da área
		massCenters[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
		drawContours(markers, contours, static_cast<int>(i), Scalar::all((static_cast<int>(i)+1)), -1);
		// Desenha ponto no centro de massa
		// circle( markers, massCenters[i], 4, Scalar(0, 0, 0), -1, 8, 0 );
	}


	Mat imgYCbCr;
	cvtColor(img, imgYCbCr, CV_BGR2YCrCb);

	Mat channels[3];
	split(imgYCbCr, channels);
	// imshow("YCrCb", imgYCbCr);
	// waitKey();

	// markers = markers * 10000;

	// imshow("Drawn contours", markers*10000);

	// Draw the background marker
	// circle(markers, Point(5,5), 3, CV_RGB(255,255,255), -1);

	namedWindow("Markers", WINDOW_NORMAL);
	imshow("Markers", markers * 10000);

	// Plotar gráficos Y, Cb e Cr nas 4 direções de cada região encontrada
	for (size_t i = 0; i < contours.size(); i++)
	{
		// std::vector< std::vector<int> > yValues(4);
		// std::vector< std::vector<int> > cbValues(4);
		// std::vector< std::vector<int> > crValues(4);
		std::vector < std::vector < std::vector<int> > > values(4);
		std::vector < std::vector < std::vector<int> > > variation(4);
//      4 direções    canais ycbcr      valores

		// Inicializar vetor
		for (int j = 0; j < 4; j++)
		{
			values[j] = std::vector<std::vector<int>>(3);
			variation[j] = std::vector<std::vector<int>>(3);
		}

		int k = 0;
		while(1)
		{
			bool inRegion = false;
			// Up direction
			if ( (int)massCenters[i].y - k >= 0 && markers.at<int>((int)massCenters[i].y - k, (int)massCenters[i].x) != 0 )
			{
				values[0][0].push_back( (int)channels[0].at<uchar>((int)massCenters[i].y - k, (int)massCenters[i].x) );
				values[0][1].push_back( (int)channels[1].at<uchar>((int)massCenters[i].y - k, (int)massCenters[i].x) );
				values[0][2].push_back( (int)channels[2].at<uchar>((int)massCenters[i].y - k, (int)massCenters[i].x) );
				if (k > 0)
				{
					variation[0][0].push_back( values[0][0][k] - values[0][0][k-1] );
					variation[0][1].push_back( values[0][1][k] - values[0][1][k-1] );
					variation[0][2].push_back( values[0][2][k] - values[0][2][k-1] );
				}
				inRegion = true;
			}
			// Right direction
			if ( (int)massCenters[i].x + k <= img.cols && markers.at<int>((int)massCenters[i].y, (int)massCenters[i].x + k) != 0 )
			{
				values[1][0].push_back( (int)channels[0].at<uchar>((int)massCenters[i].y, (int)massCenters[i].x + k) );
				values[1][1].push_back( (int)channels[1].at<uchar>((int)massCenters[i].y, (int)massCenters[i].x + k) );
				values[1][2].push_back( (int)channels[2].at<uchar>((int)massCenters[i].y, (int)massCenters[i].x + k) );
				if (k > 0)
				{
					variation[1][0].push_back( values[1][0][k] - values[1][0][k-1] );
					variation[1][1].push_back( values[1][1][k] - values[1][1][k-1] );
					variation[1][2].push_back( values[1][2][k] - values[1][2][k-1] );
				}
				inRegion = true;
			}
			// Down direction
			if ( (int)massCenters[i].y + k <= img.rows && markers.at<int>((int)massCenters[i].y + k, (int)massCenters[i].x) != 0 )
			{
				values[2][0].push_back( (int)channels[0].at<uchar>((int)massCenters[i].y + k, (int)massCenters[i].x) );
				values[2][1].push_back( (int)channels[1].at<uchar>((int)massCenters[i].y + k, (int)massCenters[i].x) );
				values[2][2].push_back( (int)channels[2].at<uchar>((int)massCenters[i].y + k, (int)massCenters[i].x) );
				if (k > 0)
				{
					variation[2][0].push_back( values[2][0][k] - values[2][0][k-1] );
					variation[2][1].push_back( values[2][1][k] - values[2][1][k-1] );
					variation[2][2].push_back( values[2][2][k] - values[2][2][k-1] );
				}
				inRegion = true;
			}
			// Left direction
			if ( (int)massCenters[i].x - k >= 0 && markers.at<int>((int)massCenters[i].y, (int)massCenters[i].x - k) != 0 )
			{
				values[3][0].push_back( (int)channels[0].at<uchar>((int)massCenters[i].y, (int)massCenters[i].x - k) );
				values[3][1].push_back( (int)channels[1].at<uchar>((int)massCenters[i].y, (int)massCenters[i].x - k) );
				values[3][2].push_back( (int)channels[2].at<uchar>((int)massCenters[i].y, (int)massCenters[i].x - k) );
				if (k > 0)
				{
					variation[3][0].push_back( values[3][0][k] - values[3][0][k-1] );
					variation[3][1].push_back( values[3][1][k] - values[3][1][k-1] );
					variation[3][2].push_back( values[3][2][k] - values[3][2][k-1] );
				}
				inRegion = true;
			}
			k++;
			if (!inRegion)
				break;
		}

		CvPlot::plot("Y", &values[0][0][0], values[0][0].size(), 1);
		CvPlot::label("Up");
		CvPlot::plot("Y", &values[1][0][0], values[1][0].size(), 1);
		CvPlot::label("Right");
		CvPlot::plot("Y", &values[2][0][0], values[2][0].size(), 1);
		CvPlot::label("Down");
		CvPlot::plot("Y", &values[3][0][0], values[3][0].size(), 1);
		CvPlot::label("Left");

		CvPlot::plot("Cr", &values[0][1][0], values[0][1].size(), 1);
		CvPlot::label("Up");
		CvPlot::plot("Cr", &values[1][1][0], values[1][1].size(), 1);
		CvPlot::label("Right");
		CvPlot::plot("Cr", &values[2][1][0], values[2][1].size(), 1);
		CvPlot::label("Down");
		CvPlot::plot("Cr", &values[3][1][0], values[3][1].size(), 1);
		CvPlot::label("Left");

		CvPlot::plot("Cb", &values[0][2][0], values[0][2].size(), 1);
		CvPlot::label("Up");
		CvPlot::plot("Cb", &values[1][2][0], values[1][2].size(), 1);
		CvPlot::label("Right");
		CvPlot::plot("Cb", &values[2][2][0], values[2][2].size(), 1);
		CvPlot::label("Down");
		CvPlot::plot("Cb", &values[3][2][0], values[3][2].size(), 1);
		CvPlot::label("Left");

		waitKey();

		// Plotar derivadas
		CvPlot::plot("Y variation", &variation[0][0][0], variation[0][0].size(), 1);
		CvPlot::label("Up");
		CvPlot::plot("Y variation", &variation[1][0][0], variation[1][0].size(), 1);
		CvPlot::label("Right");
		CvPlot::plot("Y variation", &variation[2][0][0], variation[2][0].size(), 1);
		CvPlot::label("Down");
		CvPlot::plot("Y variation", &variation[3][0][0], variation[3][0].size(), 1);
		CvPlot::label("Left");

		CvPlot::plot("Cr variation", &variation[0][1][0], variation[0][1].size(), 1);
		CvPlot::label("Up");
		CvPlot::plot("Cr variation", &variation[1][1][0], variation[1][1].size(), 1);
		CvPlot::label("Right");
		CvPlot::plot("Cr variation", &variation[2][1][0], variation[2][1].size(), 1);
		CvPlot::label("Down");
		CvPlot::plot("Cr variation", &variation[3][1][0], variation[3][1].size(), 1);
		CvPlot::label("Left");

		CvPlot::plot("Cb variation", &variation[0][2][0], variation[0][2].size(), 1);
		CvPlot::label("Up");
		CvPlot::plot("Cb variation", &variation[1][2][0], variation[1][2].size(), 1);
		CvPlot::label("Right");
		CvPlot::plot("Cb variation", &variation[2][2][0], variation[2][2].size(), 1);
		CvPlot::label("Down");
		CvPlot::plot("Cb variation", &variation[3][2][0], variation[3][2].size(), 1);
		CvPlot::label("Left");

		// namedWindow("Up", WINDOW_NORMAL);
		// namedWindow("Down", WINDOW_NORMAL);
		// namedWindow("Left", WINDOW_NORMAL);
		// namedWindow("Right", WINDOW_NORMAL);

		// int coisa = upDirY.size();
		// int range[2] = {0, coisa};
		// imshow("Up", plotGraph(upDirY, range));
		// range[1] = downDirY.size();
		// imshow("Down", plotGraph(downDirY, range));
		// range[1] = leftDirY.size();
		// imshow("Left", plotGraph(leftDirY, range));
		// range[1] = rightDirY.size();
		// imshow("Right", plotGraph(rightDirY, range));

		waitKey();

		CvPlot::clear("Y");
		CvPlot::clear("Cr");
		CvPlot::clear("Cb");
		CvPlot::clear("Y variation");
		CvPlot::clear("Cr variation");
		CvPlot::clear("Cb variation");
	}


/*	watershed(img, markers);

	// imshow("Markers after watershed", markers*10000);

	Mat mark = Mat::zeros(markers.size(), CV_8UC1);
	markers.convertTo(mark, CV_8UC1);
	bitwise_not(mark, mark);
//    imshow("Markers_v2", mark); // uncomment this if you want to see how the mark
								  // image looks like at that point
	// Generate random colors
	vector<Vec3b> colors;
	for (size_t i = 0; i < contours.size(); i++)
	{
		int b = theRNG().uniform(0, 255);
		int g = theRNG().uniform(0, 255);
		int r = theRNG().uniform(0, 255);
		colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
	}
	// Create the result image
	wshed = Mat::zeros(markers.size(), CV_8UC3);
	// Fill labeled objects with random colors
	for (int i = 0; i < markers.rows; i++)
	{
		for (int j = 0; j < markers.cols; j++)
		{
			int index = markers.at<int>(i,j);
			if (index > 0 && index <= static_cast<int>(contours.size()))
				wshed.at<Vec3b>(i,j) = colors[index-1];
			else
				wshed.at<Vec3b>(i,j) = Vec3b(0,0,0);
		}
	}*/
}


/*void Watershed::FindWatershed(Mat img, Mat mask, Mat& wshed)
{
	// Eliminate noise and smaller objects
	Mat fg;
	erode(mask, fg, Mat(), Point(-1,-1), 2);

	// Identify image pixels without objects
	Mat bg;
	dilate(mask, bg, Mat(), Point(-1,-1), 3);
	threshold(bg, bg, 1, 128, THRESH_BINARY_INV);

	// Create markers image
	wshed = Mat::zeros(mask.size(), CV_8U);
	wshed = fg + bg;

	wshed.convertTo(wshed, CV_32S);
	watershed(img, wshed);

	wshed.convertTo(wshed, CV_8U);
}
*/
void Watershed::FindWatershed(Mat img, Mat mask, Mat& wshed)
{
	// Eliminate noise and smaller objects
	Mat fg;										// fg: Foreground
	erode(mask, fg, Mat(), Point(-1,-1), 2);

	// Identify image pixels without objects
	Mat bg;										// bg: Background
	dilate(mask, bg, Mat(), Point(-1,-1), 3);
	threshold(bg, bg, 1, 128, THRESH_BINARY_INV);

	// Distance transform
	Mat dst;
	distanceTransform(bg,dst,CV_DIST_L2,CV_DIST_MASK_PRECISE);		// Aplicar em fg e bg para ver os resultados?
	normalize(dst,dst,0,1, NORM_MINMAX,-1);
	
	imshow("Distance Transformed and Normalized",dst);
	
	//Para criar os marcadores do foreground
	threshold(dst,dst,.4,1.,CV_THRESH_BINARY);
	
	imshow("Thresholded",dst);
	
	//Converter a imagem para uso no findContours
	Mat dst_8u;
	dst.convertTo(dst_8u,CV_8U);
	
	//Achar os marcadores
	std::vector<std::vector<Point>> contours;
	findContours(dst_8u, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	
	// Create markers image
	wshed = Mat::zeros(mask.size(), CV_8U);
	wshed = fg + bg;

	//wshed.convertTo(wshed, CV_32S);
	//watershed(img, wshed);

	//wshed.convertTo(wshed, CV_8U);
	
	// waitKey(8000);
}