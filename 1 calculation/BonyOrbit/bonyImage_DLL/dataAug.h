#pragma once
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;




extern "C" _declspec(dllexport) int _stdcall dataAugment(char *imagePath, int cnt);
extern "C" _declspec(dllexport) int _stdcall dataAugment1(char *imagePath, int cnt);

int bnyEyeMat(cv::Mat src, cv::Mat &dst);
int offSetMat(cv::Mat src, string imageName, int num);
int findFstCol(cv::Mat src, int &num, int &numR);
int offSetMat2(cv::Mat src, string imageName, int fstColNum, int offNum);
int offSetMat3(cv::Mat src, string imageName, int fstColNum, int offNum);

cv::Mat saltAndPepper(cv::Mat src, int perct);
cv::Mat addGaussianNoise(cv::Mat src, int perct);
cv::Mat darker(cv::Mat src);
cv::Mat brighter(cv::Mat src);
cv::Mat rotation(cv::Mat src, double degree);


double generateGaussianNoise(double mu, double sigma);

#define EC_NOIMAGEINPUT 10001			//有没有读到图
#define OFFSETNUM 20					//平移的像素











