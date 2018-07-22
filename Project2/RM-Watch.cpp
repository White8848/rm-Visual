#include <stdio.h>
#include <iostream>
#include<opencv.hpp>
#include<Windows.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include "trace.h"
#include<memory>
#include<string.h>
//#include "trace.cpp"

using namespace std;
using namespace cv;

extern int iLowH, iLowS, iLowV;
extern int iHighH, iHighS, iHighV;

int angle_x[4];
int angle_y[4];
void showpeacture(Mat peacture);
void showvideo(Mat peacture, int velue);
void on_trackbar(int, void*);

//const int y = 5, x = 5;初始化尺寸
const int y = 480;
const int x = 640;

//int a = 0; 第几个角

//初始化变量
int RGB[3][y][x];//RGB == 012
int R = 0, G = 1, B = 2, Y = 3;
int HSV[3][y][x];//HSV == 012
int H = 0, S = 1, V = 2;
bool rgb_color[y][x][4] = { 0 };//RGBY = 0123
bool hsv_color[y][x][4] = { 0 };
Mat matSrc, matDst_RGB, matDst_HSV;
Mat matDst_GRAY_1;
Mat normImage;//归一化后的图
Mat scaledImage;//线性变换后的八位无符号整型的图

int thresh = 30; //当前阈值
int max_thresh = 175; //最大阈值

Point s1;
void drawRect(const RotatedRect &box, Mat dst) {
	Point2f vertex[4];
	box.points(vertex);
	//绘制出最小面积的包围矩形
	line(dst, vertex[0], vertex[1], Scalar(100, 200, 211), 5, LINE_AA);
	line(dst, vertex[1], vertex[2], Scalar(100, 200, 211), 5, LINE_AA);
	line(dst, vertex[2], vertex[3], Scalar(100, 200, 211), 5, LINE_AA);
	line(dst, vertex[3], vertex[0], Scalar(100, 200, 211), 5, LINE_AA);
	//绘制中心的光标
	Point l, r, u, d;
	s1.x = (vertex[0].x + vertex[2].x) / 2.0;
	s1.y = (vertex[0].y + vertex[2].y) / 2.0;
	l.x = s1.x - 10;
	l.y = s1.y;

	r.x = s1.x + 10;
	r.y = s1.y;

	u.x = s1.x;
	u.y = s1.y - 10;

	d.x = s1.x;
	d.y = s1.y + 10;
	line(dst, l, r, Scalar(100, 200, 211), 2, LINE_AA);
	line(dst, u, d, Scalar(100, 200, 211), 2, LINE_AA);
}
//(240,320)(sl.y-240,s1.x-320)
int main(int argc, char* argv[]) {
	/*图像分辨率定义初始化*/
	int size = y * x;
	//初始化摄像头并读取->matDst
	VideoCapture cap(0);
	if (!cap.isOpened()) {
		return -1;
	}

	//cvInitFont(CvFont* font, int font_face, double hscale,double vscale, double shear = 0,int thickness = 1, int line_type = 8);

	while (1) {
		cap >> matSrc;
		resize(matSrc, matDst_RGB, Size(x, y), 0, 0, INTER_NEAREST);//获取RGB图像并变换尺寸
		cvtColor(matDst_RGB, matDst_HSV, COLOR_BGR2HSV);//RGB->HSV

		chose_color('Ylow');

		inRange(matDst_HSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), matDst_GRAY_1);
		//GaussianBlur(matDst_GRAY_1, matDst_GRAY_1, Size(3, 3), 0);
		//cvtColor(matDst_GRAY_1, matDst_GRAY_1, CV_BGR2GRAY);//初始化灰度图像，为二值化做准备

		Mat element = getStructuringElement(MORPH_RECT, Size(6, 6));
		Mat element2 = getStructuringElement(MORPH_RECT, Size(4, 4));
		morphologyEx(matDst_GRAY_1, matDst_GRAY_1, MORPH_OPEN, element);
		//闭操作 (连接一些连通域)
		morphologyEx(matDst_GRAY_1, matDst_GRAY_1, MORPH_CLOSE, element2);

		Mat matDst_GRAY_2 = Mat::zeros(matDst_GRAY_1.rows, matDst_GRAY_1.cols, CV_8UC3);
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(matDst_GRAY_1, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
		int index = 0;
		//for (; index >= 0; index = hierarchy[index][0])
		//{
		/*
		for (int i = 0; i < contours.size(); ++i) {
			Scalar color(255, 255, 255);
			drawContours(matDst_GRAY_2, contours, index, color, NULL, 8, hierarchy);
		}
		//}
		*/
		Mat matDst_GRAY_3 = matDst_GRAY_2.clone();
		constexpr auto sizeLimit = 10000.0;
		if (contours.size() > 0) {
			//RotatedRect max = minAreaRect(Mat(contours[0]));
			for (auto contour : contours) {
				auto rect = minAreaRect(Mat(contour));
				//if (rect.size.width * rect.size.height > max.size.width * max.size.height)
				//max = rect;
				if (rect.size.width * rect.size.height > sizeLimit)
					drawRect(rect, matDst_RGB);
			}
			//drawRect(max);//这样不是小的也进去了吗//调sizeLimit呗
		}
		//各项参数显示

		char move[20];
		//_itoa(value, move, 10);
		sprintf(move, "(%d, %d)", s1.x - 320, s1.y - 240);
		//char h[20] = "handsome:";
		//strcat(h, move);
		Point point = Point(s1.x, s1.y);
		putText(matDst_RGB, move, point, FONT_HERSHEY_PLAIN, 1, Scalar(124, 252, 0), 1, 8, false);

		imshow("try", matDst_RGB);
		if (waitKey(50) == 27)
			break;
		//showpeacture(matDst_GRAY_1);
		//trace('R');
	}

	return 0;
}

void showpeacture(Mat peacture) {
	namedWindow("MyWindow", CV_WINDOW_AUTOSIZE);
	//在MyWindow的窗中中显示存储在img中的图片

	imshow("MyWindow", peacture);
	//等待直到有键按下
	waitKey(0);
	//销毁MyWindow的窗口
	destroyWindow("MyWindow");
}

/*
void cornerHarris_demo(int, void*)
{
	Mat dst, dst_norm;
	dst = Mat::zeros(src.size(), CV_32FC1);
	/// Detector parameters
	int blockSize = 2;
	int apertureSize = 3;
	double k = 0.04;
	/// Detecting corners
	cornerHarris(src_gray, dst, blockSize, apertureSize, k, BORDER_DEFAULT);
	/// Normalizing
	normalize(dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
	convertScaleAbs(dst_norm, dst_norm_scaled);
	/// Drawing a circle around corners
	for (int j = 0; j < dst_norm.rows; j++)
	{
		for (int i = 0; i < dst_norm.cols; i++)
		{
			if ((int)dst_norm.at<float>(j, i) > thresh)
			{
				angle_x[a] = i;
				angle_y[a] = j;
				a++;
			}
		}
	}
	/// Showing the result
	//imshow(corners_window, dst_norm_scaled);
	//imshow(source_window, src);
}
*/