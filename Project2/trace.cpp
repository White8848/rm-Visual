#include "opencv2/opencv.hpp"
#include "iostream"
#include "trace.h"
using namespace cv;
using namespace std;

int iLowH, iLowS, iLowV;
int iHighH, iHighS, iHighV;

void chose_color(int color) {
	if (color == 0) {
		iLowH = 156; iHighH = 180;
		iLowS = 140; iHighS = 255;
		iLowV = 0; iHighV = 255;
	}
	if (color == 1) {
		iLowH = 66; iHighH = 80;
		iLowS = 130; iHighS = 255;
		iLowV = 0; iHighV = 255;
	}
	if (color == 2) {
		iLowH = 100; iHighH = 124;
		iLowS = 125; iHighS = 255;
		iLowV = 0; iHighV = 255;
	}
	if (color == 3) {
		iLowH = 14; iHighH = 23;
		iLowS = 130; iHighS = 255;
		iLowV = 0; iHighV = 255;
	}
}