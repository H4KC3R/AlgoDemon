#include <stdint.h>
#include <chrono>
#include "camenums.h"

#ifndef CAMSTRUCT_H
#define CAMSTRUCT_H

struct CamParameters
{
    char mModel[20];         // camera model
    bool mIsMono;            // mono or color
    bool mIsLiveMode;        // camera mode
    bool mIsConnected = false;

    CameraStatus mStatus = idle;

    double mChipw;           // chip size width (in mm)
    double mChiph;           // chip size height (in mm)
    double mPixelw;          // chip pixel size width (in um)
    double mPixelh;          // chip pixel size height (in um)
    uint32_t mMaximgw;       // maximum chip output image width
    uint32_t mMaximgh;       // maximum chip output image height
    uint32_t mBpp;           // chip pixel depth

    uint8_t mWbin;
    uint8_t mHbin;
};

struct CamImage{
    uint8_t* data;

    std::time_t time;

    uint32_t w = 0;
    uint32_t h = 0;
    uint32_t bpp = 0;
    uint32_t channels = 0;

    int32_t length;

};


#endif // CAMSTRUCT_H
