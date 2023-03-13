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

bool whiteBalanceImg(cv::Mat& src, cv::Mat& dst);

bool debayerImg(cv::Mat& src, cv::Mat& dst);

bool contrastImg(cv::Mat& src, cv::Mat& dst, double coeff);

bool gammaContrastImg(cv::Mat& src, cv::Mat& dst, double kGamma);

}

#endif // IMAGEPROCESS_H
