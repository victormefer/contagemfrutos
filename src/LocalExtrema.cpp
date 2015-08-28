#include "LocalExtrema.h"


// int LocalExtrema::MaxUsedValInHistogramData(cv::Mat img)
// {
// 	// Calcular histograma
// 	int i, histSize = 256; // Numero de divisões do histograma
// 	float range[] = { 0, 256 } ;
// 	const float* histRange = { range };
// 	int channels[] = { 0 };
// 	cv::Mat hist;

// 	// cv::Mat imgGray;

// 	// img.copyTo(imgGray);
// 	// cv::cvtColor(img, imgGray, CV_BGR2GRAY);
	
// 	cv::calcHist( &img, 1, channels, cv::Mat(), hist, 1, &histSize, &histRange, true, false );

// 	float max = 0.0;
// 	int maxIdx = 0;
// 	for (i = 0; i < 256; i++)
// 	{
// 		if (hist.at<float>(i) > max)
// 		{
// 			max = hist.at<float>(i);
// 			maxIdx = i;
// 		}
// 	}

// 	return i;
// }


void LocalExtrema::LocalMaxima(cv::Mat src, cv::Mat &dst, int squareSize)
{
	if (squareSize==0)
	{
		dst = src.clone();
		return;
	}

	cv::Mat m0;
	// cv::Mat aux;
	dst = src.clone();
	cv::Point maxLoc(0,0);

	// dst = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);

	//1.Be sure to have at least 3x3 for at least looking at 1 pixel close neighbours
	//  Also the window must be <odd>x<odd>
	// SANITYCHECK(squareSize,3,1);
	int sqrCenter = (squareSize-1)/2;

	//2.Create the localWindow mask to get things done faster
	//  When we find a local maxima we will multiply the subwindow with this MASK
	//  So that we will not search for those 0 values again and again
	cv::Mat localWindowMask = cv::Mat::zeros( cv::Size(squareSize,squareSize), CV_8U );//boolean
	localWindowMask.at<unsigned char>(sqrCenter,sqrCenter)=1;

	//3.Find the threshold value to threshold the image
		//this function here returns the peak of histogram of picture
		//the picture is a thresholded picture it will have a lot of zero values in it
		//so that the second boolean variable says :
		//  (boolean) ? "return peak even if it is at 0" : "return peak discarding 0"
	// int thrshld =  MaxUsedValInHistogramData(dst);
	int thrshld = 100;
	cv::threshold(dst, m0, thrshld, 1, cv::THRESH_BINARY);

	// cv::moveWindow("Alguma coisa", 500, 500);
	// cv::namedWindow("Alguma coisa", cv::WINDOW_NORMAL);
	// cv::imshow("Alguma coisa", m0);

	//4.Now delete all thresholded values from picture
	dst = dst.mul(m0);

	//put the src in the middle of the big array
	for (int row=sqrCenter;row<dst.size().height-sqrCenter;row++)
		for (int col=sqrCenter;col<dst.size().width-sqrCenter;col++)
		{
			//1.if the value is zero it can not be a local maxima
			if (dst.at<unsigned char>(row,col)==0)
				continue;
			//2.the value at (row,col) is not 0 so it can be a local maxima point
			m0 =  dst.colRange(col-sqrCenter,col+sqrCenter+1).rowRange(row-sqrCenter,row+sqrCenter+1);

			cv::minMaxLoc(m0,NULL,NULL,NULL,&maxLoc);
			//if the maximum location of this subWindow is at center
			//it means we found the local maxima
			//so we should delete the surrounding values which lies in the subWindow area
			//hence we will not try to find if a point is at localMaxima when already found a neighbour was
			// if ((maxLoc.x==sqrCenter)&&(maxLoc.y==sqrCenter))
			// {
			// 	m0 = m0.mul(localWindowMask);
			// 					//we can skip the values that we already made 0 by the above function
			// 	col+=sqrCenter;
			// }
			// dst.at<uchar>(maxLoc.x, maxLoc.y) = 255;
		}
	// cv::Point maxLoc(0,0);
	// dst = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);

	// cv::minMaxLoc(src,NULL,NULL,NULL,&maxLoc);
	// dst.at<uchar>(maxLoc.x, maxLoc.y) = 255;
}