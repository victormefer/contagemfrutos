#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <list>
#include "Trainer.h"
#include "Classifier.h"
#include "SiftMatcher.h"

class UserInterface
{
public:
	UserInterface() : trainer(), sift() {}
	~UserInterface(){}

	void MainMenu();

private:
	cv::Mat CarregarImagem();
	void TreinoManual();
	void SeletorROIs(cv::Mat img, cv::Mat* dadosTreino, cv::Mat* classesTreino, int nClasses);
	void CarregarArvore();
	void SalvarArvore();
	void CarregarTreino();
	void SalvarTreino();
	void Classificar();
	void AddSIFT();
	void TestarSIFT();
	void TesteBatch();
	int Comparar(std::string nomeArq, cv::Mat saida);

	Trainer trainer;

	int canaisTreino;

	SiftMatcher sift;

};


#endif // USERINTERFACE_H