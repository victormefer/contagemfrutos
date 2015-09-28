// #ifndef SIFTMATCHER_H
// #define SIFTMATCHER_H

// #include <opencv2/opencv.hpp>
// #include <opencv2/nonfree/features2d.hpp>
// #include <list>
// #include <vector>
// #include <string>
// #include <fstream>


// class SiftMatcher
// {
// public:
// 	SiftMatcher();
// 	~SiftMatcher();

// 	void AddImageToDatabase(std::string imageName);

// 	/*std::list<cv::Mat>*/cv::Mat TestSift(cv::Mat image);

// private:
// 	class SiftImage
// 	{
// 	public:
// 		SiftImage();
// 		SiftImage(std::string name);
// 		std::string imageName;
// 		std::vector<cv::KeyPoint> keypoints;
// 		cv::Mat descriptors;
// 	};
	
// 	std::list<SiftImage> imageDatabase;

// };


// #endif // SIFTMATCHER_H