#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H

#include "camenums.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/xphoto/white_balance.hpp>
#include <opencv2/imgproc.hpp>


namespace ImageProcess {

int getOpenCvType(BitMode bpp, int channels);

cv::Mat whiteBalanceImg(const cv::Mat& src);

cv::Mat debayerImg(const cv::Mat& src);

cv::Mat contrastImg(const cv::Mat& src, double coeff);

cv::Mat gammaContrastImg(const cv::Mat& src, double kGamma);

#endif // IMAGEPROCESS_H
