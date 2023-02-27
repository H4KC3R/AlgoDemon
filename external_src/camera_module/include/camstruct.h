#include <stdint.h>
#include "camenums.h"

#ifndef CAMSTRUCT_H
#define CAMSTRUCT_H

struct CamParameters
{
    char mModel[12];         // camera model
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

#endif // CAMSTRUCT_H
