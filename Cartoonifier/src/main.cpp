/*
 * main.cpp
 *
 *  Created on: 2016. 9. 10.
 *      Author: lastkgb
 */

#include <stdio.h>
#include <stdlib.h>

#include "opencv2/opencv.hpp"
#include "cartoon.h"

#define CAMERA_NUMBER 0
#define MY_CAMERA_WIDTH 640
#define MY_CAMERA_HEIGHT 480

#define STICK_FIGURE_ITER_NUM 40
#define WINDOW_NAME "Cartoonifier"

bool g_debugMode = false;
bool g_sketchMode = false;
bool g_alienMode = false;
bool g_evilMode = false;

int g_stickFigureIter = 0;

using namespace cv;
using namespace std;

#define VK_ESCAPE 0x1B

void onKeyPress(char key)
{
	switch(key) {
	case 's':
		break;
	default:
		break;
	}
}

int main(int argc, char* argv[])
{
    cout << "Cartoonifier, by Shervin Emami (www.shervinemami.info), June 2012." << endl;
    cout << "Converts real-life images to cartoon-like images." << endl;
    cout << "Compiled with OpenCV version " << CV_VERSION << endl;
    cout << endl;

    cout << "Keyboard commands (press in the GUI window):" << endl;
    cout << "    Esc:  Quit the program." << endl;
    cout << "    s:    change Sketch / Paint mode." << endl;
    cout << "    a:    change Alien / Human mode." << endl;
    cout << "    e:    change Evil / Good character mode." << endl;
    cout << "    d:    change debug mode." << endl;
    cout << endl;

    VideoCapture camera;

    try {
    	camera.open(CAMERA_NUMBER);
    } catch (cv::Exception &e) {
    	// Do something
    }

    if (!camera.isOpened()) {
    	cerr << "Could not access the camera" << endl;
    	exit(1);
    }

    camera.set(CV_CAP_PROP_FRAME_WIDTH, MY_CAMERA_WIDTH);
    camera.set(CV_CAP_PROP_FRAME_HEIGHT, MY_CAMERA_HEIGHT);

    namedWindow(WINDOW_NAME);

    while (true) {
    	Mat cameraFrame;
    	camera >> cameraFrame;
    	if (cameraFrame.empty()) {
    		cout << "ERROR: could not grab the next camera frame" << endl;
    		exit(1);
    	}

    	Mat displayFrame = Mat(cameraFrame.size(), CV_8UC3);

    	int debugType = 0;
    	if (g_debugMode)
    		debugType = 2;

    	cartoonifyImage(cameraFrame, displayFrame, g_sketchMode, g_alienMode, g_evilMode, debugType);

    	if (g_stickFigureIter > 0) {
    		drawFaceStickFigure(displayFrame);
    		g_stickFigureIter--;
    	}

    	imshow(WINDOW_NAME, displayFrame);

    	char keypress = waitKey(20);
    	if (keypress == VK_ESCAPE)
    		break;
    	if (keypress > 0)
    		onKeyPress(keypress);
    }

	return 0;
}


