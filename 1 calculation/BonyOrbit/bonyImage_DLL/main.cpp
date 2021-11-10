#include "main.h"
#include <opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"

using namespace std;
using namespace cv;





int _stdcall processImage(int compareLength, char *savePath)
{
	//*******************1.����ͼƬ**********************
	cv::Mat compareImage = m_currentMat.clone();

	//*******************2.����ֱ�߳���******************
	float outlength = 0;

	//*****************2.1.��������********************
	//Ϊ�˵õ���ɫͨ�����д�����תΪ��ֵͼ
	cv::Mat grayImage[3];
	split(compareImage, grayImage);
	cv::Mat grayRed = grayImage[2];
	cv::Mat binaryMat;
	cv::threshold(grayRed, binaryMat, 127, 255, THRESH_OTSU);

	//�ڵõ��Ķ�ֵͼ����Ѱ������
	vector<vector<cv::Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(binaryMat, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	//*****************2.2.ɸѡ����********************
	int m_min = 250;
	int m_max = 460;
	int a = contours.size();
	for (int i = 0; i < contours.size(); i++)
	{
		//�����С����ı���
		cv::RotatedRect rbox = minAreaRect(contours[i]);

		//�������
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
 
	//*******************3.����������********************
	if (outlength == 0) {
		return EC_NOTFIND;
	}
	cv::Mat resizeImage;
	float adjustScale = outlength / compareLength;

	sizeAdject(compareImage, resizeImage, adjustScale);
	//cv::imshow("���������ͼ��", resizeImage);


	//*******************4.ȥ���߶�**********************
	cv::Mat justImage;
	cutRed(resizeImage, justImage);
	//cv::imshow("�����ͼ��", justImage);

	//*******************5.�洢ͼ��**********************
	string currentSavePath = savePath + m_sCurrFrameName;
	cv::imwrite(currentSavePath, justImage);

	return 0;
}





int _stdcall readImage(char * imagePath)
{
	m_currentMat = cv::imread(imagePath);
	string framePath = imagePath;
	//string 
	int Tpos = framePath.find_last_of("\\"); //������\\���һ�γ��ֵ�λ�ã������أ�����0��
	m_sCurrFrameName = framePath.substr(Tpos + 1); //��ȡ��T����һλ��ʼ��ĩβ���ַ��������ļ������ƣ���ȥ��T
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





























