/*
 * cartoon.cpp
 *
 *  Created on: 2016. 9. 10.
 *      Author: lastkgb
 */

#include "cartoon.h"
#include "opencv2/opencv.hpp"


void cartoonifyImage(Mat src, Mat dst, bool sketchMode, bool alienMode, bool evilMode, int debugType)
{
	Mat srcGray;
	cvtColor(src, srcGray, CV_BGR2GRAY);

	medianBlur(srcGray, srcGray, 7);

	Size size = src.size();

	Mat mask = Mat(size, CV_8U);
	Mat edges = Mat(size, CV_8U);

	if (!evilMode) {
		Laplacian(srcGray, edges, CV_8U, 5);
		threshold(edges, mask, 80, 255, THRESH_BINARY_INV);
		removePepperNoise(mask);
	} else {
		Mat edges2;
		Scharr(srcGray, edges, CV_8U, 1, 0);
		Scharr(srcGray, edges2, CV_8U, 1, 0, -1);
		edges += edges2;
		threshold(edges, mask, 12, 255, THRESH_BINARY_INV);
		medianBlur(mask, mask, 3);
	}

	if(sketchMode) {
		cvtColor(mask, dst, CV_GRAY2BGR);
		return;
	}

	Size smallSize;
	smallSize.width = size.width/2;
	smallSize.height = size.height/2;
	Mat smallImg = Mat(smallSize, CV_8UC3);
	resize(src, smallImg, smallSize, 0, 0, INTER_LINEAR);

	Mat tmp = Mat(smallSize, CV_8UC3);
	int repetitions = 7;
	for (int i=0; i<repetitions; i++) {
		int size = 9;
		double sigmaColor = 9;
		double sigmaSpace = 7;
		bilateralFilter(smallImg, tmp, size, sigmaColor, sigmaSpace);
		bilateralFilter(tmp, smallImg, size, sigmaColor, sigmaSpace);
	}

	if (alienMode) {
		changeFacialSkinColor(smallImg, edges, debugType);
	}

	resize(smallImg, src, size, 0, 0, INTER_LINEAR);
	memset((char*) dst.data, 0, dst.step * dst.rows);
	src.copyTo(dst, mask);

	return;
}

void drawFaceStickFigure(Mat dst)
{

}

void changeFacialSkinColor(Mat smallImgBGR, Mat bigEdges, int debugType)
{
	Mat yuv = Mat(smallImgBGR.size(), CV_8UC3);
	cvtColor(smallImgBGR, yuv, CV_BGR2YCrCb);

	int sw = smallImgBGR.cols;
	int sh = smallImgBGR.rows;
	Mat maskPlusBorder = Mat::zeros(sh+2, sw+2, CV_8U);
	Mat mask = maskPlusBorder(Rect(1,1,sw,sh));

	resize(bigEdges, mask, smallImgBGR.size());

	threshold(mask, mask, 80, 255, THRESH_BINARY);
	dilate(mask, mask, Mat());
	erode(mask, mask, Mat());

	int const NUM_SKIN_POINTS = 6;
	Point skinPts[NUM_SKIN_POINTS];
	skinPts[0] = Point(sw/2,				sh/2 - sh/6);
	skinPts[1] = Point(sw/2 - sw/11,	sh/2 - sh/6);
	skinPts[2] = Point(sw/2 + sw/11,	sh/2 - sh/6);
	skinPts[3] = Point(sw/2,				sh/2 + sh/16);
	skinPts[4] = Point(sw/2 - sw/9,		sh/2 + sh/16);
	skinPts[5] = Point(sw/2 + sw/9,		sh/2 + sh/16);
	const int LOWER_Y = 60;
	const int UPPER_Y = 80;
	const int LOWER_Cr = 25;
	const int UPPER_Cr = 15;
	const int LOWER_Cb = 20;
	const int UPPER_Cb = 15;
	Scalar lowerDiff = Scalar(LOWER_Y, LOWER_Cr, LOWER_Cb);
	Scalar upperDiff = Scalar(UPPER_Y, UPPER_Cr, UPPER_Cb);

	Mat edgeMask = mask.clone();
	for (int i=0; i<NUM_SKIN_POINTS; i++) {
		const int flags = 4 | FLOODFILL_FIXED_RANGE | FLOODFILL_MASK_ONLY;
		floodFill(yuv, maskPlusBorder, skinPts[i], scalar(), NULL, lowerDiff, upperDiff, flags);
		if (debugType >= 1)
			circle(smallImgBGR, skinPts[i], 5, CV_RGB(0, 0, 255), 1, CV_AA);
	}


}

void removePepperNoise(Mat &mask)
{

}

void drawFaceStickFigure(Mat dst)
{

}

