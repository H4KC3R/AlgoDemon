#include "imageprocess.h"

int ImageProcess::getOpenCvType(BitMode bpp, int channels) {
    if(bpp == bit8)
        return CV_MAKE_TYPE(CV_8U, channels);
    else
        return CV_MAKE_TYPE(CV_16U, channels);

}

CamImage ImageProcess::whiteBalanceImg(const CamImage& src) {
    CamImage result;
    result = src;
    cv::Ptr<cv::xphoto::WhiteBalancer> wb = cv::xphoto::createSimpleWB();
    wb->balanceWhite(src.img, result.img);
    return result;
}

CamImage ImageProcess::debayerImg(const CamImage& src) {
    CamImage result;
    result = src;
    cv::cvtColor(src.img, result.img, cv::COLOR_BayerRG2BGR);
    return result;
}

CamImage ImageProcess::contrastImg(const CamImage& src, double coeff) {
    if(coeff < 0)
        throw std::logic_error("The contrast coefficient should not be negative!");
    CamImage result;
    result = src;
    src.img.convertTo(result.img, -1, coeff);
    return result;
}

CamImage ImageProcess::gammaContrastImg(const CamImage& src, double kGamma) {
    CamImage result;
    result = src;

    cv::Mat lookUpTable(1, 256, CV_8U);
    uchar* p = lookUpTable.ptr();
    for(int i = 0; i < 256; ++i)
        p[i] = cv::saturate_cast<uchar>(pow(i / 255.0, kGamma) * 255.0);

    cv::LUT(src.img, lookUpTable, result.img);
    return result;
}
