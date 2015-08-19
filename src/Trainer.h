#ifndef TRAINER_H
#define TRAINER_H

#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <list>
#include <string>

enum ColorSpaces
{
	RGB = 1,
	LAB = 2,
	RGB_EQ = 4,
	LAB_EQ = 8
};

class Trainer
{
public:
	Trainer();
	Trainer(int nClasses, int nAttributes);
	~Trainer();

	void SetColorSpaces(int colorSpaceFlags);

	void AddTrainingImg(std::string filename);

	void AddTrainingData(cv::Mat* trainData, cv::Mat* trainClasses);
	//void AddTrainingData(std::list<std::list<Point>> pointList, std::list<std::list<int>> classesList);

	void Train(bool surrogates = false, int cvfolds = 10, bool rule1se = true);

	void LoadTrainingData(std::string filename);
	void SaveTrainingData(std::string filename);

	void LoadTree(std::string filename);
	void SaveTree(std::string filename);

	//void SetTrainDataAttributes(Mat* trainAttributes);
	//void SetTrainDataClasses(Mat* trainClasses);

	void SetNumberClasses(int nClasses);
	void SetNumberAttributes(int nAttributes);

	CvDTree* GetTree();
	int GetNClasses();
	int GetNAttributes();

	cv::Mat trainAttributes, trainClasses;

private:
	CvDTree* dtree;
	std::list<cv::Mat> imageList;

	int nClasses, nAttributes;
	bool rgb, lab, rgbEq, labEq;
};


#endif // TRAINER_H