#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <opencv2/opencv.hpp>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <list>
#include "Trainer.h"
#include "Classifier.h"
// #include "SiftMatcher.h"
#include "LocalExtrema.h"
#include "Watershed.h"
#include "FruitFinder.h"
#include "Superpixel.h"
#include "Arquivos.h"

class UserInterface
{
public:
	UserInterface() : trainer()/*, sift()*/ {}
	~UserInterface(){}

	void MainMenu();
	void TesteBatch(int fruta, int inicio, std::string arqTreino, int canal, int threshold, int cut, int distTransf);

private:
	std::string CarregarImagem();
	void TreinoManual();
	void SeletorROIs(cv::Mat img, cv::Mat* dadosTreino, cv::Mat* classesTreino, int nClasses);
	void CarregarArvore();
	void SalvarArvore();
	void CarregarTreino();
	void SalvarTreino();
	void Classificar();
	// void AddSIFT();
	// void TestarSIFT();
	void TesteBatch();
	int Comparar(std::string nomeArq, cv::Mat saida);
	void ExtremosLocais();
	void Watershed();
	void Superpixel(int nr_superpixels,int nc);

	Trainer trainer;

	int canaisTreino;

	// SiftMatcher sift;

	cv::Mat imgOriginal, resultClassif, resultExtrem, resultBlobs;
	int numFruits;
};


#endif // USERINTERFACE_H
