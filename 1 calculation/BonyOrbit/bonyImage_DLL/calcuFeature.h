#pragma once
#include "opencv2/opencv.hpp"
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;


typedef struct {
	//一般式直线方程的参数

	double A;
	double B;
	double C;

}sttLine2P;



//calcuFeature.cpp
extern "C" _declspec(dllexport) int _stdcall imageReady(char *imagePath);
extern "C" _declspec(dllexport) int _stdcall calcuArea(char *saveName); 
extern "C" _declspec(dllexport) int _stdcall diagonalRatio(char *saveName);
extern "C" _declspec(dllexport) int _stdcall partAreas(char *saveName);
extern "C" _declspec(dllexport) int _stdcall apertureWidth(char *saveName); 
extern "C" _declspec(dllexport) void _stdcall postProcess();
extern "C" _declspec(dllexport) int _stdcall calcu1vec(char *imagePath, char *saveName);
extern "C" _declspec(dllexport) int _stdcall bboxWidth(char *saveName);
extern "C" _declspec(dllexport) int _stdcall bboxInfo(char *saveName);


//计算上下对角线长度
int calcuLengthUD(cv::Mat src, float &length, std::vector<cv::Point2f> &vPtsUD);	
//计算左右对角线长度
int calcuLengthLR(cv::Mat src, cv::Mat &eyeMat, float &length, std::vector<cv::Point2f> &vPtsLR);
//三通道按取通道中最大值转为灰度图
void grayMaxChnl(cv::Mat src, cv::Mat &dst);
//将眼眶与特征点图转为只有眼眶和只有特征点的图
int sprtImage(cv::Mat src, cv::Mat &eyeMat, cv::Point2f &p1, cv::Point2f &p2);
//根据两点计算该直线与图像的两个交点
int calcuCross(cv::Mat src, cv::Point2f p1, cv::Point2f p2, cv::Point2f &pU, cv::Point2f &pD);
//聚类距离较近的点
int clusterPts(std::vector<cv::Point2f> vPts, std::vector<cv::Point2f> &outPts);

//根据两点计算直线的一般式参数 Ax+By+C=0
int calcGenLine2P(cv::Point2f p1, cv::Point2f p2, sttLine2P &para);		
//判断点在直线上方还是下方
int ptUpOrDn(cv::Point2f pt, sttLine2P &para);		
//判断点在直线左边还是右边
int ptLtOrRt(cv::Point2f pt, sttLine2P &para);		
//计算分区面积
int calcuPartArea(cv::Mat eyeMat, std::vector<float> &partAreas, sttLine2P L1, sttLine2P L2);	

//在左右对角线特征点中找颧额缝交点
int findSeamPt(std::vector<cv::Point2f> &vPts, int flag);
//找到同水平线的轮廓交点并计算宽度输出
int calcuAptWidth(cv::Mat src, cv::Point2f inPt, cv::Point2f &outPt, 
					float &width, vector<vector<cv::Point>> &contours);
//两水平点之间十等分
int tenEqual(cv::Point p1, cv::Point p2, vector<cv::Point> &vPts, int flag);
//计算各等分点到上下轮廓的距离
int calcuDvsHt(vector<vector<cv::Point>> contours, vector<cv::Point> equalPts,
				vector<float> &vHts, float coff);

//计算距离一维向量
int oneVec(cv::Mat src, float *ary);

//根据BBOX中心点找到眼眶外侧的的边缘点
int findEdgePts(cv::Mat src, cv::Point2f center, int flag, cv::Point2f &outPt);


float m_pixelScale;					//图像像素与实际距离的比例
									//真实面积=s*49/40000
									//真实长度=l*7/20
cv::Mat m_crtMat;					//当前图像
cv::Mat m_rtMat;					//右侧眼眶图像  二值图
cv::Mat m_ltMat;					//左侧眼眶图像
string m_sCrtFrameName;				//当前图像的名字
ofstream m_file;					//保存眼眶信息
cv::Mat m_ltEyeMat;					//左侧只有眼眶图像
cv::Mat m_rtEyeMat;					//右侧只有眼眶图像


std::vector<float> m_vAreas;			//存放左右眼眶的连通域面积，先左后右（图像中的角度来看）
std::vector<cv::Rect> m_vBbox;			//左右眼眶的BBOX
std::vector<cv::Point2f> m_vCtrPts;		//存放左右眼眶的BBOX的中心点，先左后右（图像中的角度来看）
std::vector<cv::Point2f> m_vPtsUDL;		//左眼眶上下对角点
std::vector<cv::Point2f> m_vPtsLRL;		//左眼眶左右对角点
std::vector<cv::Point2f> m_vPtsUDR;		//右眼眶上下对角点
std::vector<cv::Point2f> m_vPtsLRR;		//右眼眶左右对角点







#define COMPTHRE 500				//分离眼眶和小东西连通域的阈值

#define EC_NOIMAGE 10001			//有没有读到图
#define EC_RIGHTANDLEFT 10002		//包含两个眼睛

#define EC_NOTEYEMAT  20001			//单纯眼眶图像中不止一个连通域
#define EC_FEATUREOVER 20002		//不是两个小特征		
#define EC_NOCROSSIMAGE 20003		//两特征点直线与图像不是两个交点
#define EC_NOT2CROSS 20004			//直线与眼眶轮廓交点不是两个

#define EC_NOT2POINT 30001			//点聚类的输入少于两个点

