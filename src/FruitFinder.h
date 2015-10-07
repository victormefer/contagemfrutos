#ifndef FRUIT_FINDER_H
#define FRUIT_FINDER_H

#include <opencv2/opencv.hpp>
#include "cvplot/cvplot.h"
#include <exception>

#define N_DIRECTIONS 8
#define CHANNEL 1
#define THRESHOLD 30

class FruitFinder
{
public:
	FruitFinder() { channel = CHANNEL; thresh = THRESHOLD; }
	FruitFinder(int channel, int thresh, int cut, int distTransf) { this->channel = channel; this->thresh = thresh; this->cut = cut; this->distTransf = distTransf; }
	
	int FindFruits(cv::Mat img, cv::Mat mask, cv::Mat& outputMarkers);

private:
	bool SplitBlobs(int massCenterRow, int massCenterCol);

	cv::Mat imgYCbCr, markers, channels[3];
	std::vector<cv::Point2f> massCenters;
	std::vector<std::vector<cv::Point>> contours;
	int channel, thresh;			// Patamar para testar as derivadas.
	bool cut, distTransf;
};


#endif