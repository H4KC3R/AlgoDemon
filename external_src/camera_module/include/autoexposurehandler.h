#ifndef AUTOEXPOSUREHANDLER_H
#define AUTOEXPOSUREHANDLER_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/xphoto/white_balance.hpp>
#include <opencv2/imgproc.hpp>

struct AutoExposureParams{
    double maxExposure;
    double minExposure;
    double maxGain;
    double minGain;

    double maxPercent = 0;
    double minRelCoef;
    double maxRelCoeff;
    double mean;
};

class AutoExposureHandler
{
private:
    int mProcessCounter = 0;
    int mMaxFrameCoeff = 20;
    int mDivideCoeff = 2;
    int mDivideCoeffMax = 24;
    int mDivideCoeffDefault = 2;

    double mExposureToSet;
    double mGainToSet;

    AutoExposureParams mParams;

public:

    AutoExposureHandler(AutoExposureParams params);

    bool correct(cv::Mat& image, double currExposure, double currGain);

    // ************************** Getters ************************** //
    double getMaxExposure()             { return mParams.maxExposure; }
    double getMinExposure()             { return mParams.minExposure; }
    double getMaxGain()                 { return mParams.maxGain; }
    double getMinGain()                 { return mParams.minGain; }

    int getMaxFrameCoeff()              { return mMaxFrameCoeff; }
    int getDivideCoeff()                { return mDivideCoeff; }
    int getDivideCoeffMax()             { return mDivideCoeffMax; }
    int getDivideCoeffDefault()         { return mDivideCoeffDefault; }

    double getExposure()                { return mExposureToSet; }
    double getGain()                    { return mGainToSet; }

    AutoExposureParams getParams()      { return mParams; }

    // ************************** Setters ************************** //
    void setMaxExposure(double maxExposure)                 { mParams.maxExposure = maxExposure; }
    void setMinExposure(double minExposure)                 { mParams.minExposure = minExposure; }
    void setMaxGain(double maxGain)                         { mParams.maxGain = maxGain; }
    void setMinGain(double minGain)                         { mParams.minGain = minGain; }

    void setMaxFrameCoeff(int maxFrameCoeff)                { mMaxFrameCoeff = maxFrameCoeff; }
    void setDivideCoeff(int divideCoeff)                    { mDivideCoeff = divideCoeff; }
    void setDivideCoeffMax(int divideCoeffMax)              { mDivideCoeffMax = divideCoeffMax; }
    void setDivideCoeffDefault(int divideCoeffDefault)      { mDivideCoeffDefault = divideCoeffDefault; }

    void setParams(AutoExposureParams params)               { mParams = params; }

};

#endif // AUTOEXPOSUREHANDLER_H
