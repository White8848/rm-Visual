#include <stdio.h>
#include <iostream>
#include<opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include "trace.h"
#include<memory>
#include<string.h>
//#include "trace.cpp"

using namespace std;
using namespace cv;

extern int iLowH[3], iLowS[3], iLowV[3];
extern int iHighH[3], iHighS[3], iHighV[3];

int angle_x[4];
int angle_y[4];
void showpeacture(Mat peacture);

//初始化尺寸
const int y = 480;
const int x = 640;

//初始化变量
int RGB[3][y][x];//RGB == 012
int R = 0, G = 1, B = 2, Y = 3;
int HSV[3][y][x];//HSV == 012
int H = 0, S = 1, V = 2;
bool rgb_color[y][x][4] = { 0 };//RGBY = 0123
bool hsv_color[y][x][4] = { 0 };
Mat matSrc, matDst_RGB, matDst_HSV;
Mat matDst_GRAY[3];
Mat normImage;//归一化后的图
Mat scaledImage;//线性变换后的八位无符号整型的图

int thresh = 30; //当前阈值
int max_thresh = 175; //最大阈值

void drawRect(const RotatedRect &box, Mat &dst) {
	Point2f vertex[4];
	box.points(vertex);
	//绘制出最小面积的包围矩形
	line(dst, vertex[0], vertex[1], Scalar(100, 200, 211), 5, LINE_AA);
	line(dst, vertex[1], vertex[2], Scalar(100, 200, 211), 5, LINE_AA);
	line(dst, vertex[2], vertex[3], Scalar(100, 200, 211), 5, LINE_AA);
	line(dst, vertex[3], vertex[0], Scalar(100, 200, 211), 5, LINE_AA);
	//绘制中心的光标
	Point s, l, r, u, d;
	s.x = (vertex[0].x + vertex[2].x) / 2.0;
	s.y = (vertex[0].y + vertex[2].y) / 2.0;
	l.x = s.x - 10;
	l.y = s.y;

	r.x = s.x + 10;
	r.y = s.y;

	u.x = s.x;
	u.y = s.y - 10;

	d.x = s.x;
	d.y = s.y + 10;
	line(dst, l, r, Scalar(100, 200, 211), 2, LINE_AA);
	line(dst, u, d, Scalar(100, 200, 211), 2, LINE_AA);

	//各项参数显示 I see
	char move1[20];
	char move2[20];
	sprintf(move1, "x: %d y: %d d: %d", int(s.x) - 320, int(s.y) - 240, int(box.angle + 90));
	sprintf(move2, "h: %d w: %d", int(box.size.height), int(box.size.width));

	putText(dst, move1, s, FONT_HERSHEY_PLAIN, 1, Scalar(124, 252, 0), 1, 8, false);
	putText(dst, move2, Point(s.x, s.y + 15), FONT_HERSHEY_PLAIN, 1, Scalar(124, 252, 0), 1, 8, false);
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
	/*设置摄像头参数 不要随意修改
	capture.set(CV_CAP_PROP_FRAME_WIDTH, 1080);//宽度
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 960);//高度
	capture.set(CV_CAP_PROP_FPS, 30);//帧数
	capture.set(CV_CAP_PROP_BRIGHTNESS, 1);//亮度 1
	capture.set(CV_CAP_PROP_CONTRAST,40);//对比度 40
	capture.set(CV_CAP_PROP_SATURATION, 50);//饱和度 50
	capture.set(CV_CAP_PROP_HUE, 50);//色调 50
	capture.set(CV_CAP_PROP_EXPOSURE, 50);//曝光 50
	*/
	//打印摄像头参数
	printf("width = %.2f\n", cap.get(CV_CAP_PROP_FRAME_WIDTH));
	printf("height = %.2f\n", cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	printf("contrast = %.2f\n", cap.get(CV_CAP_PROP_CONTRAST));
	printf("saturation = %.2f\n", cap.get(CV_CAP_PROP_SATURATION));
	printf("exposure = %.2f\n", cap.get(CV_CAP_PROP_EXPOSURE));

	while (1) {
		cap >> matSrc;
		resize(matSrc, matDst_RGB, Size(x, y), 0, 0, INTER_NEAREST);//获取RGB图像并变换尺寸
		cvtColor(matDst_RGB, matDst_HSV, COLOR_BGR2HSV);//RGB->HSV

		chose_color('Ylow');
		for (int i = 0; i < 3; i++) {
			inRange(matDst_HSV, Scalar(iLowH[i], iLowS[i], iLowV[i]), Scalar(iHighH[i], iHighS[i], iHighV[i]), matDst_GRAY[i]);
		}

		Mat element = getStructuringElement(MORPH_RECT, Size(6, 6));
		Mat element2 = getStructuringElement(MORPH_RECT, Size(4, 4));
		//开操作
		morphologyEx(matDst_GRAY[0], matDst_GRAY[0], MORPH_OPEN, element);
		//闭操作 (连接一些连通域)
		morphologyEx(matDst_GRAY[0], matDst_GRAY[0], MORPH_CLOSE, element2);

		Mat matDst_GRAY_2 = Mat::zeros(matDst_GRAY[0].rows, matDst_GRAY[0].cols, CV_8UC3);
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(matDst_GRAY[0], contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
		int index = 0;

		constexpr auto sizeLimit = 10000.0;
		if (contours.size() > 0) {
			//RotatedRect max = minAreaRect(Mat(contours[0]));
			for (auto contour : contours) {
				auto rect = minAreaRect(Mat(contour));
				if (rect.size.width * rect.size.height > sizeLimit) {
					drawRect(rect, matDst_RGB);
				}
			}
		}
		imshow("GRAY", matDst_GRAY[0]);
		imshow("RGB", matDst_RGB);
		if (waitKey(50) == 27)
			break;
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