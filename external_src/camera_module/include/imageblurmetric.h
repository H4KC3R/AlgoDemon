#ifndef IMAGEBLURMETRIC_H
#define IMAGEBLURMETRIC_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/xphoto/white_balance.hpp>
#include <opencv2/imgproc.hpp>

namespace ImageBlurMetric {

double getBlurLaplacian(const cv::Mat image);

double getBlurSobel(const cv::Mat image);

double getBlurScharr(const cv::Mat image);

double getBlurFFT(const cv::Mat image, int cutOffFreq = 60);

}

#endif // IMAGEBLURMETRIC_H
