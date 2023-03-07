#include "imageprocess.h"

int ImageProcess::getOpenCvType(BitMode bpp, int channels) {
    if(bpp == bit8)
        return CV_MAKE_TYPE(CV_8U, channels);
    else
        return CV_MAKE_TYPE(CV_16U, channels);
}

cv::Mat ImageProcess::whiteBalanceImg(const cv::Mat& src) {
    cv::Mat result;
    cv::Ptr<cv::xphoto::WhiteBalancer> wb = cv::xphoto::createSimpleWB();
    wb->balanceWhite(src, result);
    return result;
}

cv::Mat ImageProcess::debayerImg(const cv::Mat& src) {
    cv::Mat result;
    result = src;
    cv::cvtColor(src, result, cv::COLOR_BayerRG2BGR);
    return result;
}

cv::Mat ImageProcess::contrastImg(const cv::Mat& src, double coeff) {
    if(coeff < 0)
        throw std::logic_error("The contrast coefficient should not be negative!");
    cv::Mat result;
    result = src;
    src.convertTo(result, -1, coeff);
    return result;
}

cv::Mat ImageProcess::gammaContrastImg(const cv::Mat& src, double kGamma) {
    cv::Mat result;
    result = src;

    cv::Mat lookUpTable(1, 256, CV_8U);
    uchar* p = lookUpTable.ptr();
    for(int i = 0; i < 256; ++i)
        p[i] = cv::saturate_cast<uchar>(pow(i / 255.0, kGamma) * 255.0);

    cv::LUT(src, lookUpTable, result);
    return result;
}
