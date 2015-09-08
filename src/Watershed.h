#ifndef WATERSHED_H
#define WATERSHED_H

#include <opencv2/opencv.hpp>
#include "cvplot/cvplot.h"
// #include "graphUtils/GraphUtils.h"

class Watershed
{
public:
	static void FindWatershed(cv::Mat img, cv::Mat mask, cv::Mat& wshed);
	static void FindWatershed2(cv::Mat img, cv::Mat mask, cv::Mat& wshed);
};




#endif