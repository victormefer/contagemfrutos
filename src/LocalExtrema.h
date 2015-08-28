#ifndef LOCALEXTREMA_H
#define LOCALEXTREMA_H

#include <opencv2/opencv.hpp>


class LocalExtrema
{
public:
	static void LocalMaxima(cv::Mat src, cv::Mat &dst, int squareSize);

// private:
// 	static int MaxUsedValInHistogramData(cv::Mat img);
};



#endif