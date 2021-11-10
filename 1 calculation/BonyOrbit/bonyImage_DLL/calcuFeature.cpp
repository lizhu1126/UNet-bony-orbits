#include "calcuFeature.h"
#include <opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"


using namespace std;
using namespace cv;


/****************************************************
���ܣ�׼��ͼ��
���룺ͼ��·��
�������

ȫ�ֱ�����ԭʼͼ��  m_crtMat
���ŵ����ۿ���ͼ��  m_rtMat  m_ltMat

��ע���ڽ�����������֮ǰ�����뾭���˺���
******************************************************/
int _stdcall imageReady(char * imagePath)
{
	m_crtMat = cv::imread(imagePath);
	string framePath = imagePath;

	//string 
	size_t Tpos = framePath.find_last_of("\\");			//������\\���һ�γ��ֵ�λ�ã������أ�����0��
	m_sCrtFrameName = framePath.substr(Tpos + 1);		//��ָ��λ�ø������ַ��������ļ������ƣ���ȥ��ǰ���

	if (m_crtMat.empty()) {
		return EC_NOIMAGE;
	}

	//����ͼ������
	cv::Mat gryMat;
	cv::Mat bnyMat;
	cv::Mat disMat;
	cv::Mat sttMat;
	cv::Mat cntMat;
	cv::Mat lblMat;
	m_rtMat = cv::Mat::zeros(m_crtMat.rows, m_crtMat.cols, CV_8UC3);		//֮ǰΪ��ͨ�� 20201230�޸�
	m_ltMat = cv::Mat::zeros(m_crtMat.rows, m_crtMat.cols, CV_8UC3);

	//�ҶȻ�
	grayMaxChnl(m_crtMat, gryMat);

	//��ֵ��
	cv::threshold(gryMat, bnyMat, 100, 255, THRESH_OTSU);

	////��ɫ������ɫΪ������Ҫ��ɫ
	//bnyMat = 255 - bnyMat;

	//��ͨ��
	int nComp = connectedComponentsWithStats(bnyMat, lblMat, sttMat, cntMat);

	//0��Ϊ������������i=1��ʼѭ��
	std::vector<cv::Rect> vBbox;
	std::vector<cv::Mat> vMat;
	std::vector<int> vArea;
	std::vector<cv::Point2f> vPts;
	for (int i = 1; i < nComp; i++) {
		if (sttMat.at<int>(i, CC_STAT_AREA) > COMPTHRE)
		{
			//����bounding box
			Rect bbox;
			cv::Point2f center;
			cv::Mat roi;
			//bounding box���Ͻ�����
			//bbox.x = sttMat.at<int>(i, 0) - 20;		//��������20�����أ���������־Ȧ��ȥ
			bbox.x = sttMat.at<int>(i, 0);
			bbox.y = sttMat.at<int>(i, 1);
			//bouding box�Ŀ�ͳ� 
			//bbox.width = sttMat.at<int>(i, 2) + 40;
			bbox.width = sttMat.at<int>(i, 2);
			bbox.height = sttMat.at<int>(i, 3);
			//���ĵ�
			center.x = bbox.x + bbox.width / 2;
			center.y = bbox.y + bbox.height / 2;
			
			//roi = bnyMat(bbox); 
			roi = m_crtMat(bbox);		//20201230�޸�
			vMat.push_back(roi); 
			vBbox.push_back(bbox);
			vPts.push_back(center);
			vArea.push_back(sttMat.at<int>(i, CC_STAT_AREA));

			////����
			//rectangle(m_crtMat, bbox, CV_RGB(255, 255, 0), 2, 8, 0);
		}
	}

	//����ȡ�����ۿ������ۿ�
	if (vBbox.size() != 2) return EC_RIGHTANDLEFT;

	if (vBbox[0].x < vBbox[1].x) {
		//0λ��Ϊ���ۿ�
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
		//0λ��Ϊ���ۿ�
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
���ܣ����������ۿ���ͨ������
���룺txt��������֣����硰����_�С�������_Ů���ȣ�
������޲������ݵ�ǰ�ˣ����ǻ��ڱ��ر���txt�ļ�

ȫ�ֱ�����ÿ��ͼƬ������   m_sCrtFrameName
		  ��������ۿ����������   m_vAreas

��ע��������������浽txt
      ������㹫ʽ��S=��ͨ�����*49/40000
******************************************************/
int _stdcall calcuArea(char *saveName)
{
	//��ͼ���е�������ͨ����Ϣ���浽����txt��
	string name = saveName;
	string nameString = name.append("_leftArea.txt");
	m_file.open(nameString, ios::app);

	float realAeraL = m_vAreas[0] * 49 / 40000;
	m_file << m_sCrtFrameName << "��" << realAeraL << endl;
	m_file.close();
	

	name = saveName;
	nameString = name.append("_rightArea.txt");
	m_file.open(nameString, ios::app);
	 
	float realAeraR = m_vAreas[1] * 49 / 40000;
	m_file << m_sCrtFrameName << "��" << realAeraR << endl;
	m_file.close();

	return 0;
}


/****************************************************
���ܣ������ۿ��Խ��ߵĳ��ȼ���ֵ
���룺txt��������֣����硰����_�С�������_Ů���ȣ�
������޲������ݵ�ǰ�ˣ����ǻ��ڱ��ر���txt�ļ�

ȫ�ֱ�����ÿ��ͼƬ������   m_sCrtFrameName
		  ���ŵ����ۿ���ͼ��  m_rtMat  m_ltMat

��ע������Խ��߳��ȼ���ֵ�����浽txt
******************************************************/
int _stdcall diagonalRatio(char *saveName)
{
	float lengthUDL = 0;		//���ۿ����¶Խ��߳���
	float lengthLRL = 0;		//���ۿ����ҶԽ��߳���
	float ratioL = 0;			//���ۿ����������ҵı�ֵ
	float lengthUDR = 0;		//���ۿ�
	float lengthLRR = 0;
	float ratioR = 0;
	int ret = 0;
	float coff = 7.0 / 20.0;

	//------------�������ۿ����¼����ҶԽ��߳��ȼ���ֵ------------------------------
	ret = calcuLengthUD(m_ltMat, lengthUDL, m_vPtsUDL);
	ret = calcuLengthLR(m_ltMat, m_ltEyeMat, lengthLRL, m_vPtsLRL);
	if (ret != 0) {
		return ret;
	}
	ratioL = lengthUDL / lengthLRL;			//�������ۿ��Խ��߱�ֵ

	string name = saveName;
	string nameString = name.append("_leftDiagonal.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "�����£�" << lengthUDL * coff << "�����ң�" << lengthLRL * coff << "�����������ұ�ֵ��" << ratioL << endl;
	m_file.close();


	//------------�������ۿ����¼����ҶԽ��߳��ȼ���ֵ-------------------------------
	ret = calcuLengthUD(m_rtMat, lengthUDR, m_vPtsUDR);
	ret = calcuLengthLR(m_rtMat, m_rtEyeMat, lengthLRR, m_vPtsLRR);
	if (ret != 0) {
		return ret;
	}
	ratioR = lengthUDR / lengthLRR;			//�������ۿ��Խ��߱�ֵ

	name = saveName;
	nameString = name.append("_rightDiagonal.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "�����£�" << lengthUDR * coff << "�����ң�" << lengthLRR * coff << "�����������ұ�ֵ��" << ratioR << endl;
	m_file.close();


	return 0;
}


//int _stdcall diagonalRatio(char *saveName)
//{
//	float lengthUDL = 0;		//���ۿ����¶Խ��߳���
//	float lengthLRL = 0;		//���ۿ����ҶԽ��߳���
//	float ratioL = 0;			//���ۿ����������ҵı�ֵ
//	float lengthUDR = 0;		//���ۿ�
//	float lengthLRR = 0;
//	float ratioR = 0;
//	int ret = 0;
//	float coff = 7.0 / 20.0;
//
//	//------------�������ۿ����¼����ҶԽ��߳��ȼ���ֵ------------------------------
//	ret = calcuLengthUD(m_ltMat, lengthUDL, m_vPtsUDL);
//	if (ret != 0) {
//		return ret;
//	}
//
//	string name = saveName;
//	string nameString = name.append("_leftDiagonal.txt");
//	m_file.open(nameString, ios::app);
//	m_file << m_sCrtFrameName << "�����£�" << lengthUDL * coff << endl;
//	m_file.close();
//
//
//	//------------�������ۿ����¼����ҶԽ��߳��ȼ���ֵ-------------------------------
//	ret = calcuLengthUD(m_rtMat, lengthUDR, m_vPtsUDR);
//	if (ret != 0) {
//		return ret;
//	}
//
//	name = saveName;
//	nameString = name.append("_rightDiagonal.txt");
//	m_file.open(nameString, ios::app);
//	m_file << m_sCrtFrameName << "�����£�" << lengthUDR * coff << endl;
//	m_file.close();
//
//
//	return 0;
//}


/****************************************************
���ܣ����������ۿ������������
���룺txt��������֣����硰����_�С�������_Ů���ȣ�
������޲������ݵ�ǰ�ˣ����ǻ��ڱ��ر���txt�ļ�

��ע�����������������浽txt
��ȡ˳������Ҵ��ϵ���
���ۿ�˳��S2,S1,S3,S4
���ۿ�˳��S1,S2,S4,S3
������
1������õ������Խ��ߣ�
2�����㱻�����Խ��߷ָ�Ϊ�ĸ�������ۿ������
******************************************************/
int _stdcall partAreas(char *saveName)
{
	int ret = 0;
	sttLine2P L1, L2;				//�����Խ���
	std::vector<float> partAreas;	//��ŷ����������
	float coff = 49.0 / 40000.0;	//��������ı�����

	//------------�������ۿ����������-------------------------------
	//���������Խ���
	calcGenLine2P(m_vPtsUDL[0], m_vPtsUDL[1], L1);		//���¶Խ���
	calcGenLine2P(m_vPtsLRL[0], m_vPtsLRL[1], L2);		//���ҶԽ���
	////////////////////////////////////////////////////////////////
	cv::Mat showMat;
	cvtColor(m_ltEyeMat, showMat, CV_GRAY2BGR);
	cv::circle(showMat, m_vPtsUDL[0], 3, Scalar(255, 255, 255), 3, 8, 0);
	cv::circle(showMat, m_vPtsUDL[1], 3, Scalar(255, 255, 255), 3, 8, 0);
	line(showMat, m_vPtsUDL[0], m_vPtsUDL[1], Scalar(0, 0, 255), 1, 8, 0);
	line(showMat, m_vPtsLRL[0], m_vPtsLRL[1], Scalar(0, 255, 0), 1, 8, 0);
	///////////////////////////////////////////////////////////////
	//����������
	calcuPartArea(m_ltEyeMat, partAreas, L1, L2);

	string name = saveName;
	string nameString = name.append("_leftPartAreas.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "��S1��" << partAreas[1] * coff << "��S2��" << partAreas[0] * coff << "��S3��" << partAreas[2] * coff << "��S4��" << partAreas[3] * coff << endl;
	m_file.close();


	//------------�������ۿ����������-------------------------------
	//���������Խ���
	calcGenLine2P(m_vPtsUDR[0], m_vPtsUDR[1], L1);		//���¶Խ���
	calcGenLine2P(m_vPtsLRR[0], m_vPtsLRR[1], L2);		//���ҶԽ���
	//����������
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
	m_file << m_sCrtFrameName << "��S1��" << partAreas[4] * coff << "��S2��" << partAreas[5] * coff << "��S3��" << partAreas[7] * coff << "��S4��" << partAreas[6] * coff << endl;
	m_file.close();

	return 0;
}




/****************************************************
���ܣ����������ۿ����ڿ�ȼ������ϵȷֵ�������Ե�Ϳ���Ե�Ĵ�ֱ����
���룺txt��������֣����硰����_�С�������_Ů���ȣ�
������޲������ݵ�ǰ�ˣ����ǻ��ڱ��ر���txt�ļ�

��ע��
����㡪�����ڿ�ȡ��ȷֵ�����Ե����Ե�ľ���
������
1���ҵ�ȧ��������㣨�����۲�ͬ����
2���ҵ�yֵ��ͬ�������ϵĵ㣬�����������
3��������ˮƽ����ʮ�ȷֵ㣨9���ߣ�
4�����ȷֵ��x��һ�����ҵ������϶�Ӧ������yֵ��������롣
******************************************************/
int _stdcall apertureWidth(char *saveName)
{
	int ret = 0;
	float aptWidthL = 0;					//���ۿ��Ŀ��ڿ��
	float aptWidthR = 0;					//���ۿ��Ŀ��ڿ��
	cv::Point2f horiLtPt;					//���ۿ���ȧ����ˮƽ��������
	cv::Point2f horiRtPt;					//���ۿ���ȧ����ˮƽ��������
	vector<cv::Point> equalPtsL;			//������ۿ���ʮ�ȷֵ�
	vector<cv::Point> equalPtsR;			//������ۿ���ʮ�ȷֵ�
	vector<float> vHtsL;					//������ۿ��ȷֵ���������Ե�Ĵ�ֱ����
	vector<float> vHtsR;					//������ۿ��ȷֵ���������Ե�Ĵ�ֱ����
	vector<vector<cv::Point>> contoursL;	//���ۿ�������
	vector<vector<cv::Point>> contoursR;	//���ۿ�������
	float coff = 7.0 / 20.0;				//���ȵı�����


	//------------�������ۿ����ڿ�ȵ�-------------------------------
	//�����ҶԽǵ�������ɸѡ��ȧ����
	findSeamPt(m_vPtsLRL, 1);
	//�ҵ�ͬˮƽ�ߵ��������㲢���������
	calcuAptWidth(m_ltEyeMat, m_vPtsLRL[0], horiLtPt, aptWidthL, contoursL);
	//����֮����ʮ�ȷֵ�
	tenEqual(m_vPtsLRL[0], horiLtPt, equalPtsL, 1);
	//������ȷֵ㵽���������ľ���
	calcuDvsHt(contoursL, equalPtsL, vHtsL, coff);
	///////�����Ƿ���ȷ��///////////////////////////////////////////////////////////
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
	
	//������д���ļ�
	string name = saveName;
	string nameString = name.append("_leftApeWidth.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "��width��" << aptWidthL * coff << endl;
	m_file.close();   
	//���ȷֵ㵽����Ե����д���ļ�
	name = saveName;
	nameString = name.append("_leftHeightS.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName<<"��short��";
	for (int i = 0; i < 9; i++) {
		m_file << abs(vHtsL[2 * i]) << "��";
	}
	m_file << endl;
	m_file.close();
	//���ȷֵ㵽����Ե����д���ļ�
	name = saveName;
	nameString = name.append("_leftHeightH.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "��high��";
	for (int i = 0; i < 9; i++) {
		m_file << abs(vHtsL[2 * i + 1]) << "��";
	}
	m_file << endl;
	m_file.close();

	//------------�������ۿ����ڿ�ȵ�-------------------------------
	//�����ҶԽǵ�������ɸѡ��ȧ����
	findSeamPt(m_vPtsLRR, 2);
	//�ҵ�ͬˮƽ�ߵ��������㲢���������
	calcuAptWidth(m_rtEyeMat, m_vPtsLRR[0], horiRtPt, aptWidthR, contoursR);
	//����֮����ʮ�ȷֵ�
	tenEqual(m_vPtsLRR[0], horiRtPt, equalPtsR, 2);
	//������ȷֵ㵽���������ľ���
	calcuDvsHt(contoursR, equalPtsR, vHtsR, coff);

	//������д���ļ�
	name = saveName;
	nameString = name.append("_rightApeWidth.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "��width��" << aptWidthR * coff << endl;
	m_file.close();
	//���ȷֵ㵽����Ե����д���ļ�
	name = saveName;
	nameString = name.append("_rightHeightS.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "��short��";
	for (int i = 0; i < 9; i++) {
		m_file << abs(vHtsR[2 * i]) << "��";
	}
	m_file << endl;
	m_file.close();
	//���ȷֵ㵽����Ե����д���ļ�
	name = saveName;
	nameString = name.append("_rightHeightH.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "��high��";
	for (int i = 0; i < 9; i++) {
		m_file << abs(vHtsR[2 * i + 1]) << "��";
	}
	m_file << endl;
	m_file.close();

	///////�����Ƿ���ȷ��///////////////////////////////////////////////////////////
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
���ܣ��������ۿ���BBOX�Ŀ�ȸ߶ȴ���ȥ
���룺txt��������֣����硰����_�С�������_Ů���ȣ�
������޲������ݵ�ǰ�ˣ����ǻ��ڱ��ر���txt�ļ�

��ע���Ѿ��Ǽ���õ�ȫ�ֱ�����ֻ��Ҫ����ȥ����
******************************************************/
int _stdcall bboxInfo(char *saveName)
{
	//��ͼ���е�����BBOX��Ϣ���浽����txt��
	string name = saveName;
	string nameString = name.append("_leftHW.txt");
	m_file.open(nameString, ios::app);

	m_file << m_sCrtFrameName << "��" << "height��" << m_vBbox[0].height * 7.0 / 20.0 << "��width��" << m_vBbox[0].width * 7.0 / 20.0 << endl;
	m_file.close();


	name = saveName;
	nameString = name.append("_rightHW.txt");
	m_file.open(nameString, ios::app);

	m_file << m_sCrtFrameName << "��" << "height��" << m_vBbox[1].height * 7.0 / 20.0 << "��width��" << m_vBbox[1].width * 7.0 / 20.0 << endl;
	m_file.close();



	return 0;
}




/****************************************************
���ܣ����������ۿ�BBOX���ĵ��Ӧˮƽ���ϵľŵȷ�
���룺txt��������֣����硰����_�С�������_Ů���ȣ�
������޲������ݵ�ǰ�ˣ����ǻ��ڱ��ر���txt�ļ�

��ע��
����㡪���ȷֵ�����Ե����Ե�ľ���
������
1���ҵ����ĵ��Ӧ������������Ե�㣻
2���ҵ�yֵ��ͬ�������ϵĵ㣬�����������
3��������ˮƽ����ʮ�ȷֵ㣨9���ߣ�
4�����ȷֵ��x��һ�����ҵ������϶�Ӧ������yֵ��������롣
******************************************************/
int _stdcall bboxWidth(char *saveName)
{
	int ret = 0;
	float aptWidthL = 0;					//���ۿ��Ŀ��ڿ��
	float aptWidthR = 0;					//���ۿ��Ŀ��ڿ��
	cv::Point2f horiLtPts;					//���ۿ�ˮƽ������Ե��
	cv::Point2f horiLtPtm;					//���ۿ�ˮƽ���ڲ��Ե��
	cv::Point2f horiRtPts;					//���ۿ�ˮƽ���ڲ��Ե��
	cv::Point2f horiRtPtm;					//���ۿ�ˮƽ������Ե��
	vector<cv::Point> equalPtsL;			//������ۿ���ʮ�ȷֵ�
	vector<cv::Point> equalPtsR;			//������ۿ���ʮ�ȷֵ�
	vector<float> vHtsL;					//������ۿ��ȷֵ���������Ե�Ĵ�ֱ����
	vector<float> vHtsR;					//������ۿ��ȷֵ���������Ե�Ĵ�ֱ����
	vector<vector<cv::Point>> contoursL;	//���ۿ�������
	vector<vector<cv::Point>> contoursR;	//���ۿ�������
	float coff = 7.0 / 20.0;				//���ȵı�����

	////BBOX����������
	//cv::Mat src = m_crtMat;
	//rectangle(src, m_vBbox[0], CV_RGB(255, 255, 0), 1, 8, 0);
	//circle(src, m_vCtrPts[0], 1, Scalar(0, 0, 255), 1, 8, 0);
	//rectangle(src, m_vBbox[1], CV_RGB(255, 255, 0), 1, 8, 0);
	//circle(src, m_vCtrPts[1], 1, Scalar(0, 0, 255), 1, 8, 0);

	//------------�������ۿ��ŵȷ�-------------------------------
	//�ȸ���BBOX���ĵ���ˮƽ���ҵ��ۿ�������Ե��
	findEdgePts(m_ltMat, m_vCtrPts[0], 1, horiLtPts);
	//�ҵ�ͬˮƽ�ߵ��������㲢���������
	calcuAptWidth(m_ltMat, horiLtPts, horiLtPtm, aptWidthL, contoursL);
	//����֮����ʮ�ȷֵ�
	tenEqual(horiLtPts, horiLtPtm, equalPtsL, 1);
	//������ȷֵ㵽���������ľ���
	calcuDvsHt(contoursL, equalPtsL, vHtsL, coff);

	//���ȷֵ㵽����Ե����д���ļ�
	string name = saveName;
	string nameString = name.append("_leftHeightS.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "��short��";
	for (int i = 0; i < 9; i++) {
		m_file << abs(vHtsL[2 * i]) << "��";
	}
	m_file << endl;
	m_file.close();
	//���ȷֵ㵽����Ե����д���ļ�
	name = saveName;
	nameString = name.append("_leftHeightH.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "��high��";
	for (int i = 0; i < 9; i++) {
		m_file << abs(vHtsL[2 * i + 1]) << "��";
	}
	m_file << endl;
	m_file.close();



	//------------�������ۿ��ŵȷ�-------------------------------
	//�ȸ���BBOX���ĵ���ˮƽ���ҵ��ۿ�������Ե��
	findEdgePts(m_rtMat, m_vCtrPts[1], 2, horiRtPts);
	//�ҵ�ͬˮƽ�ߵ��������㲢���������
	calcuAptWidth(m_rtMat, horiRtPts, horiRtPtm, aptWidthR, contoursR);
	//����֮����ʮ�ȷֵ�
	tenEqual(horiRtPts, horiRtPtm, equalPtsR, 2);
	//������ȷֵ㵽���������ľ���
	calcuDvsHt(contoursR, equalPtsR, vHtsR, coff);

	//���ȷֵ㵽����Ե����д���ļ�
	name = saveName;
	nameString = name.append("_rightHeightS.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "��short��";
	for (int i = 0; i < 9; i++) {
		m_file << abs(vHtsR[2 * i]) << "��";
	}
	m_file << endl;
	m_file.close();
	//���ȷֵ㵽����Ե����д���ļ�
	name = saveName;
	nameString = name.append("_rightHeightH.txt");
	m_file.open(nameString, ios::app);
	m_file << m_sCrtFrameName << "��high��";
	for (int i = 0; i < 9; i++) {
		m_file << abs(vHtsR[2 * i + 1]) << "��";
	}
	m_file << endl;
	m_file.close();



	return 0;
}







/****************************************************
���ܣ����㵥���ۿ���һά��������
���룺txt��������֣����硰����_�С�������_Ů���ȣ�
������޲������ݵ�ǰ�ˣ����ǻ��ڱ��ر���txt�ļ�

ȫ�ֱ�����ÿ��ͼƬ������   m_sCrtFrameName

��ע������һ���ۿ���BBOX����360�ȵ�������Ե�ľ��룬
	  չ��Ϊһά�������洢��txt��
******************************************************/
int _stdcall calcu1vec(char *imagePath, char *saveName)
{
	cv::Mat src = cv::imread(imagePath);

	string framePath = imagePath;
	size_t Tpos = framePath.find_last_of("\\");			//������\\���һ�γ��ֵ�λ�ã������أ�����0��
	m_sCrtFrameName = framePath.substr(Tpos + 1);		//��ָ��λ�ø������ַ��������ļ������ƣ���ȥ��ǰ���

	float dist[360] = { 0 };
	int ret = oneVec(src, dist);		//����һά����
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
���ܣ������ۿ����¶Խ��ߵĳ���
���룺src  ��Ҫ������ۿ�       //20201230��Ϊ��ͨ��
���������ֵ��Ϊ���¶Խ��ߵĳ���

��ע���Խ����϶˵㣺��һ����ֵ�е�
	  �Խ����¶˵㣺���һ����ֵ�е�
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

	//1����ͷ�����ҵ�һ����ֵ���е㼴Ϊ���϶˵�
	//���ҵ���һ�е����
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
	//ͳ����һ���м���ֵΪ255��
	int cnt = 0;
	uchar* dataUP = bnyMat.ptr<uchar>(upP.y);
	for (int j = 0; j < colNum; j++) {
		if (dataUP[j] == 255) {
			cnt++;
		}
	}
	upP.x = upP.x + cnt / 2;

	
	//2����β���������һ����ֵ���е㼴Ϊ���¶˵�
	//���ҵ����һ�е����
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
	//ͳ����һ���м���ֵΪ255��
	cnt = 0;
	uchar* dataDN = bnyMat.ptr<uchar>(downP.y);
	for (int j = colNum - 1; j >= 0; j--) {
		if (dataDN[j] == 255) {
			cnt++;
		}
	}
	downP.x = downP.x - cnt / 2;

	//��ȫ�ֱ���
	vPtsUD.push_back(upP);
	vPtsUD.push_back(downP);

	//3������������ĳ���
	length = std::pow(std::pow(upP.x - downP.x, 2) + std::pow(upP.y - downP.y, 2), 0.5);


	return 0;
}




/****************************************************
���ܣ������ۿ����ҶԽ��ߵĳ���
���룺src  ��Ҫ������ۿ�  
�����eyeMat   ֻ���ۿ���ͼ��������������
����ֵ��Ϊ���ҶԽ��ߵĳ���

��ע��
�Խ������Ҷ˵㣺���������ȧ��㣨����С��ͨ��
������
1���õ�������ע�������㣻
2����������������߶���ͼ��Ľ���õ���ֱ�ߣ�
3��������ֱ���ҵ����ۿ��������������㡣
******************************************************/
int calcuLengthLR(cv::Mat src, cv::Mat &eyeMat, float &length, std::vector<cv::Point2f> &vPtsLR)
{
	//���������������
	cv::Point2f p1;		
	cv::Point2f p2;
	int ret = 0;

	//�õ������ۿ�ͼ������������
	ret = sprtImage(src, eyeMat, p1, p2);
	if (ret != 0) return ret;

	//if (src.channels() == 3) {
	//	grayMaxChnl(src, grayMat);
	//	threshold(grayMat, bnyMat, 50, 255, THRESH_OTSU);
	//}
	//else {
	//	threshold(src, bnyMat, 50, 255, THRESH_OTSU);
	//}

	//�ڶ�ֵ�ۿ�ͼ����Ѱ���ۿ�����
	vector<vector<cv::Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(eyeMat, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	int a = contours.size();

	if (contours.size() != 1) {
		return EC_NOTEYEMAT;
	}
	////////////��С������//////////////////////////////////
	//cv::Mat dst;
	//cvtColor(eyeMat, dst, COLOR_GRAY2BGR);
	//RotatedRect minrect = minAreaRect(contours[0]);	//��С��Ӿ���
	//Point2f vertices[4];      //����4���������
	//minrect.points(vertices);   //���ĸ���洢��vertices������
	//for (int i = 0; i < 4; i++)
	//{ 
	//	// ע��Scala�д洢˳�� BGR
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
	////�����С����ı���
	//cv::RotatedRect rbox = minAreaRect(contours[0]);

	//��ֱ�߷�����ͼ�Ľ���
	cv::Point2f pU, pD;		//��������ֱ����ͼ�Ľ���
	ret = calcuCross(src, p1, p2, pU, pD);
	if (ret != 0) return ret;
	
	//��������֮����߶Σ��õ��������Ľ���
	LineIterator it(eyeMat, pU, pD, 8);
	std::vector<cv::Point2f> vPts;
	for (int i = 0; i < it.count; i++, ++it) {
		Point pt(it.pos());			//��ȡ�߶��ϵĵ�
		if (abs(pointPolygonTest(contours[0], pt, true)) < 1) {
			vPts.push_back(pt);
			circle(src, pt, 5, Scalar(255, 255, 255), -1);
		}
	}

	//3������������ĳ���
	//�Ե���о��ֻ࣬����������
	int b = vPts.size();

	ret = clusterPts(vPts, vPtsLR);
	b = vPtsLR.size();
	if (vPtsLR.size() != 2) return EC_NOT2CROSS;
	length = std::pow(std::pow(vPtsLR[0].x - vPtsLR[1].x, 2) + std::pow(vPtsLR[0].y - vPtsLR[1].y, 2), 0.5);

	return 0;
}



/****************************************************
���ܣ������ۿ��ĸ����������
���룺eyeMat  ��Ҫ����Ĵ��ۿ�ͼ����ͨ����ֵͼ
�����partAreas     �����ĸ����������

��ע��
L1 ���¶Խ��ߣ� L2 ���ҶԽ���
��ȡ˳������Ҵ��ϵ���
���ۿ�˳��S2,S1,S3,S4
���ۿ�˳��S1,S2,S4,S3

������
1���������أ��ҵ�ǰ����
2���жϸ�����������L1��L2��λ�ù�ϵ
******************************************************/
int calcuPartArea(cv::Mat eyeMat, std::vector<float> &partAreas, sttLine2P L1, sttLine2P L2)
{
	////Ϊ��ֱ����ʾ
	//cv::Mat showMat;
	//cvtColor(eyeMat, showMat, CV_GRAY2BGR);
	//////////////////////////////////////////////

	//����ͼ��������ֵΪ255
	//�������������ң�������ֵ++
	int areaLT = 0;			//��������
	int areaLB = 0;			//��������
	int areaRT = 0;			//��������
	int areaRB = 0;			//��������

	for (int i = 0; i < eyeMat.rows; i++)
	{
		uchar* data = eyeMat.ptr<uchar>(i);
		//uchar* show = showMat.ptr<uchar>(i);
		for (int j = 0; j < eyeMat.cols; j++) {
			//ֻͳ��ǰ������
			if (data[j] == 255)
			{
				int retL1 = ptLtOrRt(Point2f(j, i), L1);		//����ֵ1-��L1��ߣ�2-��L1�ұ�
				int retL2 = ptUpOrDn(Point2f(j, i), L2);		//����ֵ1-��L2���棬2-��L2����
				//�ж����ص�λ���ĸ�����
				if (retL1 == 1 && retL2 == 1) {
					//��λ��L1�������L2�Ϸ�
					areaLT++;
					////Ϊ��ֱ��//////////////
					//show[3 * j] = 128;
					//show[3 * j + 1] = 0;
					//show[3 * j + 2] = 0;
				}
				else if (retL1 == 2 && retL2 == 1) {
					//��λ��L1�ұ�����L2�Ϸ�
					areaRT++;
					////Ϊ��ֱ��//////////////
					//show[3 * j] = 0;
					//show[3 * j + 1] = 128;
					//show[3 * j + 2] = 0;
				}
				else if (retL1 == 1 && retL2 == 2) {
					//��λ��L1�������L2�·�
					areaLB++;
					////Ϊ��ֱ��//////////////
					//show[3 * j] = 0;
					//show[3 * j + 1] = 0;
					//show[3 * j + 2] = 128;
				}
				else if (retL1 == 2 && retL2 == 2) {
					//��λ��L1�ұ�����L2�·�
					areaRB++;
					////Ϊ��ֱ��//////////////
					//show[3 * j] = 128;
					//show[3 * j + 1] = 128;
					//show[3 * j + 2] = 0;
				}
			}
		}
	}

	//�������Ҵ��ϵ��´�������
	partAreas.push_back(areaLT);
	partAreas.push_back(areaRT);
	partAreas.push_back(areaLB);
	partAreas.push_back(areaRB);

	return 0;
}



/****************************************************
���ܣ�ѡ����ͨ����ֵ�����Ǹ�ͨ����Ϊ�Ҷ�ֵ
���룺src  ��ͨ����ɫͼ
�����dst  ��ͨ���Ҷ�ͼ

��ע��ȡ��ͨ���е����ֵ
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
���ܣ��ۿ�ͼ���е���ͨ����룬һ���ۿ�����������
���룺src  ��ͨ����ɫͼ
�����eyeMat-��ͨ�� �ۿ���point-��������������BBOX�����ĵ�


��ע���ۿ���������[0,128,0]������[128,128,0].[128.0.0]
******************************************************/
int sprtImage(cv::Mat src, cv::Mat &eyeMat, cv::Point2f &p1, cv::Point2f &p2)
{
	eyeMat = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
	cv::Mat smallMat = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);

	//�ֽ������������������ۿ����뿪��
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
			//���������ֵeyeMat������ֵsmallMat
			else if ((dataS[3 * j] == 0 && dataS[3 * j + 1] > 0 && dataS[3 * j + 2] == 0) 
					|| (dataS[3 * j] == 0 && dataS[3 * j + 1] == 0 && dataS[3 * j + 2] > 0)) {
				dataE[j] = 255;
			}
			else if ((dataS[3 * j] > 0 && dataS[3 * j + 1] > 0 && dataS[3 * j + 2] == 0)
				|| (dataS[3 * j] > 0 && dataS[3 * j + 1] == 0 && dataS[3 * j + 2] > 0)) {
				//�������ӵ��������߶���ֵ
				dataE[j] = 255;
				dataD[j] = 255;
			}
			else {
				dataD[j] = 255;
			}
		}
	}

	//��smallMat��Ѱ��������ע�������㣬�����BBOX������
	cv::Mat lblMat;
	cv::Mat sttMat;
	cv::Mat cntMat;
	std::vector<cv::Point2f> vPts;

	//������ͨ��
	int nComp = connectedComponentsWithStats(smallMat, lblMat, sttMat, cntMat);
	for (int i = 1; i < nComp; i++) {
		if (sttMat.at<int>(i, CC_STAT_AREA) < COMPTHRE)
		{
			//cv::Rect bbox;
			cv::Point2f p;
			////bounding box���Ͻ�����
			//bbox.x = sttMat.at<int>(i, 0);
			//bbox.y = sttMat.at<int>(i, 1);
			////bouding box�Ŀ�ͳ� 
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
���ܣ��ۿ�ͼ���е���ͨ����룬һ���ۿ�����������
���룺src  ��ͨ����ɫͼ
�����eyeMat-��ͨ�� �ۿ���point-��������������BBOX�����ĵ�


��ע������ͼ�е����㣬�����ֱ����ͼ��Ľ���
******************************************************/
int calcuCross(cv::Mat src, cv::Point2f p1, cv::Point2f p2, cv::Point2f &pU, cv::Point2f &pD)
{
	std::vector<cv::Point2f> vPts;
	std::vector<cv::Point2f> outPts;		//ֻ��������Ԫ��
	if (p1.x != p2.x) {
		float k, b;
		k = (p2.y - p1.y) / (p2.x - p1.x);
		b = p1.y - p1.x * k;
		//����ͼ�������ߣ�Ѱ����ֱ�ߵĽ���
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
		//�ĸ��������⿼��
		if (abs(b) < 1) vPts.push_back(Point2f(0, 0));
		if (abs(src.cols*k + b) < 1) vPts.push_back(Point2f(src.cols, 0));
		if (abs(b - src.rows) < 1) vPts.push_back(Point2f(0, src.rows));
		if (abs(src.cols*k + b - src.rows) < 1) vPts.push_back(Point2f(src.cols, src.rows));

		//�����
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
���ܣ�����࣬һ�ѵ����Ϊ������
���룺vector<cv::Point2f> vPts    �����һ�ѵ�
�����vector<cv::Point2f> outPts  �����������

��ע����������������ľ��볬��10
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
					//�����С��ɾ��
					iterl2 = vPts.erase(iterl2) - 1;

				}
			}
		}
	}

	//�������ĵ㸳ֵ�����
	for (int i = 0; i < vPts.size(); i++) {
		outPts.push_back(vPts[i]);
	}


	return 0;
}




/********************************************************
���ܣ����������������ֱ�ߵ�һ����ʽ

���룺��������[x1,y1],[x2,y2]
�����
A��B��C
Ax+By+C=0��

-����ֵ:
����ɹ�����0
������ͬ����1
********************************************************/
int calcGenLine2P(cv::Point2f p1, cv::Point2f p2, sttLine2P &para)
{
	if ((p1.x == p2.x) && (p1.y == p2.y)) {
		return 1;
	}

	if (p1.x == p2.x) {
		//��ֱֱ��
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
���ܣ��жϵ���ֱ���Ϸ������·�

���룺������꼰ֱ�ߵ�����ʽ����
�����

-����ֵ:
����ֱ���Ϸ�����1
����ֱ���·�����2
��������0
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
���ܣ��жϵ���ֱ����߻����ұ�

���룺������꼰ֱ�ߵ�����ʽ����
�����

-����ֵ:
����ֱ����߷���1
����ֱ���ұ߷���2
��������0
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
���ܣ��������ҶԽ��ߵ㣬ȧ���������ǰ��

���룺���ҶԽ��ߵ�����������ۿ������ۿ���flag
�����������������

��ע��
flag=1�����ۿ���ȧ�������vPts[0];
flag=2�����ۿ���ȧ�������vPts[0];
������
���ۿ���xֵ��С�����ۿ���xֵ�ϴ�
********************************************************/
int findSeamPt(std::vector<cv::Point2f> &vPts, int flag)
{
	cv::Point2f p1, p2;

	if (flag == 1) {
		//�������ۿ�
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
		//�������ۿ�
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
���ܣ���֪ȧ���㣬������ڿ��

���룺ֻ���ۿ���ͼ����ͨ������ ȧ����
�������ȧ����ˮƽ�������ϵĵ㣬 ���ڿ�ȣ� �ۿ�������

��ע��
������
����yֵ��ͬ
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

	//��������Ѱ�Ҷ�Ӧ��ȧ�����ˮƽ��
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
���ܣ���֪��ˮƽ�㣨yֵ��ͬ��������ʮ�ȷֵõ��Ÿ���

���룺yֵ��ͬ�����㣬���ۿ��������ۿ���flag
�������˳������ľŸ���

��ע��
flag=1 ���ۿ�xֵ�Ӵ�С����
flag=2 ���ۿ�xֵ��С��������
������
********************************************************/
int tenEqual(cv::Point p1, cv::Point p2, vector<cv::Point> &vPts, int flag)
{
	float diffX = 0;
	cv::Point pt;
	pt.y = p1.y;

	if (flag == 1) {
		//��Ϊ���ۿ���pt1����ߣ�xֵ��С
		diffX = (p2.x - p1.x) / 10;
		for (int i = 1; i < 10; i++) {
			pt.x = p1.x + i * diffX;
			vPts.push_back(pt);
		}
	}
	else if (flag == 2) {
		//��Ϊ���ۿ���pt2����ߣ�xֵ��С
		diffX = (p1.x - p2.x) / 10;
		for (int i = 1; i < 10; i++) {
			pt.x = p2.x + i * diffX;
			vPts.push_back(pt);
		}
	}


	return 0;
}



/********************************************************
���ܣ���������ϵȷֵ�������Ե�Ϳ���Ե�Ĵ�ֱ����

���룺�ۿ������� 9���㣬 ������
�����9�����Ӧ��18�����루��ʵ���룩

��ע��
���ظ�������coff�õ�����ľ���
������
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


//����BBOX���ĵ��ҵ��ۿ����ĵı�Ե��
//flag=1�����ۿ����Ϊx�Ƚ�С��
//flag=2�����ۿ����Ϊx�Ƚϴ��
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

	//��������Ѱ��ˮƽ��
	if (flag == 1)
	{
		//�������ۿ�����xֵС��
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
		//�������ۿ�����xֵ���
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
���ܣ����㵥�ۿ��ľ���һά����

���룺���ۿ�
�����

��ע��

������
********************************************************/
int oneVec(cv::Mat src, float *ary)
{
	//����ͼ������
	cv::Mat gryMat;
	cv::Mat bnyMat;
	cv::Mat disMat;
	cv::Mat sttMat;
	cv::Mat cntMat;
	cv::Mat lblMat;
	cv::Mat srcCopy = src.clone();
	//�ҶȻ�
	//grayMaxChnl(src, gryMat);
	cvtColor(src, gryMat, CV_BGR2GRAY);
	//��ֵ��
	cv::threshold(gryMat, bnyMat, 23, 255, THRESH_OTSU);
	//��ͨ��
	int nComp = connectedComponentsWithStats(bnyMat, lblMat, sttMat, cntMat);

	//0��Ϊ������������i=1��ʼѭ��
	cv::Rect BBOX;
	std::vector<cv::Rect> vBbox;
	std::vector<cv::Mat> vMat;
	std::vector<int> vArea;
	for (int i = 1; i < nComp; i++) {
		if (sttMat.at<int>(i, CC_STAT_AREA) > 500)
		{
			//bounding box���Ͻ�����
			BBOX.x = sttMat.at<int>(i, 0);
			BBOX.y = sttMat.at<int>(i, 1);
			//bouding box�Ŀ�ͳ� 
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
	drawContours(ctImage, contours, 0, Scalar(255, 255, 255), 2, 8);	//�ҵ�ʱԽ��Ļ����������ڶ�������

	cv::Mat showImage = ctImage.clone();
	cv::circle(showImage, center, 1, Scalar(255, 255, 255), 2, 8, 0);
	cvtColor(ctImage, ctImage, CV_BGR2GRAY);

	//���ҳ���90�ȵ������Ƕȵľ���ֵ
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

		//����lineMat�е�ֱ�ߣ������ϵĵ��ӦshwoImage�е�λ�������������
		bool flag = false;
		for (int x = center.x; x < ctImage.cols && flag == false; x++) {
			for (int y = 0; y < ctImage.rows && flag == false; y++) {
				if (lineMat.at<uchar>(y, x) == 255 && ctImage.at<uchar>(y, x) == 255) {
					cv::circle(showImage, Point2f(x, y), 1, Scalar(0, 0, 255), 1, 8, 0);
					if (i < 90) {
						//��0-90�ȷ�Χ�ڵ��½���
						ary[i] = sqrt(pow(center.x - x, 2) + pow(center.y - y, 2));
					}
					else {
						//��270-360�ȷ�Χ�ڵ��Ͻ���
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
						//��180-270�ȷ�Χ�ڵ��½���
						ary[i + 180] = sqrt(pow(center.x - x, 2) + pow(center.y - y, 2));
					}
					else {
						//��90-180�ȷ�Χ�ڵ��Ͻ���
						ary[i] = sqrt(pow(center.x - x, 2) + pow(center.y - y, 2));
					}
					flag = true;
				}
			}
		}
	}

	//��90�ȵ�
	for (int y = center.y + 10; y < ctImage.rows; y++) {
		if (ctImage.at<uchar>(y, center.x) == 255) {
			ary[90] = y - center.y;
			break;
		}
	}
	//��270�ȵ�
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
















//���������ͷ�ȫ��vector����
void _stdcall postProcess()
{
	//�ͷ��ڴ�
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









