#include "imageblurmetric.h"

bool ImageBlurMetric::getBlurLaplacian(const cv::Mat image, double& result) {
    if(image.dims != 2)
        return false;

    cv::Mat output, outputAbs;
    cv::Scalar mean, stddev; //0:1st channel, 1:2nd channel and 2:3rd channel

    cv::Laplacian(image, output, CV_64F, 3, 1, 0, cv::BORDER_DEFAULT);
    // изначально выходное изображение было в формате 64 float,
    // чтобы избежать переполнение
    cv::convertScaleAbs(output, outputAbs);
    cv::meanStdDev(outputAbs, mean, stddev, cv::Mat());

    double variance = stddev.val[0] * stddev.val[0];
    result = variance;
    return true;
}

bool ImageBlurMetric::getBlurSobel(const cv::Mat image, double& result) {
    if(image.dims != 2)
        return false;

    cv::Mat outputX, outputY;

    cv::Sobel(image, outputX, CV_32FC1, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    cv::Sobel(image, outputY, CV_32FC1, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT);

    cv::Mat1f magn;

    cv::magnitude(outputX, outputY, magn);
    cv::Scalar mean = cv::mean(magn);

    result = mean.val[0];
    return true;
}

bool ImageBlurMetric::getBlurScharr(const cv::Mat image, double& result) {
    if(image.dims != 2)
        return false;

    cv::Mat outputX, outputY;

    cv::Scharr(image, outputX, CV_32FC1, 1, 0, 1, 0, cv::BORDER_DEFAULT);
    cv::Scharr(image, outputY, CV_32FC1, 0, 1, 1, 0, cv::BORDER_DEFAULT);

    cv::Mat1f magn;

    cv::magnitude(outputX, outputY, magn);
    cv::Scalar mean = cv::mean(magn);

    result = mean.val[0];
    return true;
}

bool ImageBlurMetric::getBlurFFT(const cv::Mat image, double& result, int cutOffFreq) {
    if(image.dims != 2)
        return false;

    int cx = image.cols/2;
    int cy = image.rows/2;

    cv::Mat fImage;
    image.convertTo(fImage, CV_32F);

    // FFT
    cv::Mat fourierTransform;
    cv::dft(fImage, fourierTransform, cv::DFT_SCALE|cv::DFT_COMPLEX_OUTPUT);

    //помещаем низкие частоты в центр
    //путем перетасовки квадрантов.
    cv::Mat q0(fourierTransform, cv::Rect(0, 0, cx, cy));       // Верхний левый
    cv::Mat q1(fourierTransform, cv::Rect(cx, 0, cx, cy));      // Верхний правый
    cv::Mat q2(fourierTransform, cv::Rect(0, cy, cx, cy));      // Нижний левый
    cv::Mat q3(fourierTransform, cv::Rect(cx, cy, cx, cy));     // Нижний правый

    // Верхний левый <---> Нижний правый
    cv::Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    // Верхний правый <---> Нижний левый
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);

    // Занулить низкие частоты
    fourierTransform(cv::Rect(cx-cutOffFreq, cy-cutOffFreq, 2*cutOffFreq, 2*cutOffFreq)).setTo(0);

    // Ставим все на место
    cv::Mat orgFFT;
    fourierTransform.copyTo(orgFFT);
    cv::Mat p0(orgFFT, cv::Rect(0, 0, cx, cy));       // Верхний левый
    cv::Mat p1(orgFFT, cv::Rect(cx, 0, cx, cy));      // Верхний правый
    cv::Mat p2(orgFFT, cv::Rect(0, cy, cx, cy));      // Нижний левый
    cv::Mat p3(orgFFT, cv::Rect(cx, cy, cx, cy));     // Нижний правый

    p0.copyTo(tmp);
    p3.copyTo(p0);
    tmp.copyTo(p3);

    p1.copyTo(tmp);
    p2.copyTo(p1);
    tmp.copyTo(p2);

    cv::Mat invFFT;
    cv::Mat logFFT;
    double minVal,maxVal;

    cv::dft(orgFFT, invFFT, cv::DFT_INVERSE|cv::DFT_REAL_OUTPUT);

    invFFT = cv::abs(invFFT);
    cv::minMaxLoc(invFFT,&minVal,&maxVal,NULL,NULL);

    // Проверяем что если все хорошо или плохо
    if(maxVal<=0.0)
        return -1;

    cv::log(invFFT,logFFT);
    logFFT *= 20;

    cv::Scalar processedMean = cv::mean(logFFT);
    result = processedMean.val[0];
    return true;
}
