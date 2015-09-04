#include "Watershed.h"

using namespace cv;

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
		massCenters[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
		drawContours(markers, contours, static_cast<int>(i), Scalar::all(static_cast<int>(i)+1), -1);
		circle( markers, massCenters[i], 4, Scalar(0, 0, 0), -1, 8, 0 );
	}

	// imshow("Drawn contours", markers*10000);

	// Draw the background marker
	// circle(markers, Point(5,5), 3, CV_RGB(255,255,255), -1);

	namedWindow("Markers", WINDOW_NORMAL);
	imshow("Markers", markers*10000);

	watershed(img, markers);

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
	}
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