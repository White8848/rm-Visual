#include "opencv2/opencv.hpp"
#include "iostream"
#include "trace.h"
using namespace cv;
using namespace std;

int iLowH, iLowS, iLowV;
int iHighH, iHighS, iHighV;

void chose_color(int color) {
	if (color == 'Red') {
		iLowH = 0; iHighH = 10;
		iLowS = 140; iHighS = 255;
		iLowV = 0; iHighV = 255;
	}
	if (color == 'Gren') {
		iLowH = 66; iHighH = 74;
		iLowS = 250; iHighS = 255;
		iLowV = 0; iHighV = 255;
	}
	if (color == 'Blue') {
		iLowH = 106; iHighH = 110;
		iLowS = 250; iHighS = 255;
		iLowV = 0; iHighV = 255;
	}
	if (color == 'Ylow') {
		iLowH = 16; iHighH = 30;
		iLowS = 155; iHighS = 230;
		iLowV = 0; iHighV = 255;
	}
}