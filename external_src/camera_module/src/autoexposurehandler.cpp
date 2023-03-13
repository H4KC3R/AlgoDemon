#include "autoexposurehandler.h"


AutoExposureHandler::AutoExposureHandler(double maxExposure, double minExposure, double maxGain,
                    double minGain, AutoExposureParams params) : mMaxExposure(maxExposure),
    mMinExposure(minExposure), mMaxGain(maxGain), mMinGain(minGain), mParams(params)
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

        if(mExposureToSet > mMinExposure && currGain <= mMaxGain)
            mGainToSet = currGain + gainStep;
        else if(mExposureToSet < mMinExposure)
            mGainToSet = mMinGain;

        if(mExposureToSet > mMaxExposure)
            mExposureToSet = mMaxExposure;

        if(mGainToSet > mMaxGain)
            mGainToSet = mMaxGain;

        return true;
      }
      else {
          mProcessCounter = 0;
          return false;
      }
}
