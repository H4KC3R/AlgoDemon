#include "imageprocess.h"

int ImageProcess::getOpenCvType(BitMode bpp, int channels) {
    if(bpp == bit8)
        return CV_MAKE_TYPE(CV_8U, channels);
    else
        return CV_MAKE_TYPE(CV_16U, channels);
}

bool ImageProcess::whiteBalanceImg(cv::Mat& src, cv::Mat& dst) {
    cv::Ptr<cv::xphoto::WhiteBalancer> wb = cv::xphoto::createSimpleWB();
    wb->balanceWhite(src, dst);
    return true;
}

bool ImageProcess::debayerImg(cv::Mat& src, cv::Mat& dst) {
    cv::cvtColor(src, dst, cv::COLOR_BayerRG2BGR);
    return true;
}

bool ImageProcess::contrastImg(cv::Mat& src, cv::Mat& dst, double coeff) {
    if(coeff < 0)
        return false;
    src.convertTo(dst, -1, coeff);
    return true;
}

bool ImageProcess::gammaContrastImg(cv::Mat& src, cv::Mat& dst, double kGamma) {
    cv::Mat lookUpTable(1, 256, CV_8U);
    uchar* p = lookUpTable.ptr();

    for(int i = 0; i < 256; ++i)
        p[i] = cv::saturate_cast<uchar>(pow(i / 255.0, kGamma) * 255.0);

    cv::LUT(src, lookUpTable, dst);
    return true;
}
