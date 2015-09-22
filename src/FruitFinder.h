#ifndef FRUIT_FINDER_H
#define FRUIT_FINDER_H

#include <opencv2/opencv.hpp>
#include "cvplot/cvplot.h"

#define N_DIRECTIONS 8

class FruitFinder
{
public:
	static int FindFruits(cv::Mat img, cv::Mat mask, cv::Mat& outputMarkers);

private:
	static void BeginBlobSplit();
	static bool SplitBlobs(int massCenterRow, int massCenterCol);

	static cv::Mat imgYCbCr, markers, channels[3];
	static std::vector<cv::Point2f> massCenters;
	static std::vector<std::vector<cv::Point>> contours;
};




#endif