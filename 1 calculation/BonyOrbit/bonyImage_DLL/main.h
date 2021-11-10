#pragma once
#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;


//main.cpp
extern "C" _declspec(dllexport) int _stdcall readImage(char *imagePath);
extern "C" _declspec(dllexport) int _stdcall processImage(int compareLength, char *savePath);


void sizeAdject(cv::Mat &inputImage, cv::Mat &outputImage, float scale);
void cutRed(cv::Mat &inputImage, cv::Mat &outputImage);




cv::Mat m_currentMat;				//当前图像
string m_sCurrFrameName;			//当前图像的名字


#define EC_NOTFIND 00002			//没找到红线


