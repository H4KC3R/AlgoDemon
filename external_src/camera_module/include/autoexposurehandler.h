#ifndef AUTOEXPOSUREHANDLER_H
#define AUTOEXPOSUREHANDLER_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/xphoto/white_balance.hpp>
#include <opencv2/imgproc.hpp>

struct AutoExposureParams{
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

    double mMaxExposure, mMinExposure;
    double mMaxGain, mMinGain;

    AutoExposureParams mParams;

public:

    AutoExposureHandler(AutoExposureParams params, double maxExposure, double minExposure,
                        double maxGain, double minGain);

    bool correct(cv::Mat& image, double currExposure, double currGain);

    // ************************** Getters ************************** //
    double getMaxExposure()             { return mMaxExposure; }
    double getMinExposure()             { return mMinExposure; }
    double getMaxGain()                 { return mMaxGain; }
    double getMinGain()                 { return mMinGain; }

    int getMaxFrameCoeff()              { return mMaxFrameCoeff; }
    int getDivideCoeff()                { return mDivideCoeff; }
    int getDivideCoeffMax()             { return mDivideCoeffMax; }
    int getDivideCoeffDefault()         { return mDivideCoeffDefault; }

    double getExposure()                { return mExposureToSet; }
    double getGain()                    { return mGainToSet; }

    AutoExposureParams getParams()      { return mParams; }

    // ************************** Setters ************************** //
    void setMaxExposure(double maxExposure)                 { mMaxExposure = maxExposure; }
    void setMinExposure(double minExposure)                 { mMinExposure = minExposure; }
    void setMaxGain(double maxGain)                         { mMaxGain = maxGain; }
    void setMinGain(double minGain)                         { mMinGain = minGain; }

    void setMaxFrameCoeff(int maxFrameCoeff)                { mMaxFrameCoeff = maxFrameCoeff; }
    void setDivideCoeff(int divideCoeff)                    { mDivideCoeff = divideCoeff; }
    void setDivideCoeffMax(int divideCoeffMax)              { mDivideCoeffMax = divideCoeffMax; }
    void setDivideCoeffDefault(int divideCoeffDefault)      { mDivideCoeffDefault = divideCoeffDefault; }

    void setParams(AutoExposureParams params)               { mParams = params; }

};

#endif // AUTOEXPOSUREHANDLER_H
