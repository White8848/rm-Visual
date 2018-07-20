#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include<opencv.hpp>
#include<ctime>
#include<Windows.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;

bool SetConsoleColor(WORD forceGroundColor, WORD backGroundColor);

void picture_get();
void color_get();
void velue_z();
void judge(int first, int secend);
void showpeacture(Mat peacture);
bool SetConsoleColor(WORD forceGroundColor, WORD backGroundColor);

//const int Height = 5, Width = 5;初始化尺寸
const int Height = 176;
const int Width = 144;

//初始化变量
int RGB[3][Height][Width];//RGB == 012
int R = 0, G = 1, B = 2, Y = 3;
int HSV[3][Height][Width];//HSV == 012
int H = 0, S = 1, V = 2;
bool rgb_color[Height][Width][4] = { 0 };//RGBY = 0123
bool hsv_color[Height][Width][4] = { 0 };
Mat matSrc, matDst_RGB, matDst_HSV, matDst_GRAY;

int main(int argc, char* argv[]) {
	/*图像分辨率定义初始化*/
	int size = Height * Width, Half_size = size / 2;

	//初始化摄像头并读取->matDst
	VideoCapture cap(2);
	if (!cap.isOpened()) {
		return -1;
	}

	for (int i = 0; i < 10; i++) {
		cap >> matSrc;
		resize(matSrc, matDst_RGB, Size(Height, Width), 0, 0, INTER_NEAREST);//获取RGB图像并变换尺寸
		cvtColor(matDst_RGB, matDst_HSV, COLOR_BGR2HSV);//RGB->HSV
		//cvtColor(matDst1, matDst1, CV_BGR2GRAY);
	}
	showpeacture(matDst_RGB);

	while (1) {
		cap >> matSrc;
		resize(matSrc, matDst_RGB, Size(Height, Width), 0, 0, INTER_NEAREST);//获取RGB图像并变换尺寸
		cvtColor(matDst_RGB, matDst_HSV, COLOR_BGR2HSV);//RGB->HSV

		//RGB/HSV图像读取
		picture_get();
		//使用RGB/HSV图像获取颜色
		color_get();
		cvtColor(matDst_RGB, matDst_GRAY, CV_BGR2GRAY);//初始化灰度图像，为二值化做准备
		for (int row = 0; row < Width; row++)
		{
			for (int col = 0; col < Height; col++)
			{
				if (hsv_color[row][col][Y] == 1) {
					matDst_GRAY.at<uchar>(row, col) = 255;
				}
				else matDst_GRAY.at<uchar>(row, col) = 0;
			}
		}
		//showpeacture(matDst_GRAY);

		velue_z();
	}

	return 0;
}

void picture_get() {
	for (int row = 0; row < Width; row++)
	{
		for (int col = 0; col < Height; col++)
		{
			Vec3i bgr = matDst_RGB.at<Vec3b>(row, col);//用Vec3b也行
			RGB[R][row][col] = bgr.val[2];
			RGB[G][row][col] = bgr.val[1];
			RGB[B][row][col] = bgr.val[0];
			Vec3i hsv = matDst_HSV.at<Vec3b>(row, col);
			HSV[H][row][col] = hsv.val[0];
			HSV[S][row][col] = hsv.val[1];
			HSV[V][row][col] = hsv.val[2];
		}
	}
}
void color_get() {
	for (int row = 0; row < Width; row++)
	{
		for (int col = 0; col < Height; col++)
		{
			if (RGB[R][row][col] * 1.5 > RGB[1][row][col] + RGB[1][row][col])rgb_color[row][col][R] = 1;
			else if (RGB[G][row][col] * 1.5 > RGB[2][row][col] + RGB[0][row][col])rgb_color[row][col][G] = 1;
			else if (RGB[B][row][col] * 1.5 > RGB[2][row][col] + RGB[1][row][col])rgb_color[row][col][B] = 1;
			else rgb_color[row][col][5] = 1;

			if (HSV[H][row][col] >= 156 && HSV[H][row][col] <= 180) {
				if (HSV[S][row][col] >= 43 && HSV[S][row][col] <= 255) {
					if (HSV[V][row][col] >= 46 && HSV[V][row][col] <= 255) {
						hsv_color[row][col][R] = 1;
					}
				}
			}
			if (HSV[H][row][col] >= 35 && HSV[H][row][col] <= 77) {
				if (HSV[S][row][col] >= 43 && HSV[S][row][col] <= 255) {
					if (HSV[V][row][col] >= 46 && HSV[V][row][col] <= 255) {
						hsv_color[row][col][G] = 1;
					}
				}
			}
			if (HSV[H][row][col] >= 100 && HSV[H][row][col] <= 124) {
				if (HSV[S][row][col] >= 43 && HSV[S][row][col] <= 255) {
					if (HSV[V][row][col] >= 46 && HSV[V][row][col] <= 255) {
						hsv_color[row][col][B] = 1;
					}
				}
			}
			if (HSV[H][row][col] >= 26 && HSV[H][row][col] <= 34) {
				if (HSV[S][row][col] >= 70 && HSV[S][row][col] <= 255) {
					if (HSV[V][row][col] >= 70 && HSV[V][row][col] <= 255) {
						hsv_color[row][col][Y] = 1;
					}
				}
			}
		}
	}
}

void velue_z() {
	int i, j, k;
	for (k = 0; k < 4; k++) {
		for (i = 0; i < Width; i++) {
			for (j = 0; j < Height; j++) {
				hsv_color[i][j][k] = 0;
				rgb_color[i][j][k] = 0;
			}
		}
	}
}
void judge(int first, int second) {
	int change = 13;
	if (first <= change && second <= change) {
		SetConsoleColor(FOREGROUND_GREEN, FOREGROUND_INTENSITY);
		cout << "empty" << endl;
	}
	else if (first > change && second < change) {
		SetConsoleColor(FOREGROUND_RED, FOREGROUND_INTENSITY);
		cout << "left" << endl;
	}
	else if (first <= change && second > change) {
		SetConsoleColor(FOREGROUND_BLUE, FOREGROUND_INTENSITY);
		cout << "right" << endl;
	}
	else if (first > change && second > change) {
		if (first - second > 5) {
			SetConsoleColor(FOREGROUND_RED, FOREGROUND_INTENSITY);
			cout << "left" << endl;
		}
		else if (second - first > 15) {
			SetConsoleColor(FOREGROUND_BLUE, FOREGROUND_INTENSITY);
			cout << "right" << endl;
		}
		else {
			SetConsoleColor(FOREGROUND_GREEN, FOREGROUND_INTENSITY);
			cout << "bothside" << endl;
		}
	}
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

bool SetConsoleColor(WORD forceGroundColor, WORD backGroundColor) {
	// 获得缓冲区句柄。参数可以选择一下三种值：
	//// STD_INPUT_HANDLE            标准输入的句柄
	//// STD_OUTPUT_HANDLE            标准输出的句柄
	//// STD_ERROR_HANDLE`            标准错误的句柄
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (handle == 0)
		return false;
	//设置文本及背景颜色。
	BOOL ret = SetConsoleTextAttribute(handle, forceGroundColor | backGroundColor);
	return(ret == TRUE);
}