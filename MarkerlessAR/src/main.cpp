#include <opencv2/opencv.hpp>
#include <gl/gl.h>
#include <gl/glu.h>

#include "ARDrawingContext.hpp"
#include "ARPipeline.hpp"
#include "DebugHelpers.hpp"

bool processFrame(const cv::Mat& cameraFrame, ARPipeline& pipeline, ARDrawingContext& drawingCtx)
{
	cv::Mat img = cameraFrame.clone();

	if (pipeline.m_patternDetector.enableHomographyRefinement)
		cv::putText(img, "Pose refinement: On   ('h' to switch off)", cv::Point(10,15), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(0,200,0));
	else
		cv::putText(img, "Pose refinement: Off  ('h' to switch on)", cv::Point(10,5), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(0,200,0));

	cv::putText(img, "RANSAC threshold: " + ToString(pipeline.m_patternDetector.homographyReprojectionThreshold) + "(Use'-'/'+' to adjust)", cv::Point(10,30), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(0,200,0));

	drawingCtx.updateBackground(img);
	drawingCtx.isPatternPresent = pipeline.processFrame(cameraFrame);
	drawingCtx.patternPose = pipeline.getPatternLocation();
	drawingCtx.updateWindow();

	int keyCode = cv::waitKey(5);
	bool shouldQuit = false;
	if (keyCode == '+' || keyCode == '=') {
		pipeline.m_patternDetector.homographyReprojectionThreshold += 0.2f;
		pipeline.m_patternDetector.homographyReprojectionThreshold = std::min(10.0f, pipeline.m_patternDetector.homographyReprojectionThreshold);
	} else if (keyCode == '-') {
		pipeline.m_patternDetector.homographyReprojectionThreshold -= 0.2f;
		pipeline.m_patternDetector.homographyReprojectionThreshold = std::max(0.0f, pipeline.m_patternDetector.homographyReprojectionThreshold);
	} else if (keyCode == 'h') {
		pipeline.m_patternDetector.enableHomographyRefinement = !pipeline.m_patternDetector.enableHomographyRefinement;
	} else if (keyCode == 27 || keyCode == 'q') {
		shouldQuit = true;
	}

	return shouldQuit;
}

void processVideo(const cv::Mat& patternImage, CameraCalibration& calibration, cv::VideoCapture& capture)
{
	cv::Mat currentFrame;
	capture >> currentFrame;

	if (currentFrame.empty()) {
		std::cout << "Cannot open video capture device" << std::endl;
		return;
	}

	cv::Size frameSize(currentFrame.cols, currentFrame.rows);

	ARPipeline pipeline(patternImage, calibration);
	ARDrawingContext drawingCtx("Markerless AR", frameSize, calibration);

	bool shouldQuit = false;
	do {
		capture >> currentFrame;
		if (currentFrame.empty()) {
			shouldQuit = true;
			continue;
		}
		shouldQuit = processFrame(currentFrame, pipeline, drawingCtx);
	} while (!shouldQuit);
}

void processSingleImage(const cv::Mat& patternImage, CameraCalibration& calibration, const cv::Mat& image)
{
	cv::Size frameSize(image.cols, image.rows);
	ARPipeline pipeline(patternImage, calibration);
	ARDrawingContext drawingCtx("Markerless AR", frameSize, calibration);

	bool shouldQuit = false;
	do {
		shouldQuit = processFrame(image, pipeline, drawingCtx);
	} while (!shouldQuit);
}

int main(int argc, char *argv[])
{
	CameraCalibration calibration(526.58037f, 524.65577f, 318.41744f, 202.96659f);

	if (argc < 2) {
		std::cout << "Input image not specified" << std::endl;
		std::cout << "Usage: markerlessAR <pattern image> [filepath to be recorded video or image]" << std::endl;
		return 1;
	}

	cv::Mat patternImg = cv::imread(argv[1]);
	if (patternImg.empty()) {
		std::cout << "Input image cannot be read" << std::endl;
		return 2;
	}

	if (argc == 2) {
		cv::VideoCapture temp = cv::VideoCapture();
		processVideo(patternImg, calibration, temp);
	} else if (argc == 3) {
		std::string input = argv[2];
		cv::Mat testImage = cv::imread(input);
		if (!testImage.empty()) {
			processSingleImage(patternImg, calibration, testImage);
		} else {
			cv::VideoCapture cap;
			if (cap.open(input))
				processVideo(patternImg, calibration, cap);
		}
	} else {
		std::cerr << "Invalid number of arguments passed" << std::endl;
		return 1;
	}

	return 0;
}
