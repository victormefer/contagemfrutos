#include "SiftMatcher.h"

using namespace cv;

SiftMatcher::SiftImage::SiftImage(){}

SiftMatcher::SiftImage::SiftImage(std::string name)
{
	cv::Mat mat;

	imageName = name;
	mat = cv::imread(name);
	if (mat.data == NULL)
		throw std::string("Nao foi possivel abrir a imagem.");

	// detecting keypoints
	cv::SIFT detector = cv::SIFT(0, 3, 0.04, 10, 1.6);
	detector.detect(mat, keypoints);

	// computing descriptors
	cv::SiftDescriptorExtractor extractor;
	extractor.compute(mat, keypoints, descriptors);
}


SiftMatcher::SiftMatcher()
{
	/*cv::FileStorage file( "SiftMatcherDatabase.xml", cv::FileStorage::READ);

	if (!file.isOpened())
	{
		throw std::string("Base de dados SiftMatcher vazia.");
	}
	else
	{
		int i = 0, j;
		while (!file.empty())
		{
			SiftImage image = SiftImage();
			file["imageName" + std::to_string(i)] >> image.imageName;
			file["keypointsSize" + std::to_string(i)] >> j;
			for (j -= 1; j >= 0; j++)
				file["keypointsVec" + std::to_string(i) + std::to_string(j)] >> image.keypoints[j];
			file["descriptors" + std::to_string(i)] >> image.descriptors;
			imageDatabase.push_back(image);
			i++;
		}
	}

	file.release();*/
}


SiftMatcher::~SiftMatcher()
{
	/*cv::FileStorage file( "SiftMatcherDatabase.xml", cv::FileStorage::WRITE);

	int i = 0;
	for (auto it = imageDatabase.begin(); it != imageDatabase.end(); ++it, ++i)
	{
		file["image" + std::to_string(i)] << it->imageName;
		file["keypointsSize" + std::to_string(i)] << it->keypoints.size();
		for (int j = it->keypoints.size() - 1; j >= 0; j++)
			file["keypointsVec" + std::to_string(i) + std::to_string(j)] << it->keypoints[j];
		file["descriptors" + std::to_string(i)] << it->descriptors;
	}

	file.release();*/
}


void SiftMatcher::AddImageToDatabase(std::string name)
{
	SiftImage image = SiftImage(name);
	imageDatabase.push_back(image);
}


/*std::list<cv::Mat>*/cv::Mat SiftMatcher::TestSift(cv::Mat image)
{
	//std::list<cv::Mat> output;
	cv::Mat img_matches = cv::Mat(image);

	cv::SIFT detector = cv::SIFT(0, 3, 0.04, 10, 1.6);
	std::vector<cv::KeyPoint> testKeypoints;
	detector.detect(image, testKeypoints);

	// computing descriptors
	cv::SiftDescriptorExtractor extractor;
	cv::Mat testDescriptors;
	extractor.compute(image, testKeypoints, testDescriptors);

	for (auto it = imageDatabase.begin(); it != imageDatabase.end(); ++it)
	{
		cv::Mat img_object = cv::imread(it->imageName);

		// MATCHING DECRIPTORS
		cv::FlannBasedMatcher matcher;
		std::vector< cv::DMatch > matches;
		matcher.match( it->descriptors, testDescriptors, matches );

		double max_dist = 0; double min_dist = 100;

		//-- Quick calculation of max and min distances between keypoints
		for( int i = 0; i < it->descriptors.rows; i++ )
		{
			double dist = matches[i].distance;
			if( dist < min_dist ) min_dist = dist;
			if( dist > max_dist ) max_dist = dist;
		}

		std::cout << "-- Max dist : " <<  max_dist << std::endl;
		std::cout << "-- Min dist : " << min_dist << std::endl;

		//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
		std::vector< cv::DMatch > good_matches;

		for( int i = 0; i < it->descriptors.rows; i++ )
		{
			if( matches[i].distance < 3*min_dist )
			{
				good_matches.push_back( matches[i]);
			}
		}

		//cv::Mat img_matches;
		/*cv::drawMatches( img_object, it->keypoints, image, testKeypoints,
			good_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1),
			std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );*/

		//-- Localize the object
		std::vector<cv::Point2f> obj;
		std::vector<cv::Point2f> scene;

		for( int i = 0; i < good_matches.size(); i++ )
		{
			//-- Get the keypoints from the good matches
			obj.push_back( it->keypoints[ good_matches[i].queryIdx ].pt );
			scene.push_back( testKeypoints[ good_matches[i].trainIdx ].pt );
		}

		std::cout << obj.size() << ", " << scene.size() << std::endl;

		if (obj.size() >= 4)
		{
			cv::Mat H = cv::findHomography( obj, scene, CV_RANSAC );

			//-- Get the corners from the image_1 ( the object to be "detected" )
			std::vector<cv::Point2f> obj_corners(4);
			obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( img_object.cols, 0 );
			obj_corners[2] = cvPoint( img_object.cols, img_object.rows ); obj_corners[3] = cvPoint( 0, img_object.rows );
			std::vector<cv::Point2f> scene_corners(4);

			cv::perspectiveTransform( obj_corners, scene_corners, H);

			//-- Draw lines between the corners (the mapped object in the scene - image_2 )
			cv::line( img_matches, scene_corners[0] /*+ cv::Point2f( img_object.cols, 0)*/, scene_corners[1] /*+ cv::Point2f( img_object.cols, 0)*/, cv::Scalar(0, 255, 0), 4 );
			cv::line( img_matches, scene_corners[1] /*+ cv::Point2f( img_object.cols, 0)*/, scene_corners[2] /*+ cv::Point2f( img_object.cols, 0)*/, cv::Scalar( 0, 255, 0), 4 );
			cv::line( img_matches, scene_corners[2] /*+ cv::Point2f( img_object.cols, 0)*/, scene_corners[3] /*+ cv::Point2f( img_object.cols, 0)*/, cv::Scalar( 0, 255, 0), 4 );
			cv::line( img_matches, scene_corners[3] /*+ cv::Point2f( img_object.cols, 0)*/, scene_corners[0] /*+ cv::Point2f( img_object.cols, 0)*/, cv::Scalar( 0, 255, 0), 4 );

		}

		//output.push_back(img_matches);
	}

	return img_matches;
	//return output;
}