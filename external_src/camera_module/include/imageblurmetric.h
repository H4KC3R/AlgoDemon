#ifndef IMAGEBLURMETRIC_H
#define IMAGEBLURMETRIC_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/xphoto/white_balance.hpp>
#include <opencv2/imgproc.hpp>

namespace ImageBlurMetric {

bool getBlurLaplacian(const cv::Mat image, double& result);

bool getBlurSobel(const cv::Mat image, double& result);

bool getBlurScharr(const cv::Mat image, double& result);

bool getBlurFFT(const cv::Mat image, double& result, int cutOffFreq = 60);

}

#endif // IMAGEBLURMETRIC_H
