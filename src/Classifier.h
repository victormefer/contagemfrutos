#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>


class Classifier
{
public:
	Classifier(int nClasses, int colorSpaceFlags);
	Classifier(std::string filename, CvDTree* tree, int nClasses, int colorSpaceFlags);
	~Classifier(){}

	void LoadIMG(std::string filename);

	cv::Mat Classify();

	void SetTree(CvDTree* tree);

	void ImproveWithWatershed(cv::Mat& output);

private:
	CvDTree* tree;
	cv::Mat img, imgLab, imgRgbEq, imgLabEq;

	int nClasses, nAttributes;
	bool rgb, lab, rgbEq, labEq;

	std::vector<cv::Vec3b> classColors = {
		cv::Vec3b(255, 255, 255),
		cv::Vec3b(0, 0, 0),
		cv::Vec3b(0, 255, 0), 
		cv::Vec3b(0, 255, 255), 
		cv::Vec3b(255, 0, 0), 
		cv::Vec3b(0, 0, 255), 
		// cv::Vec3b(0, 0, 0),
		cv::Vec3b(255, 0, 255), 
		cv::Vec3b(255, 255, 0)
	};

};


#endif // CLASSIFIER_H