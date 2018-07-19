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

void compare(int x[], int y, int z[], int r);
void judge(int first, int secend);
void showpeacture(Mat peacture);
bool SetConsoleColor(WORD forceGroundColor, WORD backGroundColor);

const int High = 16, Width = 16;
const int Half_High = High / 2, Half_Width = Width / 2, Sum = High * Width, Half_Sum = Sum / 2;

int main(int argc, char* argv[]) {
	//初始化变量

	int i, j, tmp = 0, tmpl = 0;
	/*图像分辨率定义初始化*/

	int size = High * Width, Half_size = size / 2;
	int *left_arr1 = new int[Half_size], *left_arr2 = new int[Half_size];
	int	*right_arr1 = new int[Half_size], *right_arr2 = new int[Half_size];

	int left_iAvg1 = 0, left_iAvg2 = 0, right_iAvg1 = 0, right_iAvg2 = 0;
	int iDiffNum1 = 0, iDiffNum2 = 0;

	Mat matSrc, matDst1, matDst2;
	//初始化摄像头并读取->matDst1
	VideoCapture cap(1);
	if (!cap.isOpened()) {
		return -1;
	}
	for (i = 0; i < 10; i++) {
		Sleep(10);
		cap >> matSrc;
		resize(matSrc, matDst1, Size(High, Width), 0, 0, INTER_NEAREST);
		cvtColor(matDst1, matDst1, CV_BGR2GRAY);
	}

	showpeacture(matDst1);

	//初始化图像并分析

	for (i = 0; i < Width; i++) {
		uchar* data1 = matDst1.ptr<uchar>(i);
		for (j = 0; j < Half_Width; j++) {
			left_arr1[tmp] = data1[j];
			left_iAvg1 += left_arr1[tmp];
			tmp++;
		}
		for (j = Half_Width; j < Width; j++) {
			right_arr1[tmpl] = data1[j];
			right_iAvg1 += right_arr1[tmpl];
			tmpl++;
		}
	}
	left_iAvg1 /= Half_Sum;
	right_iAvg1 /= Half_Sum;

	cout << left_iAvg1 << endl << right_iAvg1 << endl;

	compare(left_arr1, left_iAvg1, right_arr1, right_iAvg1);//判断是否改变。变大：1 不变：0 变小：-1

	//初始化结束，开始监控

	while (1) {
		//读取摄像头->matDst2
		cap >> matSrc;
		//imshow("camera", matSrc);//显示
		resize(matSrc, matDst2, Size(High, Width), 0, 0, INTER_NEAREST);
		cvtColor(matDst2, matDst2, CV_BGR2GRAY);
		tmp = tmpl = 0;

		for (i = 0; i < Width; i++) {
			uchar* data2 = matDst2.ptr<uchar>(i);
			for (j = 0; j < Half_Width; j++) {
				left_arr2[tmp] = data2[j];
				left_iAvg2 += left_arr2[tmp];
				tmp++;
			}
			for (j = Half_Width; j < Width; j++) {
				right_arr2[tmpl] = data2[j];
				right_iAvg2 += right_arr2[tmpl];
				tmpl++;
			}
		}

		left_iAvg2 /= Half_Sum;
		right_iAvg2 /= Half_Sum;

		compare(left_arr2, left_iAvg2, right_arr2, right_iAvg2);//判断

		for (i = 0; i < Half_Sum; i++) {
			if (left_arr1[i] != left_arr2[i])
				++iDiffNum1;
			if (right_arr1[i] != right_arr2[i])
				++iDiffNum2;
		}

		cout << "iDffNum1=" << iDiffNum1 << endl;
		cout << "iDffNum2=" << iDiffNum2 << endl;
		cout << "gray1=" << left_iAvg2 << endl;
		cout << "gray2=" << right_iAvg2 << endl;

		judge(iDiffNum1, iDiffNum2);//最终判断
		SetConsoleColor(FOREGROUND_INTENSITY, FOREGROUND_INTENSITY);
		//iDiffNum1左，iDiffNum2右

		iDiffNum1 = iDiffNum2 = 0;

		Sleep(10);
	}
	return 0;
}
void compare(int x[], int y, int z[], int r) {
	int i;
	int change = 6;
	for (i = 0; i < Half_Sum; i++) {
		if (x[i] - y > change) x[i] = 1;//变大
		else if (x[i] - y <= change && y - x[i] <= change)x[i] = 0;//不变
		else if (y - x[i] > change)x[i] = -1;//变小
		if (z[i] - r > change)z[i] = 1;
		else if (z[i] - r <= change && r - z[i] <= change)z[i] = 0;
		else if (r - z[i] > change)z[i] = -1;
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