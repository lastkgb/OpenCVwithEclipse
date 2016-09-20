/*
 * ARPipeline.hpp
 *
 *  Created on: 2016. 9. 19.
 *      Author: lastkgb
 */

#ifndef ARPIPELINE_HPP
#define ARPIPELINE_HPP

#include "PatternDetector.hpp"
#include "CameraCalibration.hpp"
#include "GeometryTypes.hpp"

class ARPipeline
{
public:
	ARPipeline(const cv::Mat& patternImage, const CameraCalibration& calibration);

	bool precessFrame(const cv::Mat& inputFrame);
	const Transformation& getPatternLocation() const;
	PatternDetector m_patternDetector;

private:
	CameraCalibration m_calibration;
	Pattern m_pattern;
	PatternTrackingInfo m_patternInfo;
};

#endif
