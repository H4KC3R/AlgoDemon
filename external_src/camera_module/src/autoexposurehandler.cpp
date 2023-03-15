#include "autoexposurehandler.h"


AutoExposureHandler::AutoExposureHandler(AutoExposureParams params) : mParams(params)
{

}


bool AutoExposureHandler::correct(cv::Mat& image, double currExposure, double currGain) {
    if(image.type() != CV_8UC1)
        return false;

    int bins = 256;
    int histSize[] = {bins};
    // Set ranges for histogram bins
    float lranges[] = {0, 256};
    const float* ranges[] = {lranges};
    // create matrix for histogram
    cv::Mat hist;
    int channels[] = {0}; // hsv

    cv::calcHist( &image, 1, channels, cv::Mat(), // do not use mask
                hist, 1, histSize, ranges);
    cv::normalize(hist, hist, 1, 0, cv::NORM_L1);

    double percent = hist.at <float> (255.0);

    double mean = cv::mean(image)[0];
    double relMean = mean / mParams.mean;
    double rel = percent / mParams.maxPercent;

    if (relMean > 1 && rel > 1)
      rel = relMean > rel ? relMean : rel;
    else
        rel = relMean;

    if (!(rel <= mParams.maxRelCoeff && rel >= mParams.minRelCoef)) {
        ++mProcessCounter;
        if (mProcessCounter > mMaxFrameCoeff && mDivideCoeff < mDivideCoeffMax) {
            mDivideCoeff += 2;
            mProcessCounter = 0;
        }
        if (mProcessCounter > mMaxFrameCoeff * 3 && mDivideCoeff == mDivideCoeffMax)
            mDivideCoeff = mDivideCoeffDefault;

        double neededExposure = currExposure / rel;
        double exposureStep = neededExposure - currExposure;
        double gainStep = (exposureStep / mDivideCoeff) * 15;
        gainStep = gainStep < 1 ? 1 : gainStep;

        mExposureToSet = currExposure + exposureStep / mDivideCoeff;

        if(mExposureToSet > mParams.minExposure && currGain <= mParams.maxGain)
            mGainToSet = currGain + gainStep;
        else if(mExposureToSet < mParams.minExposure)
            mGainToSet = mParams.minGain;

        if(mExposureToSet > mParams.maxExposure)
            mExposureToSet = mParams.maxExposure;

        if(mGainToSet > mParams.maxGain)
            mGainToSet = mParams.maxGain;

        return true;
      }
      else {
          mProcessCounter = 0;
          return false;
      }
}
