/*
This source is one of the sample codes from the OpenCV 2.1 library
*/

#ifdef _CH_
#pragma package <opencv>
#endif

#define CV_NO_BACKWARD_COMPATIBILITY

#ifndef _EiC

/* This is a standalone program. Pass an image name as a first parameter of the program.
   Switch between standard and probabilistic Hough transform by changing "#if 1" to "#if 0" and back */
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


#include <stdio.h>
#include <stdlib.h>
#endif

IplImage* marker_mask = 0;
IplImage* markers = 0;
IplImage* img0 = 0, *img = 0, *img_gray = 0, *wshed = 0;
Point prev_pt = {-1,-1};

void on_mouse( int event, int x, int y, int flags, void* param )
{
	if( !img )
		return;

	if( event == CV_EVENT_LBUTTONUP || !(flags & CV_EVENT_FLAG_LBUTTON) )
		prev_pt = Point(-1,-1);
	else if( event == CV_EVENT_LBUTTONDOWN )
		prev_pt = Point(x,y);
	else if( event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON) )
	{
		Point pt = Point(x,y);
		if( prev_pt.x < 0 )
			prev_pt = pt;
		Line( marker_mask, prev_pt, pt, Scalar::all(255), 5, 8, 0 );
		Line( img, prev_pt, pt, Scalar::all(255), 5, 8, 0 );
		prev_pt = pt;
		imshow( "image", img );
	}
}


int main( int argc, char** argv )
{
	char* filename = argc >= 2 ? argv[1] : (char*)"fruits.jpg";
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvRNG rng = cvRNG(-1);

	if( (img0 = imread(filename,1)) == 0 )
		return 0;

	printf( "Hot keys: \n"
			"\tESC - quit the program\n"
			"\tr - restore the original image\n"
			"\tw or SPACE - run watershed algorithm\n"
			"\t\t(before running it, roughly mark the areas on the image)\n"
			"\t  (before that, roughly outline several markers on the image)\n" );

	namedWindow( "image", 1 );
	namedWindow( "watershed transform", 1 );

	img = clone( img0 );
	img_gray = clone( img0 );
	wshed = clone( img0 );
	marker_mask.create( img.size, 8, 1 );
	markers.create( img.size, IPL_DEPTH_32S, 1 );
	cvtColor( img, marker_mask, CV_BGR2GRAY );
	cvtColor( marker_mask, img_gray, CV_GRAY2BGR );

	set(marker_mask,Scalar::all(0),0);
	set(wshed,Scalar::all(0),0);
	imshow( "image", img );
	imshow( "watershed transform", wshed );
	cvSetMouseCallback( "image", on_mouse, 0 );

	for(;;)
	{
		int c = waitKey(0);

		if( (char)c == 27 )
			break;

		if( (char)c == 'r' )
		{
			set(marker_mask,Scalar::all(0),0);
			copy( img0, img );
			imshow( "image", img );
		}

		if( (char)c == 'w' || (char)c == ' ' )
		{
			CvSeq* contours = 0;
			Mat* color_tab = 0;
			int i, j, comp_count = 0;

			cvClearMemStorage(storage);

			//cvSaveImage( "wshed_mask.png", marker_mask );
			//marker_mask = imread( "wshed_mask.png", 0 );
			cvFindContours( marker_mask, storage, &contours, sizeof(CvContour),
							CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
			set(markers,Scalar::all(0),0);
			for( ; contours != 0; contours = contours->h_next, comp_count++ )
			{
				drawContours( markers, contours, Scalar::all(comp_count+1),
								Scalar::all(comp_count+1), -1, -1, 8, Point(0,0) );
			}

			if( comp_count == 0 )
				continue;

			color_tab.create( 1, comp_count, CV_8UC3 );
			for( i = 0; i < comp_count; i++ )
			{
				uchar* ptr = color_tab->data.ptr + i*3;
				ptr[0] = (uchar)(cvRandInt(&rng)%180 + 50);
				ptr[1] = (uchar)(cvRandInt(&rng)%180 + 50);
				ptr[2] = (uchar)(cvRandInt(&rng)%180 + 50);
			}

			{
			double t = (double)getTickCount();
			cvWatershed( img0, markers );
			t = (double)getTickCount() - t;
			printf( "exec time = %gms\n", t/(getTickFrequency()*1000.) );
			}

			// paint the watershed image
			for( i = 0; i < markers->height; i++ )
				for( j = 0; j < markers->width; j++ )
				{
					int idx = CV_IMAGE_ELEM( markers, int, i, j );
					uchar* dst = &CV_IMAGE_ELEM( wshed, uchar, i, j*3 );
					if( idx == -1 )
						dst[0] = dst[1] = dst[2] = (uchar)255;
					else if( idx <= 0 || idx > comp_count )
						dst[0] = dst[1] = dst[2] = (uchar)0; // should not get here
					else
					{
						uchar* ptr = color_tab->data.ptr + (idx-1)*3;
						dst[0] = ptr[0]; dst[1] = ptr[1]; dst[2] = ptr[2];
					}
				}

			addWeighted( wshed, 0.5, img_gray, 0.5, 0, wshed );
			imshow( "watershed transform", wshed );
			cvReleaseMat( &color_tab );
		}
	}

	return 1;
}

#ifdef _EiC
main(1,"watershed.cpp");
#endif
