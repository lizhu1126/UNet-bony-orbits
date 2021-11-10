#include "calcuFeature.h"
#include <opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"


using namespace std;
using namespace cv;


/****************************************************
功能：准备图像
输入：图像路径
输出：无

全局变量：原始图像  m_crtMat
两张单独眼眶的图像  m_rtMat  m_ltMat

备注：在进行特征计算之前都必须经过此函数
******************************************************/
int _stdcall imageReady(char * imagePath)
{
	m_crtMat = cv::imread(imagePath);
	string framePath = imagePath;

	//string 
	size_t Tpos = framePath.find_last_of("\\");			//查找在\\最后一次出现的位置，并返回（包括0）
	m_sCrtFrameName = framePath.substr(Tpos + 1);		//从指定位置复制子字符串，即文件的名称，且去掉前面的

	if (m_crtMat.empty()) {
		return EC_NOIMAGE;
	}

	//定义图像容器
	cv::Mat gryMat;
	cv::Mat bnyMat;
	cv::Mat disMat;
	cv::Mat sttMat;
	cv::Mat cntMat;
	cv::Mat lblMat;
	m_rtMat = cv::Mat::zeros(m_crtMat.rows, m_crtMat.cols, CV_8UC3);		//之前为单通道 20201230修改
	m_ltMat = cv::Mat::zeros(m_crtMat.rows, m_crtMat.cols, CV_8UC3);

	//灰度化
	grayMaxChnl(m_crtMat, gryMat);

	//二值化
	cv::threshold(gryMat, bnyMat, 100, 255, THRESH_OTSU);

	////反色，若底色为黑则不需要反色
	//bnyMat = 255 - bnyMat;

	//连通域
	int nComp = connectedComponentsWithStats(bnyMat, lblMat, sttMat, cntMat);

	//0号为背景，跳过，i=1开始循环
	std::vector<cv::Rect> vBbox;
	std::vector<cv::Mat> vMat;
	std::vector<int> vArea;
	std::vector<cv::Point2f> vPts;
	for (int i = 1; i < nComp; i++) {
		if (sttMat.at<int>(i, CC_STAT_AREA) > COMPTHRE)
		{
			//绘制bounding box
			Rect bbox;
			cv::Point2f center;
			cv::Mat roi;
			//bounding box左上角坐标
			//bbox.x = sttMat.at<int>(i, 0) - 20;		//左右扩大20个像素，将两个标志圈进去
			bbox.x = sttMat.at<int>(i, 0);
			bbox.y = sttMat.at<int>(i, 1);
			//bouding box的宽和长 
			//bbox.width = sttMat.at<int>(i, 2) + 40;
			bbox.width = sttMat.at<int>(i, 2);
			bbox.height = sttMat.at<int>(i, 3);
			//中心点
			center.x = bbox.x + bbox.width / 2;
			center.y = bbox.y + bbox.height / 2;
			
			//roi = bnyMat(bbox); 
			roi = m_crtMat(bbox);		//20201230修改
			vMat.push_back(roi); 
			vBbox.push_back(bbox);
			vPts.push_back(center);
			vArea.push_back(sttMat.at<int>(i, CC_STAT_AREA));

			////绘制
			//rectangle(m_crtMat, bbox, CV_RGB(255, 255, 0), 2, 8, 0);
		}
	}

	//依次取出左眼眶和右眼眶
	if (vBbox.size() != 2) return EC_RIGHTANDLEFT;

	if (vBbox[0].x < vBbox[1].x) {
		//0位置为左眼眶
		vMat[0].copyTo(m_ltMat(vBbox[0]));
		vMat[1].copyTo(m_rtMat(vBbox[1]));
		m_vAreas.push_back(vArea[0]);
		m_vAreas.push_back(vArea[1]);
		m_vCtrPts.push_back(vPts[0]);
		m_vCtrPts.push_back(vPts[1]);
		m_vBbox.push_back(vBbox[0]);
		m_vBbox.push_back(vBbox[1]);
	}
	else {
		//0位置为右眼眶
		vMat[0].copyTo(m_rtMat(vBbox[0]));
		vMat[1].copyTo(m_ltMat(vBbox[1]));
		m_vAreas.push_back(vArea[1]);
		m_vAreas.push_back(vArea[0]);
		m_vCtrPts.push_back(vPts[1]);
		m_vCtrPts.push_back(vPts[0]);
		m_vBbox.push_back(vBbox[1]);
		m_vBbox.push_back(vBbox[0]);
	}

	
	return 0;
}


/****************************************************
功能：计算左右眼眶连通域的面积
输入：txt保存的名字（例如“青年_男”“老年_女”等）
输出：无参数传递到前端，但是会在本地保存txt文件

全局变量：每张图片的名字   m_sCrtFrameName
		  存放左右眼眶面积的向量   m_vAreas

备注：计算面积并保存到txt
      面积计算公式：S=连通域面积*49/40000
******************************************************/
int _stdcall calcuArea(char *saveName)
{
	//将图像中的左右连通域信息保存到本地txt中
	string name = saveName;
	string nameString = name.append("_leftArea.txt");
	m_file.open(nameString, ios::app);

	float realAeraL = m_vAreas[0] * 49 / 40000;
	m_file << m_sCrtFrameName << "：" << realAeraL << endl;
	m_file.close();
	

	name = saveName;
	nameString = name.append("_rightArea.txt");
	m_file.open(nameString, ios::app);
	 
	float realAeraR = m_vAreas[1] * 49 / 40000;
	m_file << m_sCrtFrameName << "：" << realAeraR << endl;
	m_file.close();

	return 0;
}


/****************************************************
功能：计算眼眶对角线的长度及比值
输入：txt保存的名字（例如“青年_男”“老年_女”等）
输出：无参数传递到前端，但是会在本地保存txt文件

全局变量：每张图片的名字   m_sCrtFrameName
		  两张单独眼眶的图像  m_rtMat  m_ltMat

备注：计算对角线长度及比值并保存到txt
******************************************************/
int _stdcall diagonalRatio(char *saveName)
{
	float lengthUDL = 0;		//左眼眶上下对角线长度
	float lengthLRL = 0;		//左眼眶左右对角线长度
	float ratioL = 0;			//左眼眶上下与左右的比值
	float lengthUDR = 0;		//右眼眶
	float lengthLRR = 0;
	float ratioR = 0;
	int ret = 0;
	float coff = 7.0 / 20.0;

	//------------计算左眼眶上下及左右对角线长度及比值------------------------------
	ret = calcuLengthUD(m_ltMat, lengthUDL, m_vPtsUDL);
	ret = calcuLengthLR(m_ltMat, m_ltEyeMat, lengthLRL, m_vPtsLRL);
	if (ret != 0) {
		return ret;
	}
	ratioL = lengthUDL / lengthLRL;			//计算左眼眶对角线比值

	string name = saveName;
	string nameString = name.append("_leftDiagonal.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "：上下：" << lengthUDL * coff << "：左右：" << lengthLRL * coff << "：上下与左右比值：" << ratioL << endl;
	m_file.close();


	//------------计算右眼眶上下及左右对角线长度及比值-------------------------------
	ret = calcuLengthUD(m_rtMat, lengthUDR, m_vPtsUDR);
	ret = calcuLengthLR(m_rtMat, m_rtEyeMat, lengthLRR, m_vPtsLRR);
	if (ret != 0) {
		return ret;
	}
	ratioR = lengthUDR / lengthLRR;			//计算右眼眶对角线比值

	name = saveName;
	nameString = name.append("_rightDiagonal.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "：上下：" << lengthUDR * coff << "：左右：" << lengthLRR * coff << "：上下与左右比值：" << ratioR << endl;
	m_file.close();


	return 0;
}


//int _stdcall diagonalRatio(char *saveName)
//{
//	float lengthUDL = 0;		//左眼眶上下对角线长度
//	float lengthLRL = 0;		//左眼眶左右对角线长度
//	float ratioL = 0;			//左眼眶上下与左右的比值
//	float lengthUDR = 0;		//右眼眶
//	float lengthLRR = 0;
//	float ratioR = 0;
//	int ret = 0;
//	float coff = 7.0 / 20.0;
//
//	//------------计算左眼眶上下及左右对角线长度及比值------------------------------
//	ret = calcuLengthUD(m_ltMat, lengthUDL, m_vPtsUDL);
//	if (ret != 0) {
//		return ret;
//	}
//
//	string name = saveName;
//	string nameString = name.append("_leftDiagonal.txt");
//	m_file.open(nameString, ios::app);
//	m_file << m_sCrtFrameName << "：上下：" << lengthUDL * coff << endl;
//	m_file.close();
//
//
//	//------------计算右眼眶上下及左右对角线长度及比值-------------------------------
//	ret = calcuLengthUD(m_rtMat, lengthUDR, m_vPtsUDR);
//	if (ret != 0) {
//		return ret;
//	}
//
//	name = saveName;
//	nameString = name.append("_rightDiagonal.txt");
//	m_file.open(nameString, ios::app);
//	m_file << m_sCrtFrameName << "：上下：" << lengthUDR * coff << endl;
//	m_file.close();
//
//
//	return 0;
//}


/****************************************************
功能：计算左右眼眶各分区的面积
输入：txt保存的名字（例如“青年_男”“老年_女”等）
输出：无参数传递到前端，但是会在本地保存txt文件

备注：计算分区面积并保存到txt
读取顺序从左到右从上到下
左眼眶顺序：S2,S1,S3,S4
右眼眶顺序：S1,S2,S4,S3
方法：
1、计算得到两条对角线；
2、计算被两条对角线分割为四个区域的眼眶面积。
******************************************************/
int _stdcall partAreas(char *saveName)
{
	int ret = 0;
	sttLine2P L1, L2;				//两条对角线
	std::vector<float> partAreas;	//存放分区面积向量
	float coff = 49.0 / 40000.0;	//计算面积的比例尺

	//------------计算左眼眶各分区面积-------------------------------
	//计算两条对角线
	calcGenLine2P(m_vPtsUDL[0], m_vPtsUDL[1], L1);		//上下对角线
	calcGenLine2P(m_vPtsLRL[0], m_vPtsLRL[1], L2);		//左右对角线
	////////////////////////////////////////////////////////////////
	cv::Mat showMat;
	cvtColor(m_ltEyeMat, showMat, CV_GRAY2BGR);
	cv::circle(showMat, m_vPtsUDL[0], 3, Scalar(255, 255, 255), 3, 8, 0);
	cv::circle(showMat, m_vPtsUDL[1], 3, Scalar(255, 255, 255), 3, 8, 0);
	line(showMat, m_vPtsUDL[0], m_vPtsUDL[1], Scalar(0, 0, 255), 1, 8, 0);
	line(showMat, m_vPtsLRL[0], m_vPtsLRL[1], Scalar(0, 255, 0), 1, 8, 0);
	///////////////////////////////////////////////////////////////
	//计算分区面积
	calcuPartArea(m_ltEyeMat, partAreas, L1, L2);

	string name = saveName;
	string nameString = name.append("_leftPartAreas.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "：S1：" << partAreas[1] * coff << "：S2：" << partAreas[0] * coff << "：S3：" << partAreas[2] * coff << "：S4：" << partAreas[3] * coff << endl;
	m_file.close();


	//------------计算右眼眶各分区面积-------------------------------
	//计算两条对角线
	calcGenLine2P(m_vPtsUDR[0], m_vPtsUDR[1], L1);		//上下对角线
	calcGenLine2P(m_vPtsLRR[0], m_vPtsLRR[1], L2);		//左右对角线
	//计算分区面积
	calcuPartArea(m_rtEyeMat, partAreas, L1, L2);
	////////////////////////////////////////////////////////////////
	cv::Mat showMat1;
	cvtColor(m_rtEyeMat, showMat1, CV_GRAY2BGR);
	cv::circle(showMat1, m_vPtsUDR[0], 3, Scalar(255, 255, 255), 3, 8, 0);
	cv::circle(showMat1, m_vPtsUDR[1], 3, Scalar(255, 255, 255), 3, 8, 0);
	line(showMat1, m_vPtsUDR[0], m_vPtsUDR[1], Scalar(0, 0, 255), 1, 8, 0);
	line(showMat1, m_vPtsLRR[0], m_vPtsLRR[1], Scalar(0, 255, 0), 1, 8, 0);
	///////////////////////////////////////////////////////////////
	name = saveName;
	nameString = name.append("_rightPartAreas.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "：S1：" << partAreas[4] * coff << "：S2：" << partAreas[5] * coff << "：S3：" << partAreas[7] * coff << "：S4：" << partAreas[6] * coff << endl;
	m_file.close();

	return 0;
}




/****************************************************
功能：计算左右眼眶眶口宽度及横轴上等分点至眶上缘和眶下缘的垂直距离
输入：txt保存的名字（例如“青年_男”“老年_女”等）
输出：无参数传递到前端，但是会在本地保存txt文件

备注：
需计算——眶口宽度、等分点至上缘和下缘的距离
方法：
1、找到颧额缝特征点（左右眼不同）；
2、找到y值相同的轮廓上的点，计算宽度输出；
3、两点间的水平线做十等分点（9条线）
4、各等分点的x轴一定，找到轮廓上对应的两个y值并计算距离。
******************************************************/
int _stdcall apertureWidth(char *saveName)
{
	int ret = 0;
	float aptWidthL = 0;					//左眼眶的眶口宽度
	float aptWidthR = 0;					//右眼眶的眶口宽度
	cv::Point2f horiLtPt;					//左眼眶与颧额缝点水平的轮廓点
	cv::Point2f horiRtPt;					//右眼眶与颧额缝点水平的轮廓点
	vector<cv::Point> equalPtsL;			//存放左眼眶的十等分点
	vector<cv::Point> equalPtsR;			//存放右眼眶的十等分点
	vector<float> vHtsL;					//存放左眼眶等分点至眶上下缘的垂直距离
	vector<float> vHtsR;					//存放右眼眶等分点至眶上下缘的垂直距离
	vector<vector<cv::Point>> contoursL;	//左眼眶的轮廓
	vector<vector<cv::Point>> contoursR;	//右眼眶的轮廓
	float coff = 7.0 / 20.0;				//长度的比例尺


	//------------计算左眼眶眶口宽度等-------------------------------
	//在左右对角点向量中筛选出颧额缝点
	findSeamPt(m_vPtsLRL, 1);
	//找到同水平线的轮廓交点并计算宽度输出
	calcuAptWidth(m_ltEyeMat, m_vPtsLRL[0], horiLtPt, aptWidthL, contoursL);
	//两点之间做十等分点
	tenEqual(m_vPtsLRL[0], horiLtPt, equalPtsL, 1);
	//计算各等分点到上下轮廓的距离
	calcuDvsHt(contoursL, equalPtsL, vHtsL, coff);
	///////检验是否正确用///////////////////////////////////////////////////////////
	//cv::Mat showMat;
	//cvtColor(m_ltEyeMat, showMat, CV_GRAY2BGR);
	//cv::line(showMat, m_vPtsLRL[0], horiLtPt, Scalar(255, 0, 0), 1, 8, 0);
	//for (int i = 0; i < equalPtsL.size(); i++) {
	//	cv::line(showMat, equalPtsL[i], Point(equalPtsL[i].x, equalPtsL[i].y - vHtsL[2 * i]/coff), Scalar(255, 255, 0), 1, 8, 0);
	//	cv::line(showMat, equalPtsL[i], Point(equalPtsL[i].x, equalPtsL[i].y - vHtsL[2 * i + 1]/coff), Scalar(0, 255, 255), 1, 8, 0);
	//}
	//for (int i = 0; i < equalPtsL.size(); i++) {
	//	cv::circle(showMat, equalPtsL[i], 1, Scalar(0, 0, 255), 1, 8, 0);
	//}
	//cv::circle(showMat, m_vPtsLRL[0], 1, Scalar(0, 0, 255), 1, 8, 0);
	//cv::circle(showMat, horiLtPt, 1, Scalar(0, 0, 255), 1, 8, 0);
	//////////////////////////////////////////////////////////////////
	
	//横轴宽度写入文件
	string name = saveName;
	string nameString = name.append("_leftApeWidth.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "：width：" << aptWidthL * coff << endl;
	m_file.close();   
	//各等分点到眶上缘距离写入文件
	name = saveName;
	nameString = name.append("_leftHeightS.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName<<"：short：";
	for (int i = 0; i < 9; i++) {
		m_file << abs(vHtsL[2 * i]) << "：";
	}
	m_file << endl;
	m_file.close();
	//各等分点到眶下缘距离写入文件
	name = saveName;
	nameString = name.append("_leftHeightH.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "：high：";
	for (int i = 0; i < 9; i++) {
		m_file << abs(vHtsL[2 * i + 1]) << "：";
	}
	m_file << endl;
	m_file.close();

	//------------计算右眼眶眶口宽度等-------------------------------
	//在左右对角点向量中筛选出颧额缝点
	findSeamPt(m_vPtsLRR, 2);
	//找到同水平线的轮廓交点并计算宽度输出
	calcuAptWidth(m_rtEyeMat, m_vPtsLRR[0], horiRtPt, aptWidthR, contoursR);
	//两点之间做十等分点
	tenEqual(m_vPtsLRR[0], horiRtPt, equalPtsR, 2);
	//计算各等分点到上下轮廓的距离
	calcuDvsHt(contoursR, equalPtsR, vHtsR, coff);

	//横轴宽度写入文件
	name = saveName;
	nameString = name.append("_rightApeWidth.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "：width：" << aptWidthR * coff << endl;
	m_file.close();
	//各等分点到眶上缘距离写入文件
	name = saveName;
	nameString = name.append("_rightHeightS.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "：short：";
	for (int i = 0; i < 9; i++) {
		m_file << abs(vHtsR[2 * i]) << "：";
	}
	m_file << endl;
	m_file.close();
	//各等分点到眶下缘距离写入文件
	name = saveName;
	nameString = name.append("_rightHeightH.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "：high：";
	for (int i = 0; i < 9; i++) {
		m_file << abs(vHtsR[2 * i + 1]) << "：";
	}
	m_file << endl;
	m_file.close();

	///////检验是否正确用///////////////////////////////////////////////////////////
	//cv::Mat showMat1;
	//cvtColor(m_rtEyeMat, showMat1, CV_GRAY2BGR);
	//cv::line(showMat1, m_vPtsLRR[0], horiRtPt, Scalar(255, 0, 0), 1, 8, 0);
	//for (int i = 0; i < equalPtsR.size(); i++) {
	//	cv::line(showMat1, equalPtsR[i], Point(equalPtsR[i].x, equalPtsR[i].y - vHtsR[2 * i] / coff), Scalar(255, 255, 0), 1, 8, 0);
	//	cv::line(showMat1, equalPtsR[i], Point(equalPtsR[i].x, equalPtsR[i].y - vHtsR[2 * i + 1] / coff), Scalar(0, 255, 255), 1, 8, 0);
	//}
	//for (int i = 0; i < equalPtsR.size(); i++) {
	//	cv::circle(showMat1, equalPtsR[i], 1, Scalar(0, 0, 255), 1, 8, 0);
	//}
	//cv::circle(showMat1, m_vPtsLRR[0], 1, Scalar(0, 0, 255), 1, 8, 0);
	//cv::circle(showMat1, horiRtPt, 1, Scalar(0, 0, 255), 1, 8, 0);
	//////////////////////////////////////////////////////////////////

 	return 0;
}


/****************************************************
功能：将左右眼眶的BBOX的宽度高度传出去
输入：txt保存的名字（例如“青年_男”“老年_女”等）
输出：无参数传递到前端，但是会在本地保存txt文件

备注：已经是计算好的全局变量，只需要传出去就行
******************************************************/
int _stdcall bboxInfo(char *saveName)
{
	//将图像中的左右BBOX信息保存到本地txt中
	string name = saveName;
	string nameString = name.append("_leftHW.txt");
	m_file.open(nameString, ios::app);

	m_file << m_sCrtFrameName << "：" << "height：" << m_vBbox[0].height * 7.0 / 20.0 << "：width：" << m_vBbox[0].width * 7.0 / 20.0 << endl;
	m_file.close();


	name = saveName;
	nameString = name.append("_rightHW.txt");
	m_file.open(nameString, ios::app);

	m_file << m_sCrtFrameName << "：" << "height：" << m_vBbox[1].height * 7.0 / 20.0 << "：width：" << m_vBbox[1].width * 7.0 / 20.0 << endl;
	m_file.close();



	return 0;
}




/****************************************************
功能：计算左右眼眶BBOX中心点对应水平线上的九等分
输入：txt保存的名字（例如“青年_男”“老年_女”等）
输出：无参数传递到前端，但是会在本地保存txt文件

备注：
需计算——等分点至上缘和下缘的距离
方法：
1、找到中心点对应的左右两个边缘点；
2、找到y值相同的轮廓上的点，计算宽度输出；
3、两点间的水平线做十等分点（9条线）
4、各等分点的x轴一定，找到轮廓上对应的两个y值并计算距离。
******************************************************/
int _stdcall bboxWidth(char *saveName)
{
	int ret = 0;
	float aptWidthL = 0;					//左眼眶的眶口宽度
	float aptWidthR = 0;					//右眼眶的眶口宽度
	cv::Point2f horiLtPts;					//左眼眶水平线外侧边缘点
	cv::Point2f horiLtPtm;					//左眼眶水平线内侧边缘点
	cv::Point2f horiRtPts;					//右眼眶水平线内侧边缘点
	cv::Point2f horiRtPtm;					//右眼眶水平线外侧边缘点
	vector<cv::Point> equalPtsL;			//存放左眼眶的十等分点
	vector<cv::Point> equalPtsR;			//存放右眼眶的十等分点
	vector<float> vHtsL;					//存放左眼眶等分点至眶上下缘的垂直距离
	vector<float> vHtsR;					//存放右眼眶等分点至眶上下缘的垂直距离
	vector<vector<cv::Point>> contoursL;	//左眼眶的轮廓
	vector<vector<cv::Point>> contoursR;	//右眼眶的轮廓
	float coff = 7.0 / 20.0;				//长度的比例尺

	////BBOX画出来看看
	//cv::Mat src = m_crtMat;
	//rectangle(src, m_vBbox[0], CV_RGB(255, 255, 0), 1, 8, 0);
	//circle(src, m_vCtrPts[0], 1, Scalar(0, 0, 255), 1, 8, 0);
	//rectangle(src, m_vBbox[1], CV_RGB(255, 255, 0), 1, 8, 0);
	//circle(src, m_vCtrPts[1], 1, Scalar(0, 0, 255), 1, 8, 0);

	//------------计算左眼眶九等分-------------------------------
	//先根据BBOX中心点做水平线找到眼眶的外侧边缘点
	findEdgePts(m_ltMat, m_vCtrPts[0], 1, horiLtPts);
	//找到同水平线的轮廓交点并计算宽度输出
	calcuAptWidth(m_ltMat, horiLtPts, horiLtPtm, aptWidthL, contoursL);
	//两点之间做十等分点
	tenEqual(horiLtPts, horiLtPtm, equalPtsL, 1);
	//计算各等分点到上下轮廓的距离
	calcuDvsHt(contoursL, equalPtsL, vHtsL, coff);

	//各等分点到眶上缘距离写入文件
	string name = saveName;
	string nameString = name.append("_leftHeightS.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "：short：";
	for (int i = 0; i < 9; i++) {
		m_file << abs(vHtsL[2 * i]) << "：";
	}
	m_file << endl;
	m_file.close();
	//各等分点到眶下缘距离写入文件
	name = saveName;
	nameString = name.append("_leftHeightH.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "：high：";
	for (int i = 0; i < 9; i++) {
		m_file << abs(vHtsL[2 * i + 1]) << "：";
	}
	m_file << endl;
	m_file.close();



	//------------计算右眼眶九等分-------------------------------
	//先根据BBOX中心点做水平线找到眼眶的外侧边缘点
	findEdgePts(m_rtMat, m_vCtrPts[1], 2, horiRtPts);
	//找到同水平线的轮廓交点并计算宽度输出
	calcuAptWidth(m_rtMat, horiRtPts, horiRtPtm, aptWidthR, contoursR);
	//两点之间做十等分点
	tenEqual(horiRtPts, horiRtPtm, equalPtsR, 2);
	//计算各等分点到上下轮廓的距离
	calcuDvsHt(contoursR, equalPtsR, vHtsR, coff);

	//各等分点到眶上缘距离写入文件
	name = saveName;
	nameString = name.append("_rightHeightS.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "：short：";
	for (int i = 0; i < 9; i++) {
		m_file << abs(vHtsR[2 * i]) << "：";
	}
	m_file << endl;
	m_file.close();
	//各等分点到眶下缘距离写入文件
	name = saveName;
	nameString = name.append("_rightHeightH.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "：high：";
	for (int i = 0; i < 9; i++) {
		m_file << abs(vHtsR[2 * i + 1]) << "：";
	}
	m_file << endl;
	m_file.close();



	return 0;
}







/****************************************************
功能：计算单侧眼眶的一维距离向量
输入：txt保存的名字（例如“青年_男”“老年_女”等）
输出：无参数传递到前端，但是会在本地保存txt文件

全局变量：每张图片的名字   m_sCrtFrameName

备注：计算一个眼眶从BBOX中心360度到轮廓边缘的距离，
	  展开为一维向量，存储在txt中
******************************************************/
int _stdcall calcu1vec(char *imagePath, char *saveName)
{
	cv::Mat src = cv::imread(imagePath);

	string framePath = imagePath;
	size_t Tpos = framePath.find_last_of("\\");			//查找在\\最后一次出现的位置，并返回（包括0）
	m_sCrtFrameName = framePath.substr(Tpos + 1);		//从指定位置复制子字符串，即文件的名称，且去掉前面的

	float dist[360] = { 0 };
	int ret = oneVec(src, dist);		//计算一维向量
	if (ret != 0) return 1;


	string name = saveName;
	string nameString = name.append("RIGHT.txt");
	m_file.open(nameString, ios::app);
	m_file << "3, " << dist[0] << ", ";
	for (int i = 1; i < 359; i++)
	{
		m_file << dist[i] << ", ";
	}
	m_file << dist[359] << endl;
	m_file.close();


	return 0;
}











/****************************************************
功能：计算眼眶上下对角线的长度
输入：src  需要计算的眼眶       //20201230改为三通道
输出：返回值即为上下对角线的长度

备注：对角线上端点：第一行有值中点
	  对角线下端点：最后一行有值中点
******************************************************/
int calcuLengthUD(cv::Mat src, float &length, std::vector<cv::Point2f> &vPtsUD)
{
	cv::Mat grayMat;
	cv::Mat bnyMat;
	if (src.channels() == 3) {
		grayMaxChnl(src, grayMat);
		threshold(grayMat, bnyMat, 50, 255, THRESH_OTSU);
	}
	else {
		threshold(src, bnyMat, 50, 255, THRESH_OTSU);
	}

	cv::Point upP, downP;
	bool flag = false;

	int rowNum = bnyMat.rows;
	int colNum = bnyMat.cols;

	//1：从头遍历找第一行有值的中点即为最上端点
	//先找到第一行的起点
	for (int i = 0; i < rowNum; i++) {
		uchar* data = bnyMat.ptr<uchar>(i);
		for (int j = 0; j < colNum; j++) {
			if (data[j] == 255) {
				upP.x = j;
				upP.y = i;
				flag = true;
				break;
			}
		}
		if (flag)
			break;
	}
	//统计这一行有几个值为255的
	int cnt = 0;
	uchar* dataUP = bnyMat.ptr<uchar>(upP.y);
	for (int j = 0; j < colNum; j++) {
		if (dataUP[j] == 255) {
			cnt++;
		}
	}
	upP.x = upP.x + cnt / 2;

	
	//2：从尾遍历找最后一行有值的中点即为最下端点
	//先找到最后一行的起点
	flag = false;
	for (int i = rowNum - 1; i >= 0; i--) {
		uchar* data = bnyMat.ptr<uchar>(i);
		for (int j = colNum - 1; j >= 0; j--) {
			if (data[j] == 255) {
				downP.x = j;
				downP.y = i;
				flag = true;
				break;
			}
		}
		if (flag)
			break;
	}
	//统计这一行有几个值为255的
	cnt = 0;
	uchar* dataDN = bnyMat.ptr<uchar>(downP.y);
	for (int j = colNum - 1; j >= 0; j--) {
		if (dataDN[j] == 255) {
			cnt++;
		}
	}
	downP.x = downP.x - cnt / 2;

	//给全局变量
	vPtsUD.push_back(upP);
	vPtsUD.push_back(downP);

	//3：计算两个点的长度
	length = std::pow(std::pow(upP.x - downP.x, 2) + std::pow(upP.y - downP.y, 2), 0.5);


	return 0;
}




/****************************************************
功能：计算眼眶左右对角线的长度
输入：src  需要计算的眼眶  
输出：eyeMat   只有眼眶的图像，无两个特征点
返回值即为左右对角线的长度

备注：
对角线左右端点：眶上颌额点与颧额点（根据小连通域）
方法：
1、得到两个标注的特征点；
2、计算这两个点的线段与图像的交点得到长直线；
3、遍历该直线找到与眼眶轮廓的两个交点。
******************************************************/
int calcuLengthLR(cv::Mat src, cv::Mat &eyeMat, float &length, std::vector<cv::Point2f> &vPtsLR)
{
	//两个特征点的坐标
	cv::Point2f p1;		
	cv::Point2f p2;
	int ret = 0;

	//得到单纯眼眶图及两个特征点
	ret = sprtImage(src, eyeMat, p1, p2);
	if (ret != 0) return ret;

	//if (src.channels() == 3) {
	//	grayMaxChnl(src, grayMat);
	//	threshold(grayMat, bnyMat, 50, 255, THRESH_OTSU);
	//}
	//else {
	//	threshold(src, bnyMat, 50, 255, THRESH_OTSU);
	//}

	//在二值眼眶图像中寻找眼眶轮廓
	vector<vector<cv::Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(eyeMat, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	int a = contours.size();

	if (contours.size() != 1) {
		return EC_NOTEYEMAT;
	}
	////////////最小外界矩形//////////////////////////////////
	//cv::Mat dst;
	//cvtColor(eyeMat, dst, COLOR_GRAY2BGR);
	//RotatedRect minrect = minAreaRect(contours[0]);	//最小外接矩形
	//Point2f vertices[4];      //定义4个点的数组
	//minrect.points(vertices);   //将四个点存储到vertices数组中
	//for (int i = 0; i < 4; i++)
	//{ 
	//	// 注意Scala中存储顺序 BGR
	//	line(dst, vertices[i], vertices[(i + 1) % 4], Scalar(0, 255, 0), 2);
	//}
	//cv::imwrite("dst.png", dst);

	//////////////////////////////////////////////////////
	//cv::Mat img(src.rows, src.cols, src.type());
	//for (int i = 0; i < contours[0].size(); i++) {
	//	circle(img, contours[0][i], 1, Scalar(255, 255, 255), -1);
	//}
	//drawContours(src, contours, 0, Scalar(255, 0, 0), 1, 8);
	//drawContours(src, contours, 1, Scalar(255, 0, 255), 3, 8);
	////获得最小外界四边形
	//cv::RotatedRect rbox = minAreaRect(contours[0]);

	//求直线方程与图的交点
	cv::Point2f pU, pD;		//俩特征点直线与图的交点
	ret = calcuCross(src, p1, p2, pU, pD);
	if (ret != 0) return ret;
	
	//遍历两点之间的线段，得到和轮廓的交点
	LineIterator it(eyeMat, pU, pD, 8);
	std::vector<cv::Point2f> vPts;
	for (int i = 0; i < it.count; i++, ++it) {
		Point pt(it.pos());			//获取线段上的点
		if (abs(pointPolygonTest(contours[0], pt, true)) < 1) {
			vPts.push_back(pt);
			circle(src, pt, 5, Scalar(255, 255, 255), -1);
		}
	}

	//3：计算两个点的长度
	//对点进行聚类，只留下两个点
	int b = vPts.size();

	ret = clusterPts(vPts, vPtsLR);
	b = vPtsLR.size();
	if (vPtsLR.size() != 2) return EC_NOT2CROSS;
	length = std::pow(std::pow(vPtsLR[0].x - vPtsLR[1].x, 2) + std::pow(vPtsLR[0].y - vPtsLR[1].y, 2), 0.5);

	return 0;
}



/****************************************************
功能：计算眼眶四个分区的面积
输入：eyeMat  需要计算的纯眼眶图，单通道二值图
输出：partAreas     包含四个分区的面积

备注：
L1 上下对角线； L2 左右对角线
读取顺序从左到右从上到下
左眼眶顺序：S2,S1,S3,S4
右眼眶顺序：S1,S2,S4,S3

方法：
1、遍历像素，找到前景；
2、判断该像素坐标与L1和L2的位置关系
******************************************************/
int calcuPartArea(cv::Mat eyeMat, std::vector<float> &partAreas, sttLine2P L1, sttLine2P L2)
{
	////为了直观显示
	//cv::Mat showMat;
	//cvtColor(eyeMat, showMat, CV_GRAY2BGR);
	//////////////////////////////////////////////

	//遍历图像若像素值为255
	//若符合上下左右，各区的值++
	int areaLT = 0;			//左上区域
	int areaLB = 0;			//左下区域
	int areaRT = 0;			//右上区域
	int areaRB = 0;			//右下区域

	for (int i = 0; i < eyeMat.rows; i++)
	{
		uchar* data = eyeMat.ptr<uchar>(i);
		//uchar* show = showMat.ptr<uchar>(i);
		for (int j = 0; j < eyeMat.cols; j++) {
			//只统计前景像素
			if (data[j] == 255)
			{
				int retL1 = ptLtOrRt(Point2f(j, i), L1);		//返回值1-在L1左边，2-在L1右边
				int retL2 = ptUpOrDn(Point2f(j, i), L2);		//返回值1-在L2上面，2-在L2下面
				//判断像素点位于哪个分区
				if (retL1 == 1 && retL2 == 1) {
					//点位于L1左边且在L2上方
					areaLT++;
					////为了直观//////////////
					//show[3 * j] = 128;
					//show[3 * j + 1] = 0;
					//show[3 * j + 2] = 0;
				}
				else if (retL1 == 2 && retL2 == 1) {
					//点位于L1右边且在L2上方
					areaRT++;
					////为了直观//////////////
					//show[3 * j] = 0;
					//show[3 * j + 1] = 128;
					//show[3 * j + 2] = 0;
				}
				else if (retL1 == 1 && retL2 == 2) {
					//点位于L1左边且在L2下方
					areaLB++;
					////为了直观//////////////
					//show[3 * j] = 0;
					//show[3 * j + 1] = 0;
					//show[3 * j + 2] = 128;
				}
				else if (retL1 == 2 && retL2 == 2) {
					//点位于L1右边且在L2下方
					areaRB++;
					////为了直观//////////////
					//show[3 * j] = 128;
					//show[3 * j + 1] = 128;
					//show[3 * j + 2] = 0;
				}
			}
		}
	}

	//按从左到右从上到下存入向量
	partAreas.push_back(areaLT);
	partAreas.push_back(areaRT);
	partAreas.push_back(areaLB);
	partAreas.push_back(areaRB);

	return 0;
}



/****************************************************
功能：选择三通道中值最大的那个通道作为灰度值
输入：src  三通道彩色图
输出：dst  单通道灰度图

备注：取三通道中的最大值
******************************************************/
void grayMaxChnl(cv::Mat src, cv::Mat &dst)
{
	dst = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
	int rowNum = dst.rows;
	int colNum = dst.cols * dst.channels();

	for (int i = 0; i < rowNum; i++) {
		uchar* dataS = src.ptr<uchar>(i);
		uchar* dataD = dst.ptr<uchar>(i);
		for (int j = 0; j < colNum; j++) {
			dataD[j] = max({ dataS[3 * j], dataS[3 * j + 1],dataS[3 * j + 2] });
		}
	}

}



/****************************************************
功能：眼眶图像中的连通域分离，一个眼眶，两个特征
输入：src  三通道彩色图
输出：eyeMat-单通道 眼眶；point-另外两个特征的BBOX的中心点


备注：眼眶区域红或绿[0,128,0]，其他[128,128,0].[128.0.0]
******************************************************/
int sprtImage(cv::Mat src, cv::Mat &eyeMat, cv::Point2f &p1, cv::Point2f &p2)
{
	eyeMat = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
	cv::Mat smallMat = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);

	//现将两个特征点与整个眼眶分离开来
	int rowNum = smallMat.rows;
	int colNum = smallMat.cols * smallMat.channels();
	for (int i = 0; i < rowNum; i++) {
		uchar* dataS = src.ptr<uchar>(i);
		uchar* dataE = eyeMat.ptr<uchar>(i);
		uchar* dataD = smallMat.ptr<uchar>(i);
		for (int j = 0; j < colNum; j++) {
			if (dataS[3 * j] == 0 && dataS[3 * j + 1] == 0 && dataS[3 * j + 2] == 0) {
				dataD[j] = 0;
				dataE[j] = 0;
			}
			//若红或绿则赋值eyeMat，否则赋值smallMat
			else if ((dataS[3 * j] == 0 && dataS[3 * j + 1] > 0 && dataS[3 * j + 2] == 0) 
					|| (dataS[3 * j] == 0 && dataS[3 * j + 1] == 0 && dataS[3 * j + 2] > 0)) {
				dataE[j] = 255;
			}
			else if ((dataS[3 * j] > 0 && dataS[3 * j + 1] > 0 && dataS[3 * j + 2] == 0)
				|| (dataS[3 * j] > 0 && dataS[3 * j + 1] == 0 && dataS[3 * j + 2] > 0)) {
				//靠近鼻子的特征两者都赋值
				dataE[j] = 255;
				dataD[j] = 255;
			}
			else {
				dataD[j] = 255;
			}
		}
	}

	//在smallMat中寻找两个标注的特征点，输出其BBOX的中心
	cv::Mat lblMat;
	cv::Mat sttMat;
	cv::Mat cntMat;
	std::vector<cv::Point2f> vPts;

	//计算连通域
	int nComp = connectedComponentsWithStats(smallMat, lblMat, sttMat, cntMat);
	for (int i = 1; i < nComp; i++) {
		if (sttMat.at<int>(i, CC_STAT_AREA) < COMPTHRE)
		{
			//cv::Rect bbox;
			cv::Point2f p;
			////bounding box左上角坐标
			//bbox.x = sttMat.at<int>(i, 0);
			//bbox.y = sttMat.at<int>(i, 1);
			////bouding box的宽和长 
			//bbox.width = sttMat.at<int>(i, 2);
			//bbox.height = sttMat.at<int>(i, 3);

			p.x = sttMat.at<int>(i, 0) + sttMat.at<int>(i, 2) / 2;
			p.y = sttMat.at<int>(i, 1) + sttMat.at<int>(i, 3) / 2;

			vPts.push_back(p);
		}
	}

	if (vPts.size() != 2) return EC_FEATUREOVER;

	p1 = vPts[0];
	p2 = vPts[1];

	return 0;
}


/****************************************************
功能：眼眶图像中的连通域分离，一个眼眶，两个特征
输入：src  三通道彩色图
输出：eyeMat-单通道 眼眶；point-另外两个特征的BBOX的中心点


备注：输入图中的两点，输出该直线与图像的交点
******************************************************/
int calcuCross(cv::Mat src, cv::Point2f p1, cv::Point2f p2, cv::Point2f &pU, cv::Point2f &pD)
{
	std::vector<cv::Point2f> vPts;
	std::vector<cv::Point2f> outPts;		//只包含两个元素
	if (p1.x != p2.x) {
		float k, b;
		k = (p2.y - p1.y) / (p2.x - p1.x);
		b = p1.y - p1.x * k;
		//遍历图像四条边，寻找与直线的交点
		for (int y = 1; y < src.rows; y++) {
			if (abs(y - b) < 1) {
				vPts.push_back(Point2f(0, y));
			}
			if (abs(y - k * src.cols - b) < 1) {
				vPts.push_back(Point2f(src.cols, y));
			}
		}
		for (int x = 1; x < src.cols; x++) {
			if (abs(x + b / k) < 1) {
				vPts.push_back(Point2f(x, 0));
			}
			if (abs(x - (src.rows - b) / k) < 1) {
				vPts.push_back(Point2f(x, src.rows));
			}
		}
		//四个顶点另外考虑
		if (abs(b) < 1) vPts.push_back(Point2f(0, 0));
		if (abs(src.cols*k + b) < 1) vPts.push_back(Point2f(src.cols, 0));
		if (abs(b - src.rows) < 1) vPts.push_back(Point2f(0, src.rows));
		if (abs(src.cols*k + b - src.rows) < 1) vPts.push_back(Point2f(src.cols, src.rows));

		//点聚类
		int ret = clusterPts(vPts, outPts);
		if (ret != 0) return ret;

		int a = outPts.size();
		if (outPts.size() != 2) return EC_NOCROSSIMAGE;

		pU = outPts[0];
		pD = outPts[1];
	}
	else {
		pU.x = p1.x;
		pU.y = 0;
		pD.x = p1.x;
		pD.y = src.rows;
	}

	return 0;
}



/****************************************************
功能：点聚类，一堆点聚类为两个点
输入：vector<cv::Point2f> vPts    输入的一堆点
输出：vector<cv::Point2f> outPts  输出的两个点

备注：结果输出的两个点的距离超过10
******************************************************/
int clusterPts(std::vector<cv::Point2f> vPts, std::vector<cv::Point2f> &outPts)
{
	if (vPts.size() < 2)
		return EC_NOT2POINT;


	std::vector<cv::Point2f>::iterator iterl1;
	std::vector<cv::Point2f>::iterator iterl2;

	for (iterl1 = vPts.begin(); iterl1 != vPts.end(); iterl1++) {
		for (iterl2 = vPts.begin(); iterl2 != vPts.end(); iterl2++) {
			if (iterl1 != iterl2) {
				int xx = pow(iterl1->y - iterl2->y, 2) + pow(iterl1->x - iterl2->x, 2);
				if (xx < 100) {
					//距离过小就删除
					iterl2 = vPts.erase(iterl2) - 1;

				}
			}
		}
	}

	//留下来的点赋值给输出
	for (int i = 0; i < vPts.size(); i++) {
		outPts.push_back(vPts[i]);
	}


	return 0;
}




/********************************************************
功能：根据两点坐标计算直线的一般形式

输入：两点坐标[x1,y1],[x2,y2]
输出：
A，B，C
Ax+By+C=0；

-返回值:
计算成功返回0
两点相同返回1
********************************************************/
int calcGenLine2P(cv::Point2f p1, cv::Point2f p2, sttLine2P &para)
{
	if ((p1.x == p2.x) && (p1.y == p2.y)) {
		return 1;
	}

	if (p1.x == p2.x) {
		//垂直直线
		//Ax + By + C = 0
		para.A = -1;
		para.B = 0;
		para.C = p1.x;
	}
	else {
		double slop = (p2.y - p1.y) / (p2.x - p1.x);
		para.A = slop;
		para.B = -1;
		para.C = -slop*p1.x + p1.y;
	}
	return 0;
}


/********************************************************
功能：判断点在直线上方还是下方

输入：点的坐标及直线的两点式参数
输出：

-返回值:
点在直线上方返回1
点在直线下方返回2
其他返回0
********************************************************/
int ptUpOrDn(cv::Point2f pt, sttLine2P &para)
{
	int lineY = -(para.A*pt.x + para.C) / para.B;
	if (lineY > pt.y) {
		return 1;
	}
	else if (lineY < pt.y) {
		return 2;
	}
	else
		return 0;
}


/********************************************************
功能：判断点在直线左边还是右边

输入：点的坐标及直线的两点式参数
输出：

-返回值:
点在直线左边返回1
点在直线右边返回2
其他返回0
********************************************************/
int ptLtOrRt(cv::Point2f pt, sttLine2P &para)
{
	int lineX = -(para.B*pt.y + para.C) / para.A;
	if (lineX > pt.x) {
		return 1;
	}
	else if (lineX < pt.x) {
		return 2;
	}
	else {
		return 0;
	}
}



/********************************************************
功能：整理左右对角线点，颧额缝点放在最前面

输入：左右对角线点的向量，左眼眶或右眼眶的flag
输出：整理过后的向量

备注：
flag=1，左眼眶，颧额缝点放在vPts[0];
flag=2，右眼眶，颧额缝点放在vPts[0];
方法：
左眼眶点x值较小，右眼眶点x值较大
********************************************************/
int findSeamPt(std::vector<cv::Point2f> &vPts, int flag)
{
	cv::Point2f p1, p2;

	if (flag == 1) {
		//若是左眼眶
		if (vPts[0].x < vPts[1].x) {
			p1 = vPts[0];
			p2 = vPts[1];
		}
		else {
			p1 = vPts[1];
			p2 = vPts[0];
		}
	}
	else if (flag == 2) {
		//若是右眼眶
		if (vPts[0].x < vPts[1].x) {
			p1 = vPts[1];
			p2 = vPts[0];
		}
		else {
			p1 = vPts[0];
			p2 = vPts[1];
		}
	}

	vPts.clear();
	vPts.push_back(p1);
	vPts.push_back(p2);


	return 0;
}




/********************************************************
功能：已知颧额缝点，计算眶口宽度

输入：只有眼眶的图（单通道）， 颧额缝点
输出：与颧额缝点水平的轮廓上的点， 眶口宽度， 眼眶的轮廓

备注：
方法：
两点y值相同
********************************************************/
int calcuAptWidth(cv::Mat src, cv::Point2f inPt, cv::Point2f &outPt, 
				float &width, vector<vector<cv::Point>> &contours)
{
	if (src.channels() == 3) {
		cvtColor(src, src, CV_BGR2GRAY);
	}

	cv::Mat image(m_crtMat.rows, m_crtMat.cols, CV_8UC3);
	image = 0;
	vector<Vec4i> hierarchy;

	findContours(src, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	drawContours(image, contours, 0, Scalar(255, 255, 255), 1, 8);
	cvtColor(image, image, CV_BGR2GRAY);

	//在轮廓中寻找对应于颧额缝点的水平点
	for (int i = 0; i < image.rows; i++) {
		uchar* data = image.ptr<uchar>(i);
		for (int j = 0; j < image.cols; j++) {
			if (data[j] == 255) {
				if (i == inPt.y && abs(j - inPt.x) > 10) {
					outPt.x = j;
					outPt.y = i;
				}
			}
		}
	}

	width = abs(outPt.x - inPt.x);

	return 0;
}



/********************************************************
功能：已知两水平点（y值相同），对其十等分得到九个点

输入：y值相同的两点，左眼眶还是右眼眶的flag
输出：按顺序输出的九个点

备注：
flag=1 左眼眶x值从大到小排列
flag=2 右眼眶x值从小到大排列
方法：
********************************************************/
int tenEqual(cv::Point p1, cv::Point p2, vector<cv::Point> &vPts, int flag)
{
	float diffX = 0;
	cv::Point pt;
	pt.y = p1.y;

	if (flag == 1) {
		//若为左眼眶，pt1在左边，x值较小
		diffX = (p2.x - p1.x) / 10;
		for (int i = 1; i < 10; i++) {
			pt.x = p1.x + i * diffX;
			vPts.push_back(pt);
		}
	}
	else if (flag == 2) {
		//若为右眼眶，pt2在左边，x值较小
		diffX = (p1.x - p2.x) / 10;
		for (int i = 1; i < 10; i++) {
			pt.x = p2.x + i * diffX;
			vPts.push_back(pt);
		}
	}


	return 0;
}



/********************************************************
功能：计算横轴上等分点至眶上缘和眶下缘的垂直距离

输入：眼眶轮廓， 9个点， 比例尺
输出：9个点对应的18个距离（真实距离）

备注：
像素个数乘以coff得到输出的距离
方法：
********************************************************/
int calcuDvsHt(vector<vector<cv::Point>> contours, vector<cv::Point> equalPts,
			    vector<float> &vHts, float coff)
{

	cv::Mat image(m_crtMat.rows, m_crtMat.cols, CV_8UC3);
	image = 0;
	
	drawContours(image, contours, 0, Scalar(255, 255, 255), 1, 8);
	cvtColor(image, image, CV_BGR2GRAY);

	for (int m = 0; m < equalPts.size(); m++) {
		for (int i = 0; i < image.rows; i++) {
			if (image.at<uchar>(i, equalPts[m].x) == 255) {
				float temp = (equalPts[m].y - i) * coff;
				vHts.push_back(temp);
			}
		}
	}

	if (vHts.size() != 18) {
		return 1;
	}

	return 0;
}


//根据BBOX中心点找到眼眶外侧的的边缘点
//flag=1，左眼眶外侧为x比较小的
//flag=2，右眼眶外侧为x比较大的
int findEdgePts(cv::Mat src, cv::Point2f center, int flag, cv::Point2f &outPt)
{
	if (src.channels() == 3) {
		cvtColor(src, src, CV_BGR2GRAY);
	}

	cv::Mat image(m_crtMat.rows, m_crtMat.cols, CV_8UC3);
	image = 0;
	vector<Vec4i> hierarchy;
	vector<vector<cv::Point>> contours;

	findContours(src, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	drawContours(image, contours, 0, Scalar(255, 255, 255), 1, 8);
	cvtColor(image, image, CV_BGR2GRAY);

	//在轮廓中寻找水平点
	if (flag == 1)
	{
		//若是左眼眶，找x值小的
		uchar* data = image.ptr<uchar>(center.y);
		for (int j = 0; j < center.x; j++) {
			if (data[j] == 255) {
				outPt.x = j;
				outPt.y = center.y;
				break;
			}
		}
	}
	else {
		//若是右眼眶，找x值大的
		uchar* data = image.ptr<uchar>(center.y);
		for (int j = center.x; j < image.cols; j++) {
			if (data[j] == 255) {
				outPt.x = j;
				outPt.y = center.y;
				break;
			}
		}
	}

	return 0;
}








/********************************************************
功能：计算单眼眶的距离一维向量

输入：单眼眶
输出：

备注：

方法：
********************************************************/
int oneVec(cv::Mat src, float *ary)
{
	//定义图像容器
	cv::Mat gryMat;
	cv::Mat bnyMat;
	cv::Mat disMat;
	cv::Mat sttMat;
	cv::Mat cntMat;
	cv::Mat lblMat;
	cv::Mat srcCopy = src.clone();
	//灰度化
	//grayMaxChnl(src, gryMat);
	cvtColor(src, gryMat, CV_BGR2GRAY);
	//二值化
	cv::threshold(gryMat, bnyMat, 23, 255, THRESH_OTSU);
	//连通域
	int nComp = connectedComponentsWithStats(bnyMat, lblMat, sttMat, cntMat);

	//0号为背景，跳过，i=1开始循环
	cv::Rect BBOX;
	std::vector<cv::Rect> vBbox;
	std::vector<cv::Mat> vMat;
	std::vector<int> vArea;
	for (int i = 1; i < nComp; i++) {
		if (sttMat.at<int>(i, CC_STAT_AREA) > 500)
		{
			//bounding box左上角坐标
			BBOX.x = sttMat.at<int>(i, 0);
			BBOX.y = sttMat.at<int>(i, 1);
			//bouding box的宽和长 
			BBOX.width = sttMat.at<int>(i, 2);
			BBOX.height = sttMat.at<int>(i, 3);

			vBbox.push_back(BBOX);
			vArea.push_back(sttMat.at<int>(i, CC_STAT_AREA));
		}
	}


	cv::Point2f center;
	center.x = BBOX.x + BBOX.width / 2;
	center.y = BBOX.y + BBOX.height / 2;

	cv::Mat ctImage(src.rows, src.cols, CV_8UC3);
	ctImage = 0;
	vector<Vec4i> hierarchy;
	vector<vector<cv::Point>> contours;

	findContours(gryMat, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	drawContours(ctImage, contours, 0, Scalar(255, 255, 255), 2, 8);	//找点时越界的话就增大倒数第二个参数

	cv::Mat showImage = ctImage.clone();
	cv::circle(showImage, center, 1, Scalar(255, 255, 255), 2, 8, 0);
	cvtColor(ctImage, ctImage, CV_BGR2GRAY);

	//先找除了90度的其他角度的距离值
	for (int i = 0; i < 180; i++)
	{
		if (i == 90) continue;
		double k = tan(CV_PI * i / 180);
		double b = center.y - k * center.x;

		int y2 = b;
		int y3 = k * src.cols + b;
		cv::Mat lineMat = showImage.clone();
		lineMat = 0;

		cv::line(lineMat, Point2f(src.cols, y3), Point2f(0, y2), Scalar(255, 255, 255), 1, 8, 0);
		cvtColor(lineMat, lineMat, CV_BGR2GRAY);
		//cv::Mat tempfile = lineMat / 2 + ctImage / 2;

		//遍历lineMat中的直线，若线上的点对应shwoImage中的位置是轮廓就输出
		bool flag = false;
		for (int x = center.x; x < ctImage.cols && flag == false; x++) {
			for (int y = 0; y < ctImage.rows && flag == false; y++) {
				if (lineMat.at<uchar>(y, x) == 255 && ctImage.at<uchar>(y, x) == 255) {
					cv::circle(showImage, Point2f(x, y), 1, Scalar(0, 0, 255), 1, 8, 0);
					if (i < 90) {
						//找0-90度范围内的下交点
						ary[i] = sqrt(pow(center.x - x, 2) + pow(center.y - y, 2));
					}
					else {
						//找270-360度范围内的上交点
						ary[i + 180] = sqrt(pow(center.x - x, 2) + pow(center.y - y, 2));
					}
					flag = true;
				}
			}
		}


		flag = false;
		for (int x = center.x; x > 0 && flag == false; x--) {
			for (int y = 0; y < ctImage.rows && flag == false; y++) {
				if (lineMat.at<uchar>(y, x) == 255 && ctImage.at<uchar>(y, x) == 255) {
					cv::circle(showImage, Point2f(x, y), 1, Scalar(0, 0, 255), 1, 8, 0);
					if (i < 90) {
						//找180-270度范围内的下交点
						ary[i + 180] = sqrt(pow(center.x - x, 2) + pow(center.y - y, 2));
					}
					else {
						//找90-180度范围内的上交点
						ary[i] = sqrt(pow(center.x - x, 2) + pow(center.y - y, 2));
					}
					flag = true;
				}
			}
		}
	}

	//找90度的
	for (int y = center.y + 10; y < ctImage.rows; y++) {
		if (ctImage.at<uchar>(y, center.x) == 255) {
			ary[90] = y - center.y;
			break;
		}
	}
	//找270度的
	for (int y = 0; y < center.y; y++) {
		if (ctImage.at<uchar>(y, center.x) == 255) {
			ary[270] = center.y - y;
			break;
		}
	}

	for (int i = 0; i < 360; i++) {
		if (ary[i] == 0)
			return 1;
	}




	return 0;
}
















//后处理函数，释放全局vector变量
void _stdcall postProcess()
{
	//释放内存
	m_vAreas.clear();
	m_vCtrPts.clear();
	m_vBbox.clear();
	m_vPtsUDL.clear();
	m_vPtsLRL.clear();
	m_vPtsUDR.clear();
	m_vPtsLRR.clear();

	std::vector<float>().swap(m_vAreas);
	std::vector<cv::Point2f>().swap(m_vCtrPts);
	std::vector<cv::Rect>().swap(m_vBbox);
	std::vector<Point2f>().swap(m_vPtsUDL);
	std::vector<Point2f>().swap(m_vPtsLRL);
	std::vector<Point2f>().swap(m_vPtsUDR);
	std::vector<Point2f>().swap(m_vPtsLRR);

}









