#include "opencv2/opencv.hpp"
#include "iostream"
#include "trace.h"
using namespace cv;
using namespace std;

int iLowH, iLowS, iLowV;
int iHighH, iHighS, iHighV;

void chose_color(int color) {
	if (color == 'Red') {
		iLowH = 100; iHighH = 100;
		iLowS = 100; iHighS = 100;
		iLowV = 100; iHighV = 100;
	}
	if (color == 'Gren') {
		iLowH = 100; iHighH = 100;
		iLowS = 100; iHighS = 100;
		iLowV = 100; iHighV = 100;
	}
	if (color == 'Blue') {
		iLowH = 100; iHighH = 100;
		iLowS = 100; iHighS = 100;
		iLowV = 100; iHighV = 100;
	}
	if (color == 'Ylow') {
		iLowH = 17; iHighH = 26;
		iLowS = 170; iHighS = 255;
		iLowV = 0; iHighV = 255;
	}
}