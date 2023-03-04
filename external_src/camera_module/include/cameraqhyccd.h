#ifndef CAMERAQHYCCD_H
#define CAMERAQHYCCD_H

#include "qhyccd.h"
#include "camstruct.h"
#include "camenums.h"

class CameraQHYCCD
{
private:
    qhyccd_handle *pCamhandle;
    static bool mIsSdkInited;
    CamParameters mParams;

public:
    CameraQHYCCD(char* id);

    ~CameraQHYCCD();

    static bool initSDK();

    static int32_t searchCamera();

    static bool getID(int32_t num, char* id);

    static std::string getModel(char* id);

    bool connect(StreamMode mode);

    bool getControlMinMaxStep(CameraControls control, double& min, double& max, double& step);

    bool setImageSize(uint32_t x, uint32_t y, uint32_t xsize, uint32_t ysize);

    bool getImageSize(uint32_t& startX, uint32_t& startY, uint32_t& sizeX, uint32_t& sizeY);

    bool setImageBitMode(BitMode bit);

    uint32_t getImageBitMode();

    bool setGain(double value);

    double getGain(void);

    bool setExposure(double ms);

    double getExposure(void);

    uint32_t getImgLength();

    CamParameters getCameraParameters();

    bool startSingleCapture();

    bool stopSingleCapture();

    bool startLiveCapture();

    bool stopLiveCapture();

    bool getImage(uint32_t& w, uint32_t& h, uint32_t& bpp, uint32_t& channels, uint8_t* imgdata);

    bool disconnect();

    static bool ReleaseSDK();

};

#endif // CAMERAQHYCCD_H
