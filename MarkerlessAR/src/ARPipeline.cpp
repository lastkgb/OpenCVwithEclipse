#include "ARPipeline.hpp"

ARPipeline::ARPipeline(const cv::Mat& patternImage, const CameraCalibration& calibration) : m_calibration(calibration)
{
	m_patternDetector.buildPatternFromImage(patternImage, m_pattern);
	m_patternDetector.train(m_pattern);
}

bool ARPipeline::processFrame(const cv::Mat& inputFrame)
{
	bool patternFound = m_patternDetector.findPattern(inputFrame, m_patternInfo);
	if (patternFound)
		m_patternInfo.computePose(m_pattern, m_calibration);

	return patternFound;
}

const Transformation& ARPipeline::getPatternLocation() const
{
	return m_patternInfo.pose3d;
}