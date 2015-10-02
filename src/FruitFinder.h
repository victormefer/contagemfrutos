#ifndef FRUIT_FINDER_H
#define FRUIT_FINDER_H

#include <opencv2/opencv.hpp>
#include "cvplot/cvplot.h"
#include <exception>

#define N_DIRECTIONS 8
#define CHANNEL 1		// Canal Y
#define THRESHOLD 30

class FruitFinder
{
public:
	FruitFinder() { thresh = THRESHOLD; }
	int FindFruits(cv::Mat img, cv::Mat mask, cv::Mat& outputMarkers);

private:
	bool SplitBlobs(int massCenterRow, int massCenterCol);

	cv::Mat imgYCbCr, markers, channels[3];
	std::vector<cv::Point2f> massCenters;
	std::vector<std::vector<cv::Point>> contours;
	int thresh;			// Patamar para testar as derivadas.
};


#endif