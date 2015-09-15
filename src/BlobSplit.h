#ifndef BLOB_SPLIT_H
#define BLOB_SPLIT_H

#include <opencv2/opencv.hpp>
#include "cvplot/cvplot.h"

#define N_DIRECTIONS 8

class BlobSplit
{
public:
	static void SplitBlobs(cv::Mat img, cv::Mat& markers, std::vector<cv::Point2f> massCenters);
};




#endif