#ifndef WATERSHED_H
#define WATERSHED_H

#include <opencv2/opencv.hpp>
#include "cvplot/cvplot.h"
// #include "graphUtils/GraphUtils.h"

class Watershed
{
public:
	// Retorna matriz markers, uma mascara com blobs q são possíveis locais de frutos,
	// e um inteiro com a quantidade desses blobs
	static int FindWatershed(cv::Mat img, cv::Mat mask, cv::Mat& markers, std::vector<cv::Point2f>& massCenters);
	// static void FindWatershed2(cv::Mat img, cv::Mat mask, cv::Mat& wshed);
};




#endif