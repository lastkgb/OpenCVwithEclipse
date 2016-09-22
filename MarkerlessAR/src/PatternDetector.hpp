/*
 * PatternDetector.hpp
 *
 *  Created on: 2016. 9. 20.
 *      Author: lastkgb
 */

#ifndef PATTERNDETECTOR_HPP
#define PATTERNDETECTOR_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/features2d.hpp>

#include "Pattern.hpp"

class PatternDetector
{
public:
	PatternDetector(cv::Ptr<cv::FeatureDetector> detector = new cv::ORB(1000),
			cv::Ptr<cv::DescriptorExtractor> extractor = new cv::FREAK(false,false),
			cv::Ptr<cv::DescriptorMatcher> matcher = new cv::BFMatcher(cv::NORM_HAMMING, true),
			bool enableRatioTest = false);

	void train(const Pattern& pattern);

	void buildPatternFromImage(const cv::Mat& image, Pattern& pattern) const;
	bool findPattern(const cv::Mat& image, PatternTrackingInfo& info);

	bool enableRatioTest;
	bool enableHomographyRefinement;
	float homographyReprojectionThreshold;

protected:
	bool extractFeatures(const cv::Mat& image, std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors) const;
	void getMatches(const cv::Mat& queryDescriptors, std::vector<cv::DMatch>& matches);

	static void getGray(const cv::Mat& image, cv::Mat& gray);

	static bool refineMatchesWithHomography(
		const std::vector<cv::KeyPoint>& queryKeypoints,
		const std::vector<cv::KeyPoint>& trainKeypoints,
		float reprojectionThreshold,
		std::vector<cv::DMatch>& matches,
		cv::Mat& homography);

private:
	std::vector<cv::KeyPoint> m_queryKeypoints;
	cv::Mat m_queryDescriptors;
	std::vector<cv::DMatch> m_matches;
	std::vector< std::vector<cv::DMatch> > m_knnMatches;

	cv::Mat m_grayImg;
	cv::Mat m_warpedImg;
	cv::Mat m_roughHomography;
	cv::Mat m_refinedHomography;

	Pattern m_pattern;
	cv::Ptr<cv::FeatureDetector> m_detector;
	cv::Ptr<cv::DescriptorExtractor> m_extractor;
	cv::Ptr<cv::DescriptorMatcher> m_matcher;
};

#endif /* PATTERNDETECTOR_HPP */
