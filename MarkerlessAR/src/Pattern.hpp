/*
 * Pattern.hpp
 *
 *  Created on: 2016. 9. 19.
 *      Author: lastkgb
 */

#ifndef PATTERN_HPP
#define PATTERN_HPP

#include <opencv2/opencv.hpp>

#include "GeometryTypes.hpp"
#include "CameraCalibration.hpp"

struct Pattern
{
	cv::Size size;
	cv::Mat frame;
	cv::Mat grayImg;

	std::vector<cv::KeyPoint> keypoints;
	cv::Mat descriptors;

	std::vector<cv::Point2f> points2d;
	std::vector<cv::Point3f> points3d;
};

struct PatternTrackingInfo
{
	cv::Mat homography;
	std::vector<cv::Point2f> points2d;
	Transformation pose3d;

	void draw2dContour(cv::Mat& image, cv::Scalar color) const;

	void computePose(const Pattern& pattern, const CameraCalibration& calibration);
};

#endif /* PATTERN_HPP */
