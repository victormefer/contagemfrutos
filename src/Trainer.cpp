#include "Trainer.h"

using namespace cv;

Trainer::Trainer()
{
	dtree = NULL;
	trainAttributes = Mat();
	trainClasses = Mat();
	this->nClasses = 0;
	this->nAttributes = 0;
	rgb = false;
	lab = false;
	rgbEq = false;
	labEq = false;
}


Trainer::Trainer(int nClasses, int nAttributes)
{
	dtree = nullptr;
	trainAttributes = Mat();
	trainClasses = Mat();
	this->nClasses = nClasses;
	this->nAttributes = nAttributes;
	rgb = false;
	lab = false;
	rgbEq = false;
	labEq = false;
}


void Trainer::SetColorSpaces(int colorSpaceFlags)
{
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


Trainer::~Trainer()
{
	imageList.clear();
}




void Trainer::AddTrainingImg(std::string filename)
{
	Mat img = imread(filename);
	if (img.data == NULL)
		throw std::invalid_argument("Nao foi possivel abrir a imagem.");

	imageList.push_back(img);
}



/*
void Trainer::AddTrainingData(std::list<std::list<Point>> pointList, std::list<std::list<int>> classesList)
{
	if (pointList.size() != imageList.size())
	{
		throw std::invalid_argument("Lista de pontos de tamanho diferente da lista de imagens.");
	}

	// Pegar valores dos pixels apontados
	for (auto imIt = imageList.begin(), auto ptIt = pointList.begin(), auto clIt = classesList.begin(); imIt != imageList.end(); ++imIt, ++ptIt, ++clIt);
	{
		for (auto point = ptIt->begin(), auto classId = clIt->begin(); point != ptIt->end(); ++point, ++classId)
		{
			Mat auxAttr = Mat( 1, nAttributes, CV_32FC1 );
			Mat auxClasses = Mat( 1, 1, CV_32S );
			auxClasses.at<int>(0) = *classId;

			if (rgb)
			{
				for (int i = 0; i < 3; i++)
					auxAttr.at<float>(0, i) = (float)imIt->at<uchar>(point->x, point->y, i);
			}

			if (lab)
			{
				Mat auxLab;
				cvtColor(*imIt, auxLab, CV_BGR2Lab);
				for (int i = 0; i < 3; i++)
					auxAttr.at<float>(0, i) = (float)auxLab.at<uchar>(point->x, point->y, i);
			}

			if (rgbEq)
			{
				Mat auxRgbEq;
				std::vector<Mat> RGBChannels;
				split(*imIt, RGBChannels);
				equalizeHist(RGBChannels[0], RGBChannels[0]);
				equalizeHist(RGBChannels[1], RGBChannels[1]);
				equalizeHist(RGBChannels[2], RGBChannels[2]);
				merge(RGBChannels, auxRgbEq);
				for (int i = 0; i < 3; i++)
					auxAttr.at<float>(0, i) = (float)auxRgbEq.at<uchar>(point->x, point->y, i);
			}

			if (labEq)
			{
				Mat auxLabEq;
				cvtColor(*imIt, auxLabEq, CV_BGR2Lab);
				std::vector<Mat> LABChannels;
				split(auxLabEq, LABChannels);
				equalizeHist(LABChannels[0], LABChannels[0]);
				equalizeHist(LABChannels[1], LABChannels[1]);
				equalizeHist(LABChannels[2], LABChannels[2]);
				merge(LABChannels, auxLabEq);
				for (int i = 0; i < 3; i++)
					auxAttr.at<float>(0, i) = (float)auxLabEq.at<uchar>(point->x, point->y, i);
			}

			trainAttributes.push_back(auxAttr);
			classesList.push_back(auxClasses);
		}
	}
}*/


void Trainer::AddTrainingData(Mat* trainData, Mat* trainClasses)
{
	this->trainAttributes.push_back(*trainData);
	this->trainClasses.push_back(*trainClasses);
}



void Trainer::Train(bool surrogates, int cvfolds, bool rule1se)
{
	Mat var_type = Mat( nAttributes + 1, 1, CV_8U );
	var_type.setTo(Scalar(CV_VAR_CATEGORICAL) );
	
	// Parâmetros da árvore de decisão
	CvDTreeParams params = CvDTreeParams( 25, // tamanho maximo da arvore 
												  3,  // numero minimo de samples
												  0.0f,  // acurácia da regressão
												  surrogates, // usar surrogates
												  nClasses, // maximo de categorias
												  cvfolds, // numero maximo de cross-validation folds
												  rule1se, // usar regras 1SE - arvores menores
												  true, // remover nós cortados
												  NULL  // array de prioridades
	);
	
	// Treinar árvore de decisão
	dtree = new CvDTree();
	dtree->train(trainAttributes, CV_ROW_SAMPLE, trainClasses, Mat(), Mat(), var_type, Mat(), params);
}



void Trainer::SaveTrainingData(std::string filename)
{
	// Salva nos arquivos dados e classes separados
	FileStorage file( filename, FileStorage::WRITE);
	file << "nAttributes" << nAttributes;
	file << "rgb" << rgb;
	file << "lab" << lab;
	file << "rgbEq" << rgbEq;
	file << "labEq" << labEq;
	file << "nClasses" << nClasses;
	file << "TrainData" << trainAttributes;
	file << "TrainClasses" << trainClasses;
	file.release();
}


void Trainer::LoadTrainingData(std::string filename)
{
	FileStorage file( filename, FileStorage::READ);
	if (!file.isOpened())
		throw std::string("Nao foi possivel abrir o arquivo.");

	trainAttributes.release();
	trainClasses.release();

	file["nAttributes"] >> nAttributes;
	file["rgb"] >> rgb;
	file["lab"] >> lab;
	file["rgbEq"] >> rgbEq;
	file["labEQ"] >> labEq;
	file["nClasses"] >> nClasses;
	file["TrainData"] >> trainAttributes;
	file["TrainClasses"] >> trainClasses;
	file.release();
}



void Trainer::SaveTree(std::string filename)
{
	dtree->save(filename.c_str());
}


void Trainer::LoadTree(std::string filename)
{
	delete(dtree);
	try
	{
		dtree->load(filename.c_str());
	}
	catch (...)
	{
		throw std::string("Nao foi possivel abrir o arquivo.");
	}
}


void Trainer::SetNumberClasses(int nClasses)
{
	this->nClasses = nClasses;
}

void Trainer::SetNumberAttributes(int nAttributes)
{
	this->nAttributes = nAttributes;
}

CvDTree* Trainer::GetTree()
{
	return dtree;
}

int Trainer::GetNClasses()
{
	return nClasses;
}

int Trainer::GetNAttributes()
{
	return nAttributes;
}