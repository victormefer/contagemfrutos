#include "Classifier.h"

using namespace cv;

Classifier::Classifier(int nClasses, int colorSpaceFlags)
{
	img = Mat();
	tree = nullptr;
	this->nClasses = nClasses;

	nAttributes = 0;
	if (colorSpaceFlags % 2 == 1)
	{
		rgb = true;
		nAttributes += 3;
	}
	else
		rgb = false;
	if ((colorSpaceFlags << 1) % 2 == 1 )
	{
		lab = true;
		nAttributes += 3;
	}
	else
		lab = false;
	if ((colorSpaceFlags << 2) % 2 == 1 )
	{
		rgbEq = true;
		nAttributes += 3;
	}
	else
		rgbEq = false;
	if ((colorSpaceFlags << 3) % 2 == 1 )
	{
		labEq = true;
		nAttributes += 3;
	}
	else
		labEq = false;
}

Classifier::Classifier(std::string filename, CvDTree* tree, int nClasses, int colorSpaceFlags)
{
	this->tree = tree;
	this->nClasses = nClasses;
	LoadIMG(filename);

	nAttributes = 0;
	if (colorSpaceFlags % 2 == 1)
	{
		rgb = true;
		nAttributes += 3;
	}
	else
		rgb = false;
	if ((colorSpaceFlags >> 1) % 2 == 1 )
	{
		lab = true;
		nAttributes += 3;
	}
	else
		lab = false;
	if ((colorSpaceFlags >> 2) % 2 == 1 )
	{
		rgbEq = true;
		nAttributes += 3;
	}
	else
		rgbEq = false;
	if ((colorSpaceFlags >> 3) % 2 == 1 )
	{
		labEq = true;
		nAttributes += 3;
	}
	else
		labEq = false;
}


void Classifier::LoadIMG(std::string filename)
{
	img = imread(filename);
	if (img.data == NULL)
		throw std::invalid_argument("Nao foi possivel abrir a imagem.");

	cvtColor(img, imgLab, CV_BGR2Lab);

	std::vector<Mat> RGBChannels;
	split(img, RGBChannels);
	equalizeHist(RGBChannels[0], RGBChannels[0]);
	equalizeHist(RGBChannels[1], RGBChannels[1]);
	equalizeHist(RGBChannels[2], RGBChannels[2]);
	merge(RGBChannels, imgRgbEq);

	std::vector<Mat> LABChannels;
	split(imgLab, LABChannels);
	equalizeHist(LABChannels[0], LABChannels[0]);
	equalizeHist(LABChannels[1], LABChannels[1]);
	equalizeHist(LABChannels[2], LABChannels[2]);
	merge(LABChannels, imgLabEq);
}


Mat Classifier::Classify()
{
	// Nó de resultado da classificacao
	CvDTreeNode* noResult;
	// Matriz de resultado da classificacao
	Mat output (img.rows, img.cols, CV_8UC3);
	// Matriz q guarda um pixel a ser classificado
	Mat pixelTeste = Mat( 1, nAttributes, CV_32FC1 );

	// Classificar
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			int k = 0;

			if (rgb)
			{
				pixelTeste.at<float>(0, k++) = (float)img.at<Vec3b>(i, j)[0];
				pixelTeste.at<float>(0, k++) = (float)img.at<Vec3b>(i, j)[1];
				pixelTeste.at<float>(0, k++) = (float)img.at<Vec3b>(i, j)[2];
			}
			if (lab)
			{
				pixelTeste.at<float>(0, k++) = (float)imgLab.at<Vec3b>(i, j)[0];
				pixelTeste.at<float>(0, k++) = (float)imgLab.at<Vec3b>(i, j)[1];
				pixelTeste.at<float>(0, k++) = (float)imgLab.at<Vec3b>(i, j)[2];
			}

			if(rgbEq)
			{
				pixelTeste.at<float>(0, k++) = (float)imgRgbEq.at<Vec3b>(i, j)[0];
				pixelTeste.at<float>(0, k++) = (float)imgRgbEq.at<Vec3b>(i, j)[1];
				pixelTeste.at<float>(0, k++) = (float)imgRgbEq.at<Vec3b>(i, j)[2];
			}
			if (labEq)
			{
				pixelTeste.at<float>(0, k++) = (float)imgLabEq.at<Vec3b>(i, j)[0];
				pixelTeste.at<float>(0, k++) = (float)imgLabEq.at<Vec3b>(i, j)[1];
				pixelTeste.at<float>(0, k++) = (float)imgLabEq.at<Vec3b>(i, j)[2];
			}

			// Classificar
			noResult = tree->predict(pixelTeste, Mat(), false);

			// Pinta cada classe de uma cor
			output.at<Vec3b>(i, j) = classColors[(int)noResult->value];
		}
	}
	ImproveWithWatershed(output);
	return output;
	// erode(*output, *output, cv::Mat(), cv::Point(-1,-1), 2);
}


void Classifier::SetTree(CvDTree* tree)
{
	this->tree = tree;
}


void Classifier::ImproveWithWatershed(cv::Mat& output)
{
	// Eliminate noise and smaller objects
	cv::Mat gray;
	cv::cvtColor(output, gray, CV_BGR2GRAY);

	cv::Mat fg;
	cv::erode(gray, fg, cv::Mat(), cv::Point(-1,-1), 2);

	// Identify image pixels without objects
	cv::Mat bg;
	cv::dilate(gray, bg, cv::Mat(), cv::Point(-1,-1), 3);
	cv::threshold(bg, bg, 1, 128, THRESH_BINARY_INV);

	// Create markers image
	cv::Mat wshed = cv::Mat::zeros(output.size(), CV_8U);
	wshed = fg + bg;

	wshed.convertTo(wshed, CV_32S);
	cv::watershed(img, wshed);

	wshed.convertTo(output, CV_8U);
	cv::threshold(output, output, 129, 255, CV_THRESH_BINARY);
}