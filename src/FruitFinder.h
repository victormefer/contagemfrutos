#ifndef FRUIT_FINDER_H
#define FRUIT_FINDER_H

#include <opencv2/opencv.hpp>
#include "cvplot/cvplot.h"
#include <exception>

#define N_DIRECTIONS 8
#define CHANNEL 0		// Canal Y
#define THRESHOLD 30

class FruitFinder
{
public:
	FruitFinder() {}
	/*static*/ int FindFruits(cv::Mat img, cv::Mat mask, cv::Mat& outputMarkers);

private:
	// /*static*/ void BeginBlobSplit();
	/*static*/ bool SplitBlobs(int massCenterRow, int massCenterCol);

	/*static*/ cv::Mat imgYCbCr, markers, channels[3];
	/*static*/ std::vector<cv::Point2f> massCenters;
	/*static*/ std::vector<std::vector<cv::Point>> contours;
	/*static*/ int thresh;			// Patamar para testar as derivadas.
	// /*static*/ std::vector<cv::Vec3b> colors;
};




#endif