#include "Watershed.h"

using namespace cv;

void Watershed::FindWatershed2(Mat img, Mat mask, Mat& wshed)
{
/*	std::vector<std::vector<Point>> contours;
	// CvSeq* contours;
	Mat* color_tab = 0;
	int i, j, comp_count = 0;
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvRNG rng = cvRNG(-1);
	Mat img_gray, markers;

	cvtColor( img, mask, CV_BGR2GRAY );
	cvtColor( mask, img_gray, CV_GRAY2BGR );

	//cvSaveImage( "wshed_mask.png", mask );
	//mask = imread( "wshed_mask.png", 0 );
	findContours( mask, contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
	
	markers = Mat::zeros(img.size(), CV_32S);
	for( int k = 0; k < contours.size(); k++, comp_count++ )
	{
		drawContours( markers, contours, Scalar::all(comp_count+1),
			Scalar::all(comp_count+1), -1, -1, 8, Point(0,0) );
	}

	// if( comp_count == 0 )
	// 	continue;

	color_tab->create( 1, comp_count, CV_8UC3 );
	for( i = 0; i < comp_count; i++ )
	{
		uchar* ptr = color_tab->data.ptr + i*3;
		ptr[0] = (uchar)(cvRandInt(&rng)%180 + 50);
		ptr[1] = (uchar)(cvRandInt(&rng)%180 + 50);
		ptr[2] = (uchar)(cvRandInt(&rng)%180 + 50);1
	}

	watershed( img, markers );
	mask.convertTo(mask, CV_32S);
	watershed( img, mask );

	paint the watershed image
	for( i = 0; i < markers.rows; i++ )
		for( j = 0; j < markers.cols; j++ )
		{
			// int idx = CV_IMAGE_ELEM( markers, int, i, j );
			int idx = markers.at<int>(i, j);
			Vec3b dst = wshed.at<Vec3b>(i, j*3);
			// uchar* dst = &CV_IMAGE_ELEM( wshed, uchar, i, j*3 );
			if( idx == -1 )
				// dst[0] = dst[1] = dst[2] = (uchar)255;
				dst = Vecb(255, 255, 255);
			else if( idx <= 0 || idx > comp_count )
				// dst[0] = dst[1] = dst[2] = (uchar)0; // should not get here
				dst = Vec3b(0, 0, 0);
			else
			{
				uchar* ptr = color_tab->data.ptr + (idx-1)*3;
				dst[0] = ptr[0]; dst[1] = ptr[1]; dst[2] = ptr[2];
			}
		}

	addWeighted( wshed, 0.5, img_gray, 0.5, 0, wshed );
	// imshow( "watershed transform", wshed );*/

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
	// Draw the foreground markers
	for (size_t i = 0; i < contours.size(); i++)
		drawContours(markers, contours, static_cast<int>(i), Scalar::all(static_cast<int>(i)+1), -1);

	// imshow("Drawn contours", markers*10000);

	// Draw the background marker
	circle(markers, Point(5,5), 3, CV_RGB(255,255,255), -1);

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