#ifndef SUPERPIXEL_H
#define SUPERPIXEL_H

#include <stdio.h>
#include <math.h>
#include <vector>
#include <float.h>

#include <opencv2/opencv.hpp>

/* Vetores 2D para trabalhar com matrizes 2D*/
#define vec2dd std::vector<std::vector<double>>
#define vec2di std::vector<std::vector<int>>
#define vec2db std::vector<std::vector<bool>>

#define NR_ITERATIONS 10			/*Num. de iteracoes na execucao do algoritmo*/

class Slic
{
	private:
        vec2di clusters;
        vec2dd distances;
        vec2dd centers;						/*Coordenadas dos centros*/
        std::vector<int> center_counts;		/*Contador de ocorrencias de centro*/
        int step, nc, ns;					/*step: tamanho do passo por cluster, nc: cor; ns: distancia*/
        
        double compute_dist(int ci, cv::Point pixel, cv::Scalar colour);	    	/*Computa a distancia entre o centro e um pixel*/
        cv::Point find_local_minimum(const cv::Mat &image, cv::Point center);		/*Acha o pixel com o menor gradiente numa regiao 3x3*/
        
        void clear_data();
        void init_data(const cv::Mat &image);
	public:
        Slic();				
        ~Slic();
        
        void generate_superpixels(const cv::Mat &image, int step, int nc);
        void create_connectivity(const cv::Mat &image);
        
        void display_center_grid(cv::Mat &image, cv::Vec3b colour);
        void display_contours(cv::Mat &image, cv::Vec3b colour);
        void colour_with_cluster_means(cv::Mat &image);
};

#endif	/*SUPERPIXEL_H*/
