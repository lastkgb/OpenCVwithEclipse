/*
 * cartoon.cpp
 *
 *  Created on: 2016. 9. 10.
 *      Author: lastkgb
 */

#include <opencv2/opencv.hpp>

#include "cartoon.h"

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
		floodFill(yuv, maskPlusBorder, skinPts[i], Scalar(), NULL, lowerDiff, upperDiff, flags);
		if (debugType >= 1)
			circle(smallImgBGR, skinPts[i], 5, CV_RGB(0, 0, 255), 1, CV_AA);
	}

	if (debugType >= 2)
		imshow("flood mask", mask*120);

	mask -= edgeMask;

	int Red = 0;
	int Green = 70;
	int Blue = 0;
	add (smallImgBGR, Scalar(Blue, Green, Red), smallImgBGR, mask);

	return;
}

void removePepperNoise(Mat &mask)
{
	for (int y=2; y<mask.rows-2; y++) {
		uchar *pThis = mask.ptr(y);
		uchar *pUp1 = mask.ptr(y-1);
		uchar *pUp2 = mask.ptr(y-2);
		uchar *pDown1 = mask.ptr(y+1);
		uchar *pDown2 = mask.ptr(y+2);

		pThis += 2;
		pUp1 += 2;
		pUp2 += 2;
		pDown1 += 2;
		pDown2 += 2;
		for (int x=2; x<mask.cols-2; x++) {
			uchar v = *pThis;
			if (v == 0) {
				bool allAbove = *(pUp2-2) && *(pUp2-1) && *(pUp2) && *(pUp2+1) && *(pUp2+2);
				bool allLeft = *(pUp1-2) && *(pThis-2) && *(pDown1-2);
				bool allBelow = *(pDown2-2) && *(pDown2-1) && *(pDown2) && *(pDown2+1) && *(pDown2+2);
				bool allRight = *(pUp1+2) && *(pThis+2) && *(pDown1+2);
				bool surroundings = allAbove && allLeft && allBelow && allRight;

				if (surroundings) {
					*(pUp1-1) = 255;
					*(pUp1) = 255;
					*(pUp1+1) = 255;
					*(pThis-1) = 255;
					*(pThis) = 255;
					*(pThis+1) = 255;
					*(pDown1-1) = 255;
					*(pDown1) = 255;
					*(pDown1+1) = 255;
				}
				pThis += 2;
				pUp1 += 2;
				pUp2 += 2;
				pDown1 += 2;
				pDown2 += 2;
			}
			pThis++;
			pUp1++;
			pUp2++;
			pDown1++;
			pDown2++;
		}
	}

	return;
}

void drawFaceStickFigure(Mat dst)
{
	Size size = dst.size();
	int sw = size.width;
	int sh = size.height;

	Mat faceOutline = Mat::zeros(size, CV_8UC3);
	Scalar color = CV_RGB(255,255,0);
	int thickness = 4;
	int faceH = sh/2 * 70/100;
	int faceW = faceH * 72/100;
	ellipse(faceOutline, Point(sw/2,sh/2), Size(faceW, faceH), 0, 0, 360, color, thickness, CV_AA);

	int eyeW = faceW * 23/100;
	int eyeH = faceH * 11/100;
	int eyeX = faceW * 48/100;
	int eyeY = faceH * 13/100;
	int eyeA = 15;
	int eyeYshift = 11;
	ellipse(faceOutline, Point(sw/2-eyeX, sh/2-eyeY), Size(eyeW, eyeH), 0, 180+eyeA, 360-eyeA, color, thickness, CV_AA);
	ellipse(faceOutline, Point(sw/2-eyeX, sh/2-eyeY-eyeYshift), Size(eyeW, eyeH), 0, 0+eyeA, 180-eyeA, color, thickness, CV_AA);
	ellipse(faceOutline, Point(sw/2+eyeX, sh/2-eyeY), Size(eyeW, eyeH), 0, 180+eyeA, 360-eyeA, color, thickness, CV_AA);
	ellipse(faceOutline, Point(sw/2+eyeX, sh/2-eyeY-eyeYshift), Size(eyeW, eyeH), 0, 0+eyeA, 180-eyeA, color, thickness, CV_AA);

	int mouthY = faceH * 53/100;
	int mouthW = faceW * 45/100;
	int mouthH = faceH * 6/100;
	ellipse(faceOutline, Point(sw/2, sh/2+mouthY), Size(mouthW, mouthH), 0, 0, 180, color, thickness, CV_AA);

	int fontFace = FONT_HERSHEY_COMPLEX;
	float fontScale = 1.0f;
	int fontThickness = 2;
	putText(faceOutline, "Put your face here", Point(sw*23/100, sh*10/100), fontFace, fontScale, color, fontThickness, CV_AA);

	addWeighted(dst, 1.0, faceOutline, 0.7, 0, dst, CV_8UC3);

	return;
}

