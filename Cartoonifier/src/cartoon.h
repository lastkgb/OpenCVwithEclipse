/*
 * cartoon.h
 *
 *  Created on: 2016. 9. 10.
 *      Author: lastkgb
 */

#ifndef SRC_CARTOON_H_
#define SRC_CARTOON_H_

#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

void cartoonifyImage(Mat src, Mat dst, bool sketchMode, bool alienMode, bool evilMode, int debugType);
void drawFaceStickFigure(Mat dst);
void changeFacialSkinColor(Mat smallImgBGR, Mat bigEdges, int debugType);
void removePepperNoise(Mat &mask);
void drawFaceStickFigure(Mat dst);



#endif /* SRC_CARTOON_H_ */
