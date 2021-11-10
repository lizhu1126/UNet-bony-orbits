#pragma once
#include "opencv2/opencv.hpp"
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;


typedef struct {
	//һ��ʽֱ�߷��̵Ĳ���

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


//�������¶Խ��߳���
int calcuLengthUD(cv::Mat src, float &length, std::vector<cv::Point2f> &vPtsUD);	
//�������ҶԽ��߳���
int calcuLengthLR(cv::Mat src, cv::Mat &eyeMat, float &length, std::vector<cv::Point2f> &vPtsLR);
//��ͨ����ȡͨ�������ֵתΪ�Ҷ�ͼ
void grayMaxChnl(cv::Mat src, cv::Mat &dst);
//���ۿ���������ͼתΪֻ���ۿ���ֻ���������ͼ
int sprtImage(cv::Mat src, cv::Mat &eyeMat, cv::Point2f &p1, cv::Point2f &p2);
//������������ֱ����ͼ�����������
int calcuCross(cv::Mat src, cv::Point2f p1, cv::Point2f p2, cv::Point2f &pU, cv::Point2f &pD);
//�������Ͻ��ĵ�
int clusterPts(std::vector<cv::Point2f> vPts, std::vector<cv::Point2f> &outPts);

//�����������ֱ�ߵ�һ��ʽ���� Ax+By+C=0
int calcGenLine2P(cv::Point2f p1, cv::Point2f p2, sttLine2P &para);		
//�жϵ���ֱ���Ϸ������·�
int ptUpOrDn(cv::Point2f pt, sttLine2P &para);		
//�жϵ���ֱ����߻����ұ�
int ptLtOrRt(cv::Point2f pt, sttLine2P &para);		
//����������
int calcuPartArea(cv::Mat eyeMat, std::vector<float> &partAreas, sttLine2P L1, sttLine2P L2);	

//�����ҶԽ�������������ȧ��콻��
int findSeamPt(std::vector<cv::Point2f> &vPts, int flag);
//�ҵ�ͬˮƽ�ߵ��������㲢���������
int calcuAptWidth(cv::Mat src, cv::Point2f inPt, cv::Point2f &outPt, 
					float &width, vector<vector<cv::Point>> &contours);
//��ˮƽ��֮��ʮ�ȷ�
int tenEqual(cv::Point p1, cv::Point p2, vector<cv::Point> &vPts, int flag);
//������ȷֵ㵽���������ľ���
int calcuDvsHt(vector<vector<cv::Point>> contours, vector<cv::Point> equalPts,
				vector<float> &vHts, float coff);

//�������һά����
int oneVec(cv::Mat src, float *ary);

//����BBOX���ĵ��ҵ��ۿ����ĵı�Ե��
int findEdgePts(cv::Mat src, cv::Point2f center, int flag, cv::Point2f &outPt);


float m_pixelScale;					//ͼ��������ʵ�ʾ���ı���
									//��ʵ���=s*49/40000
									//��ʵ����=l*7/20
cv::Mat m_crtMat;					//��ǰͼ��
cv::Mat m_rtMat;					//�Ҳ��ۿ�ͼ��  ��ֵͼ
cv::Mat m_ltMat;					//����ۿ�ͼ��
string m_sCrtFrameName;				//��ǰͼ�������
ofstream m_file;					//�����ۿ���Ϣ
cv::Mat m_ltEyeMat;					//���ֻ���ۿ�ͼ��
cv::Mat m_rtEyeMat;					//�Ҳ�ֻ���ۿ�ͼ��


std::vector<float> m_vAreas;			//��������ۿ�����ͨ�������������ң�ͼ���еĽǶ�������
std::vector<cv::Rect> m_vBbox;			//�����ۿ���BBOX
std::vector<cv::Point2f> m_vCtrPts;		//��������ۿ���BBOX�����ĵ㣬������ң�ͼ���еĽǶ�������
std::vector<cv::Point2f> m_vPtsUDL;		//���ۿ����¶Խǵ�
std::vector<cv::Point2f> m_vPtsLRL;		//���ۿ����ҶԽǵ�
std::vector<cv::Point2f> m_vPtsUDR;		//���ۿ����¶Խǵ�
std::vector<cv::Point2f> m_vPtsLRR;		//���ۿ����ҶԽǵ�







#define COMPTHRE 500				//�����ۿ���С������ͨ�����ֵ

#define EC_NOIMAGE 10001			//��û�ж���ͼ
#define EC_RIGHTANDLEFT 10002		//���������۾�

#define EC_NOTEYEMAT  20001			//�����ۿ�ͼ���в�ֹһ����ͨ��
#define EC_FEATUREOVER 20002		//��������С����		
#define EC_NOCROSSIMAGE 20003		//��������ֱ����ͼ������������
#define EC_NOT2CROSS 20004			//ֱ�����ۿ��������㲻������

#define EC_NOT2POINT 30001			//��������������������

