/*
 * ARDrawingContext.hpp
 *
 *  Created on: 2016. 9. 20.
 *      Author: lastkgb
 */

#ifndef ARDRAWINGCONTEXT_HPP
#define ARDRAWINGCONTEXT_HPP

#include <opencv2/opencv.hpp>

#include "GeometryTypes.hpp"
#include "CameraCalibration.hpp"

void ARDrawingContextDrawCallback(void* param);

class ARDrawingContext
{
public:
	ARDrawingContext(std::string windowName, cv::Size frameSize, const CameraCalibration& c);
	~ARDrawingContext();

	bool isPatternPresent;
	Transformation patternPose;

	void updateBackground(const cv::Mat& frame);
	void updateWindow();

private:
	friend void ARDrawingContextDrawCallback(void* param);
	void draw();

	void drawCameraFrame();
	void drawAugmentedScene();
	void buildProjectionMatrix(const CameraCalibration& calibration, int w, int h, Matrix44& result);
	void drawCoordinateAxis();
	void drawCubeModel();

private:
	bool m_isTextureInitialized;
	unsigned int m_backgroundTextureId;
	CameraCalibration m_calibration;
	cv::Mat m_backgroundImage;
	std::string m_windowName;
};

#endif /* ARDRAWINGCONTEXT_HPP */
