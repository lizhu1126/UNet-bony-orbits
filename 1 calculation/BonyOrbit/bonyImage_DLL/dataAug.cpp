#include "dataAug.h"

using namespace std;
using namespace cv;


int _stdcall dataAugment1(char *imagePath, int cnt)
{
	cv::Mat m_ctMat = cv::imread(imagePath);
	string framePath = imagePath;

	size_t Tpos = framePath.find_last_of("\\");			//查找在\\最后一次出现的位置，并返回（包括0）
	string m_sCrtName = framePath.substr(Tpos + 1);		//从指定位置复制子字符串，即文件的名称，且去掉前面的

	int strLength = strlen(framePath.c_str()) - strlen(m_sCrtName.c_str());
	string savePath = framePath.substr(0, strLength);

	if (m_ctMat.empty()) {
		return EC_NOIMAGEINPUT;
	}

	//实现建立一个叫“procesed”的文件夹
	string name = savePath.append("ag\\") + m_sCrtName.substr(0, m_sCrtName.length() - 4);		//保存路径及名字

	////不同颜色赋值不同背景为0，其他1,2,3,4,5,6
	//cv::Mat res(m_ctMat.rows, m_ctMat.cols, CV_8UC1);
	//res = 0;

	//int rowNum = m_ctMat.rows;
	//int colNum = m_ctMat.cols;
	//for (int i = 0; i < rowNum; i++) {
	//	uchar* src = m_ctMat.ptr<uchar>(i);
	//	uchar* dst = res.ptr<uchar>(i);
	//	for (int j = 0; j < colNum; j++) {
	//		if (src[3 * j] == 0 && src[3 * j + 1] == 128 && src[3 * j + 2] == 0) {
	//			////左大
	//			//dst[j] = 1;
	//			dst[j] = 1;
	//		}
	//		else if (src[3 * j] == 0 && src[3 * j + 1] == 0 && src[3 * j + 2] == 128) {
	//			////右大
	//			//dst[j] = 1;
	//			dst[j] = 1;
	//		}
	//		//else if (src[3 * j] == 128 && src[3 * j + 1] == 0 && src[3 * j + 2] == 0) {
	//		//	////左左
	//		//	//dst[j] = 3;
	//		//}
	//		//else if (src[3 * j] == 128 && src[3 * j + 1] == 128 && src[3 * j + 2] == 0) {
	//		//	////左右
	//		//	//dst[j] = 4;
	//		//}
	//		//else if (src[3 * j] == 128 && src[3 * j + 1] == 0 && src[3 * j + 2] == 128) {
	//		//	////右左
	//		//	//dst[j] = 5;
	//		//}
	//		//else if(src[3 * j] == 0 && src[3 * j + 1] == 128 && src[3 * j + 2] == 128) {
	//		//	////右右
	//		//	//dst[j] = 6;
	//		//}
	//	}
	//}

	//cvtColor(res, res, CV_GRAY2BGR);m_ctMat
	cv::Mat res;
	cvtColor(m_ctMat, res, CV_BGR2GRAY);
	res = res(Rect(20, 12, 576, 320));
	string name4 = name.append(".png");
	cv::imwrite(name4, res);



	return 0;
}


int _stdcall dataAugment(char *imagePath, int cnt)
{
	cv::Mat m_ctMat = cv::imread(imagePath);
	string framePath = imagePath;

	size_t Tpos = framePath.find_last_of("\\");			//查找在\\最后一次出现的位置，并返回（包括0）
	string m_sCrtName = framePath.substr(Tpos + 1);		//从指定位置复制子字符串，即文件的名称，且去掉前面的
	
	int strLength = strlen(framePath.c_str()) - strlen(m_sCrtName.c_str());
	string savePath = framePath.substr(0, strLength);

	if (m_ctMat.empty()) {
		return EC_NOIMAGEINPUT;
	}

	//实现建立一个叫“procesed”的文件夹
	string name = savePath.append("ag\\") + m_sCrtName.substr(0, m_sCrtName.length() - 4);		//保存路径及名字


	///////////////添加噪声等///////////////////////////////

	/////////////单纯裁剪，用于test///////////////////////////////
	////寻找最左边和最上边的点
	//int colNum = 0;
	//int rowNum = 0;
	//findFstCol(m_ctMat, colNum, rowNum);
	//if (colNum > 240) {
	//	colNum = 240;
	//}
	////cv::Mat saveOri = m_ctMat(Rect(colNum, 0, 360, 360));		//需要裁减时要这行
	cv::Mat saveOri = m_ctMat;		//不需要裁减时要这行

	cv::Mat imgSalt = saltAndPepper(saveOri, 0.3);
	cv::Mat imgGauss = addGaussianNoise(saveOri, 0.3);
	cv::Mat imgdarker = darker(saveOri);
	cv::Mat imgBrighter = brighter(saveOri);
	cv::Mat imgRotation1 = rotation(saveOri, 5);		//逆时针
	cv::Mat imgRotation2 = rotation(saveOri, -5);		//顺时针

	string nameTemp = name;
	string name1 = nameTemp.append("_salt").append(".png");
	nameTemp = name;
	string name2 = nameTemp.append("_gauss").append(".png");
	nameTemp = name;
	string name3 = nameTemp.append("_dark").append(".png");
	nameTemp = name;
	string name4 = nameTemp.append("_bright").append(".png");
	nameTemp = name;
	string name5 = nameTemp.append("_rotat1").append(".png");
	nameTemp = name;
	string name6 = nameTemp.append("_rotat2").append(".png");

	cv::imwrite(name.append(".png"), saveOri);
	cv::imwrite(name1, imgSalt);
	cv::imwrite(name2, imgGauss);
	cv::imwrite(name3, imgdarker);
	cv::imwrite(name4, imgBrighter);
	cv::imwrite(name5, imgRotation1);
	cv::imwrite(name6, imgRotation2);
	////////////////////////////////////////////////////////////////////

	///////////////单纯裁剪，用于test///////////////////////////////
	////寻找最左边和最上边的点
	//int colNum = 0;
	//int rowNum = 0;
	//findFstCol(m_ctMat, colNum, rowNum);
	//if (colNum > 240) {
	//	colNum = 240;
	//}
	//string name0 = name.append(".png");
	//cv::Mat saveOri = m_ctMat(Rect(colNum, 0, 360, 360));
	//cv::imwrite(name0, saveOri);
	//////////////////////////////////////////////////////////


	/////////////裁剪并扩充///////////////////////////////
	////转为眼眶区域白，其他区域黑的三通道图
	//cv::Mat whiteMat;
	//bnyEyeMat(m_ctMat, whiteMat);

	////寻找最左边和最上边的点
	//int colNum = 0;
	//int rowNum = 0;
	//findFstCol(whiteMat, colNum, rowNum);
	//if (colNum > 240) {
	//	colNum = 240;
	//}
	////colNum = 0;
	////保存原始图像裁剪后的黑白图
	//string temp = name;
	//string name0 = temp.append("0").append(".png");
	////string name0 = temp.append(".png");
	//cv::Mat saveOri = whiteMat(Rect(colNum, 0, 360, 360));
	//cv::imwrite(name0, saveOri);

	////保存原始图像裁剪后的黑白图
	////string temp = name;
	////string name0 = temp.append(".png");
	////cv::imwrite(name0, dst);
	//if (rowNum > OFFSETNUM + 10) {
	//	offSetMat3(whiteMat, name, colNum, OFFSETNUM+10);
	//}
	//else if(rowNum > OFFSETNUM) {
	//	offSetMat2(whiteMat, name, colNum, OFFSETNUM);
	//}
	////////////////////////////////////////////////////////



	//cv::Mat dst = cv::Mat::zeros(whiteMat.rows, whiteMat.cols, CV_8UC1);
	//int rowNum = dst.rows;
	//int colNum = dst.cols * dst.channels();
	//for (int i = 0; i < rowNum; i++) {
	//	uchar* dataS = whiteMat.ptr<uchar>(i);
	//	uchar* dataD = dst.ptr<uchar>(i);
	//	for (int j = 0; j < colNum; j++) {
	//		dataD[j] = max({ dataS[3 * j], dataS[3 * j + 1],dataS[3 * j + 2] });
	//		if (dataD[j] != 0) {
	//			dataD[j] = 255;
	//		}
	//	}
	//}
	//cv::Mat ans;
	//cvtColor(dst, ans, CV_GRAY2BGR);
	//for (int i = 0; i < rowNum; i++) {
	//	uchar* dataS = dst.ptr<uchar>(i);
	//	uchar* dataD = ans.ptr<uchar>(i);
	//	for (int j = 0; j < colNum; j++) {
	//		if (dataS[j] == 255) {
	//			dataD[3*j] = 0;
	//			dataD[3*j + 1] = 0;
	//			dataD[3*j + 2] = 128;
	//		}
	//	}
	//}
	

	return 0;
}





//将五颜六色的图转为只有眼眶区域时白色的图（三通道）
int bnyEyeMat(cv::Mat src, cv::Mat &dst)
{
	//dst = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
	////现将两个特征点与整个眼眶分离开来
	//int rowNum = src.rows;
	//int colNum = src.cols;
	//for (int i = 0; i < rowNum; i++) {
	//	uchar* dataS = src.ptr<uchar>(i);
	//	uchar* dataE = dst.ptr<uchar>(i);
	//	for (int j = 0; j < colNum; j++) {
	//		if (dataS[3 * j] == 0 && dataS[3 * j + 1] == 0 && dataS[3 * j + 2] == 0) {
	//			dataE[j] = 0;
	//		}
	//		//若红或绿则赋值eyeMat，否则赋值smallMat
	//		else if ((dataS[3 * j] == 0 && dataS[3 * j + 1] > 0 && dataS[3 * j + 2] == 0)
	//			|| (dataS[3 * j] == 0 && dataS[3 * j + 1] == 0 && dataS[3 * j + 2] > 0)) {
	//			dataE[j] = 255;
	//		}
	//		else if ((dataS[3 * j] > 0 && dataS[3 * j + 1] > 0 && dataS[3 * j + 2] == 0)
	//			|| (dataS[3 * j] > 0 && dataS[3 * j + 1] == 0 && dataS[3 * j + 2] > 0)) {
	//			//靠近鼻子的特征两者都赋值
	//			dataE[j] = 255;
	//		}
	//	}
	//}
	//cvtColor(dst, dst, CV_GRAY2BGR);

	dst = src.clone();
	for (int i = 0; i < src.rows; i++)
	{
		uchar* data = dst.ptr<uchar>(i);
		for (int j = 0; j < src.cols*src.channels(); j=j+3) {
			if ((data[j] == 128 && data[j + 1] == 0 && data[j + 2] == 0)
				|| (data[j] == 0 && data[j + 1] == 128 && data[j + 2] == 128)) {
				data[j] = 0;
				data[j + 1] = 0;
				data[j + 2] = 0;
			}
			else if ((data[j] == 128 && data[j + 1] == 128 && data[j + 2] == 0)) {
				data[j] = 0;
				data[j + 1] = 128;
				data[j + 2] = 0;
			}
			else if ((data[j] == 128 && data[j + 1] == 0 && data[j + 2] == 128)) {
				data[j] = 0;
				data[j + 1] = 0;
				data[j + 2] = 128;
			}
		}
	}


	return 0;
}


//上下左右各平移20个像素，一张图扩充4倍并保存
int offSetMat(cv::Mat src, string imageName, int num)
{
	//定义四个图像
	cv::Mat tempMat1(src.rows + num, src.cols + num, src.type());
	cv::Mat tempMat2;
	cv::Mat tempMat3;
	cv::Mat tempMat4;
	cv::Mat tempMat5;
	cv::Mat tempMat6;
	tempMat1 = 0;
	tempMat1.copyTo(tempMat2);
	tempMat1.copyTo(tempMat3);
	tempMat1.copyTo(tempMat4);
	tempMat1.copyTo(tempMat5);
	tempMat1.copyTo(tempMat6);

	//定义3个区域Rect
	cv::Rect roi1 = cv::Rect(0, 0, src.cols, src.rows);
	cv::Rect roi2 = cv::Rect(0, num, src.cols, src.rows);
	cv::Rect roi3 = cv::Rect(num, 0, src.cols, src.rows);
	cv::Rect roi4 = cv::Rect(num, num, src.cols, src.rows);

	//向上平移
	src.copyTo(tempMat1(roi1));
	tempMat1 = tempMat1(roi2);

	//向下平移
	src.copyTo(tempMat2(roi2));
	tempMat2 = tempMat2(roi1);

	//向左平移
	src.copyTo(tempMat3(roi1));
	tempMat3 = tempMat3(roi3);

	//向右平移
	src.copyTo(tempMat4(roi3));
	tempMat4 = tempMat4(roi1);

	//向左上平移
	src.copyTo(tempMat5(roi1));
	tempMat5 = tempMat5(roi4);

	//向右下平移
	src.copyTo(tempMat6(roi4));
	tempMat6 = tempMat6(roi1);


	////全部裁剪为360x360
	//cv::Rect roi = cv::Rect(120, 0, 360, 360);
	//tempMat1 = tempMat1(roi);
	//tempMat2 = tempMat2(roi);
	//tempMat3 = tempMat3(roi);
	//tempMat4 = tempMat4(roi);
	//tempMat5 = tempMat5(roi);
	//tempMat6 = tempMat6(roi);


	//保存四个图像
	string nameTemp = imageName;
	string name1 = nameTemp.append("1").append(".png");
	nameTemp = imageName;
	string name2 = nameTemp.append("2").append(".png");
	nameTemp = imageName;
	string name3 = nameTemp.append("3").append(".png");
	nameTemp = imageName;
	string name4 = nameTemp.append("4").append(".png");
	nameTemp = imageName;
	string name5 = nameTemp.append("5").append(".png");
	nameTemp = imageName;
	string name6 = nameTemp.append("6").append(".png");

	cv::imwrite(name1, tempMat1);
	cv::imwrite(name2, tempMat2);
	cv::imwrite(name3, tempMat3);
	cv::imwrite(name4, tempMat4);
	cv::imwrite(name5, tempMat5);
	cv::imwrite(name6, tempMat6);

	return 0;
}



//寻找第一列有值的列
int findFstCol(cv::Mat src, int &numL, int &numR)
{
	vector<int> numsL;
	vector<int> numsR;

	cvtColor(src, src, CV_BGR2GRAY);

	int rowNum = src.rows;
	int colNum = src.cols;

	for (int i = 0; i < rowNum; i++) {
		uchar *data = src.ptr<uchar>(i);
		for (int j = 0; j < colNum; j++) {
			if (data[j] != 0) {
				numsL.push_back(j);
				numsR.push_back(i);
				break;
			}
				
		}
	}

	sort(numsL.begin(), numsL.end());
	sort(numsR.begin(), numsR.end());
	numL = numsL[0] - 25;
	numR = numsR[0] - 25;

	return 0;
}


//上下平移10个和20个像素，一张图扩充4倍并保存
int offSetMat2(cv::Mat src, string imageName, int fstColNum, int offNum)
{
	//定义四个图像
	cv::Mat tempMat1(src.rows + offNum, src.cols + offNum, src.type());
	cv::Mat tempMat2;
	cv::Mat tempMat3;
	cv::Mat tempMat4;
	//cv::Mat tempMat5;
	//cv::Mat tempMat6;
	tempMat1 = 0;
	tempMat1.copyTo(tempMat2);
	tempMat1.copyTo(tempMat3);
	tempMat1.copyTo(tempMat4);
	//tempMat1.copyTo(tempMat5);
	//tempMat1.copyTo(tempMat6);

	//定义3个区域Rect
	cv::Rect roi1 = cv::Rect(0, 0, src.cols, src.rows);
	cv::Rect roi2 = cv::Rect(0, offNum, src.cols, src.rows);

	cv::Rect roi3 = cv::Rect(0, offNum / 2, src.cols, src.rows);
	//cv::Rect roi4 = cv::Rect(offNum, offNum, src.cols, src.rows);

	//向上平移20
	src.copyTo(tempMat1(roi1));
	tempMat1 = tempMat1(roi2);

	//向下平移20
	src.copyTo(tempMat2(roi2));
	tempMat2 = tempMat2(roi1);

	//向上平移10
	src.copyTo(tempMat3(roi1));
	tempMat3 = tempMat3(roi3);

	//向下平移10
	src.copyTo(tempMat4(roi3));
	tempMat4 = tempMat4(roi1);


	////全部裁剪为360x360
	tempMat1 = tempMat1(Rect(fstColNum, 0, 360, 360));
	tempMat2 = tempMat2(Rect(fstColNum, 0, 360, 360));
	tempMat3 = tempMat3(Rect(fstColNum, 0, 360, 360));
	tempMat4 = tempMat4(Rect(fstColNum, 0, 360, 360));


	//保存四个图像
	string nameTemp = imageName;
	string name1 = nameTemp.append("1").append(".png");
	nameTemp = imageName;
	string name2 = nameTemp.append("2").append(".png");
	nameTemp = imageName;
	string name3 = nameTemp.append("3").append(".png");
	nameTemp = imageName;
	string name4 = nameTemp.append("4").append(".png");
	//nameTemp = imageName;
	//string name5 = nameTemp.append("5").append(".png");
	//nameTemp = imageName;
	//string name6 = nameTemp.append("6").append(".png");

	cv::imwrite(name1, tempMat1);
	cv::imwrite(name2, tempMat2);
	cv::imwrite(name3, tempMat3);
	cv::imwrite(name4, tempMat4);
	//cv::imwrite(name5, tempMat5);
	//cv::imwrite(name6, tempMat6);

	return 0;
}



int offSetMat3(cv::Mat src, string imageName, int fstColNum, int offNum)
{
	//定义四个图像
	cv::Mat tempMat1(src.rows + offNum, src.cols + offNum, src.type());
	cv::Mat tempMat2;
	cv::Mat tempMat3;
	cv::Mat tempMat4;
	cv::Mat tempMat5;
	cv::Mat tempMat6;
	tempMat1 = 0;
	tempMat1.copyTo(tempMat2);
	tempMat1.copyTo(tempMat3);
	tempMat1.copyTo(tempMat4);
	tempMat1.copyTo(tempMat5);
	tempMat1.copyTo(tempMat6);

	//定义3个区域Rect
	cv::Rect roi1 = cv::Rect(0, 0, src.cols, src.rows);
	cv::Rect roi2 = cv::Rect(0, offNum, src.cols, src.rows);

	cv::Rect roi3 = cv::Rect(0, offNum / 2, src.cols, src.rows);
	cv::Rect roi4 = cv::Rect(0, offNum / 3, src.cols, src.rows);

	//向上平移20
	src.copyTo(tempMat1(roi1));
	tempMat1 = tempMat1(roi2);

	//向下平移20
	src.copyTo(tempMat2(roi2));
	tempMat2 = tempMat2(roi1);

	//向上平移15
	src.copyTo(tempMat3(roi1));
	tempMat3 = tempMat3(roi3);

	//向下平移15
	src.copyTo(tempMat4(roi3));
	tempMat4 = tempMat4(roi1);

	//向上平移10
	src.copyTo(tempMat5(roi1));
	tempMat5 = tempMat5(roi4);

	//向下平移10
	src.copyTo(tempMat6(roi4));
	tempMat6 = tempMat6(roi1);


	////全部裁剪为360x360
	tempMat1 = tempMat1(Rect(fstColNum, 0, 360, 360));
	tempMat2 = tempMat2(Rect(fstColNum, 0, 360, 360));
	tempMat3 = tempMat3(Rect(fstColNum, 0, 360, 360));
	tempMat4 = tempMat4(Rect(fstColNum, 0, 360, 360));
	tempMat5 = tempMat5(Rect(fstColNum, 0, 360, 360));
	tempMat6 = tempMat6(Rect(fstColNum, 0, 360, 360));

	//保存四个图像
	string nameTemp = imageName;
	string name1 = nameTemp.append("1").append(".png");
	nameTemp = imageName;
	string name2 = nameTemp.append("2").append(".png");
	nameTemp = imageName;
	string name3 = nameTemp.append("3").append(".png");
	nameTemp = imageName;
	string name4 = nameTemp.append("4").append(".png");
	nameTemp = imageName;
	string name5 = nameTemp.append("5").append(".png");
	nameTemp = imageName;
	string name6 = nameTemp.append("6").append(".png");

	cv::imwrite(name1, tempMat1);
	cv::imwrite(name2, tempMat2);
	cv::imwrite(name3, tempMat3);
	cv::imwrite(name4, tempMat4);
	cv::imwrite(name5, tempMat5);
	cv::imwrite(name6, tempMat6);

	return 0;
}


cv::Mat saltAndPepper(cv::Mat src, int perct)
{
	cv::Mat noiseMat = src.clone();
	int n = src.rows*src.cols*src.channels()*0.03;
	for (int k = 0; k < n; k++)
	{
		//随机选取行列值
		int i = rand() % noiseMat.cols;
		int j = rand() % noiseMat.rows;
		if (noiseMat.channels() == 1)
		{
			noiseMat.at<uchar>(j, i) = 255;
		}
		else
		{
			noiseMat.at<Vec3b>(j, i)[0] = 128;
			noiseMat.at<Vec3b>(j, i)[1] = 128;
			noiseMat.at<Vec3b>(j, i)[2] = 128;
		}
	}
	return noiseMat;
}


cv::Mat addGaussianNoise(cv::Mat src, int perct)
{
	Mat result = src.clone();
	int channels = result.channels();
	int nRows = result.rows;
	int nCols = result.cols*channels;
	if (result.isContinuous())
	{
		nCols = nCols*nRows;
		nRows = 1;
	}
	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < nCols; j++)
		{
			int val = result.ptr<uchar>(i)[j] + generateGaussianNoise(2, 0.8) * 16;
			if (val < 0)
				val = 0;
			if (val > 255)
				val = 255;
			result.ptr<uchar>(i)[j] = (uchar)val;
		}
	}
	return result;
}

//给图像添加高斯噪声
double generateGaussianNoise(double mu, double sigma)
{
	//定义最小值
	double epsilon = numeric_limits<double>::min();
	double z0 = 0, z1 = 0;
	bool flag = false;
	flag = !flag;
	if (!flag)
		return z1*sigma + mu;
	double u1, u2;
	do
	{
		u1 = rand()*(1.0 / RAND_MAX);
		u2 = rand()*(1.0 / RAND_MAX);
	} while (u1 <= epsilon);
	z0 = sqrt(-2.0*log(u1))*cos(2 * CV_PI*u2);
	z1 = sqrt(-2.0*log(u1))*sin(2 * CV_PI*u2);
	return z0*sigma + mu;
}

cv::Mat darker(cv::Mat src)
{
	cv::Mat result = src.clone();

	int rowNum = src.rows;
	int colNum = src.cols*src.channels();

	for (int i = 0; i < rowNum; i++) {
		uchar *data = result.ptr<uchar>(i);
		for (int j = 0; j < colNum; j++) {
			if (data[j] != 0) {
				data[j] = 80;
			}

		}
	}

	return result;
}


cv::Mat brighter(cv::Mat src)
{
	cv::Mat result = src.clone();

	int rowNum = src.rows;
	int colNum = src.cols*src.channels();

	for (int i = 0; i < rowNum; i++) {
		uchar *data = result.ptr<uchar>(i);
		for (int j = 0; j < colNum; j++) {
			if (data[j] != 0) {
				data[j] = 200;
			}

		}
	}

	return result;
}


cv::Mat rotation(cv::Mat src, double degree)
{
	cv::Mat result = src.clone();

	int rowNum = src.rows;
	int colNum = src.cols*src.channels();

	Point2f center(src.cols / 2, src.rows / 2);
	cv::Mat rotm = getRotationMatrix2D(center, degree, 1.0);

	warpAffine(src, result, rotm, src.size());

	return result;
}











