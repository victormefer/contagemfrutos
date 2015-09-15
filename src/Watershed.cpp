#include "Watershed.h"


int Watershed::FindWatershed(cv::Mat img, cv::Mat mask, cv::Mat& markers, std::vector<cv::Point2f>& massCenters)
{
	cv::Mat dst;
	cv::distanceTransform(mask,dst,CV_DIST_L2,CV_DIST_MASK_PRECISE);		// Aplicar em fg e bg para ver os resultados?
	cv::normalize(dst,dst,0,1, cv::NORM_MINMAX,-1);

	cv::threshold(dst, dst, .4, 1., CV_THRESH_BINARY);

	// cv::Mat kernel1 = cv::Mat::ones(3, 3, CV_8UC1);
	// dilate(dst, dst, kernel1);
	// imshow("Peaks", dst);

	cv::Mat dist_8u;
	dst.convertTo(dist_8u, CV_8U);

	std::vector<std::vector<cv::Point>> contours;
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
		//std::cout << "Mass center pos: " << (int)massCenters[i].x << ", " << (int)massCenters[i].y << std::endl;
		cv::drawContours(markers, contours, i, cv::Scalar(colors[i])/*::all(i+1))*/, -1);

		// Desenha ponto no centro de massa
		cv::circle( markers, massCenters[i], 4, cv::Scalar(255, 255, 255), -1, 8, 0 );

		// Desenha texto com o indice da região
		cv::putText( markers, std::to_string(i), massCenters[i] + cv::Point2f(-10, -10), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255) );
	}

	// imshow("YCrCb", imgYCbCr);
	// waitKey();

	// markers = markers * 10000;

	// imshow("Drawn contours", markers*10000);

	// Draw the background marker
	// circle(markers, cv::Point(5,5), 3, CV_RGB(255,255,255), -1);

	// namedWindow("Markers", WINDOW_NORMAL);
	// imshow("Markers", markers * 10000);

	return contours.size();

/*	watershed(img, markers);

	// imshow("Markers after watershed", markers*10000);

	cv::Mat mark = cv::Mat::zeros(markers.size(), CV_8UC1);
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
	wshed = cv::Mat::zeros(markers.size(), CV_8UC3);
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





/*void Watershed::FindWatershed(cv::Mat img, cv::Mat mask, cv::Mat& wshed)
{
	// Eliminate noise and smaller objects
	cv::Mat fg;
	erode(mask, fg, cv::Mat(), cv::Point(-1,-1), 2);

	// Identify image pixels without objects
	cv::Mat bg;
	dilate(mask, bg, cv::Mat(), cv::Point(-1,-1), 3);
	threshold(bg, bg, 1, 128, THRESH_BINARY_INV);

	// Create markers image
	wshed = cv::Mat::zeros(mask.size(), CV_8U);
	wshed = fg + bg;

	wshed.convertTo(wshed, CV_32S);
	watershed(img, wshed);

	wshed.convertTo(wshed, CV_8U);
}
*/

// ESSE FUNCIONAVA
/*void Watershed::FindWatershed(cv::Mat img, cv::Mat mask, cv::Mat& wshed)
{
	// Eliminate noise and smaller objects
	cv::Mat fg;										// fg: Foreground
	erode(mask, fg, cv::Mat(), cv::Point(-1,-1), 2);

	// Identify image pixels without objects
	cv::Mat bg;										// bg: Background
	dilate(mask, bg, cv::Mat(), cv::Point(-1,-1), 3);
	threshold(bg, bg, 1, 128, THRESH_BINARY_INV);

	// Distance transform
	cv::Mat dst;
	distanceTransform(bg,dst,CV_DIST_L2,CV_DIST_MASK_PRECISE);		// Aplicar em fg e bg para ver os resultados?
	normalize(dst,dst,0,1, NORM_MINMAX,-1);
	
	imshow("Distance Transformed and Normalized",dst);
	
	//Para criar os marcadores do foreground
	threshold(dst,dst,.4,1.,CV_THRESH_BINARY);
	
	imshow("Thresholded",dst);
	
	//Converter a imagem para uso no findContours
	cv::Mat dst_8u;
	dst.convertTo(dst_8u,CV_8U);
	
	//Achar os marcadores
	std::vector<std::vector<cv::Point>> contours;
	findContours(dst_8u, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	
	// Create markers image
	wshed = cv::Mat::zeros(mask.size(), CV_8U);
	wshed = fg + bg;

	//wshed.convertTo(wshed, CV_32S);
	//watershed(img, wshed);

	//wshed.convertTo(wshed, CV_8U);
	
	// waitKey(8000);
}
*/