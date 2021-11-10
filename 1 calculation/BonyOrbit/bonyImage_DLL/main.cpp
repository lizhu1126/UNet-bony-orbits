#include "main.h"
#include <opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"

using namespace std;
using namespace cv;





int _stdcall processImage(int compareLength, char *savePath)
{
	//*******************1.输入图片**********************
	cv::Mat compareImage = m_currentMat.clone();

	//*******************2.计算直线长度******************
	float outlength = 0;

	//*****************2.1.绘制轮廓********************
	//为了得到红色通道进行处理，并转为二值图
	cv::Mat grayImage[3];
	split(compareImage, grayImage);
	cv::Mat grayRed = grayImage[2];
	cv::Mat binaryMat;
	cv::threshold(grayRed, binaryMat, 127, 255, THRESH_OTSU);

	//在得到的二值图像中寻找轮廓
	vector<vector<cv::Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(binaryMat, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	//*****************2.2.筛选轮廓********************
	int m_min = 250;
	int m_max = 460;
	int a = contours.size();
	for (int i = 0; i < contours.size(); i++)
	{
		//获得最小外界四边形
		cv::RotatedRect rbox = minAreaRect(contours[i]);

		//计算宽长比
		float width = (float)rbox.size.width;
		float height = (float)rbox.size.height;
		//float ratio = width / height;

		if (((width > m_min && width < m_max) && height <= 15)
			|| ((height > m_min && height < m_max) && width <= 15))
		{
			//outlength = width;
			outlength = sqrt(width*width + height*height);
		}
	}
 
	//*******************3.按比例缩放********************
	if (outlength == 0) {
		return EC_NOTFIND;
	}
	cv::Mat resizeImage;
	float adjustScale = outlength / compareLength;

	sizeAdject(compareImage, resizeImage, adjustScale);
	//cv::imshow("【调整后的图像】", resizeImage);


	//*******************4.去除线段**********************
	cv::Mat justImage;
	cutRed(resizeImage, justImage);
	//cv::imshow("【结果图像】", justImage);

	//*******************5.存储图像**********************
	string currentSavePath = savePath + m_sCurrFrameName;
	cv::imwrite(currentSavePath, justImage);

	return 0;
}





int _stdcall readImage(char * imagePath)
{
	m_currentMat = cv::imread(imagePath);
	string framePath = imagePath;
	//string 
	int Tpos = framePath.find_last_of("\\"); //查找在\\最后一次出现的位置，并返回（包括0）
	m_sCurrFrameName = framePath.substr(Tpos + 1); //截取，T后面一位开始到末尾的字符串，即文件的名称，且去掉T
	if (m_currentMat.empty()) {
		return -1;
	}

	return 0;
}


void sizeAdject(cv::Mat &inputImage, cv::Mat &outputImage, float scale)
{

	float height = inputImage.rows;
	float width = inputImage.cols;
	int out_rows = height / scale;
	int out_cols = width / scale;
	resize(inputImage, outputImage, cv::Size(out_cols, out_rows), 0, 0, INTER_LINEAR);
}

void cutRed(cv::Mat &inputImage, cv::Mat &outputImage)
{

	inputImage.copyTo(outputImage);
	int i, j;
	int cPointR, cPointG, cPointB, cPoint;//currentPoint;
	for (i = 1; i < outputImage.rows; i++)
		for (j = 1; j < outputImage.cols; j++)
		{
			cPointB = outputImage.at<Vec3b>(i, j)[0];
			cPointG = outputImage.at<Vec3b>(i, j)[1];
			cPointR = outputImage.at<Vec3b>(i, j)[2];
			if (cPointB < 50 & cPointG < 50 & cPointR>50)
			{
				outputImage.at<Vec3b>(i, j)[0] = 0;
				outputImage.at<Vec3b>(i, j)[1] = 0;
				outputImage.at<Vec3b>(i, j)[2] = 0;
			}

		}
}





























