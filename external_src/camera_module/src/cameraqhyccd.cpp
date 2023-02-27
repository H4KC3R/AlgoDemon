#include "cameraqhyccd.h"
#include <iostream>
#include <exception>

bool CameraQHYCCD::mIsSdkInited = false;

CameraQHYCCD::CameraQHYCCD(char* id) {
    if(id == NULL)
        throw std::logic_error("Id address is null");
    pCamhandle = OpenQHYCCD(id);
    if(pCamhandle == NULL)
        throw std::logic_error("Failed to open camera!");
    uint32_t ret = GetQHYCCDModel(id, params.mModel);
    if(ret != QHYCCD_SUCCESS)
        throw std::logic_error("Failed to read model!");
}

bool CameraQHYCCD::initSDK() {
    if(mIsSdkInited)
        return false;

    int ret = InitQHYCCDResource();
    if(ret == QHYCCD_SUCCESS)
        mIsSdkInited = true;

    return mIsSdkInited;
}

int32_t CameraQHYCCD::searchCamera(){
    int32_t num = 0;
    num = ScanQHYCCD();
    return num;
}

bool CameraQHYCCD::getID(int32_t num, char* id){
    uint32_t ret = GetQHYCCDId(num, id);
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::connect(StreamMode mode) {
    uint32_t ret = SetQHYCCDStreamMode(pCamhandle, mode);
    if(ret != QHYCCD_SUCCESS)
        return false;

    ret = InitQHYCCD(pCamhandle);
    if(ret != QHYCCD_SUCCESS)
        return false;

    ret = IsQHYCCDControlAvailable(pCamhandle, CAM_COLOR);
    if(ret == BAYER_GB || ret == BAYER_GR || ret == BAYER_BG ||ret == BAYER_RG)
        params.mIsMono = false;
    else
        params.mIsMono = true;

    uint32_t imagew, imageh, bpp;
    double chipw, chiph, pixelw, pixelh;

    ret = GetQHYCCDChipInfo(pCamhandle, &chipw, &chiph, &imagew, &imageh, &pixelw, &pixelh, &bpp);
    if(ret == QHYCCD_SUCCESS){
        params.mChipw = chipw;
        params.mChiph = chiph;
        params.mPixelw = pixelw;
        params.mPixelh = pixelh;
        params.mMaximgw = imagew;
        params.mMaximgh = imageh;
    }
    else
        return false;

    params.mWbin = 1;
    params.mHbin = 1;
    ret = SetQHYCCDBinMode(pCamhandle, params.mWbin, params.mHbin);
    if(ret != QHYCCD_SUCCESS)
        return false;

    params.mIsLiveMode = (mode == live);
    params.mIsConnected = true;

    if(!params.mIsMono)
        SetQHYCCDDebayerOnOff(pCamhandle,false);

    return true;
}

bool CameraQHYCCD::getControlMinMaxStep(CameraControls control, double& min, double& max, double& step) {
    uint32_t ret = IsQHYCCDControlAvailable(pCamhandle, (CONTROL_ID)control);
    if(ret != QHYCCD_SUCCESS)
        return false;
    ret = GetQHYCCDParamMinMaxStep(pCamhandle, (CONTROL_ID)control, &min, &max, &step);
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::setImageSize(uint32_t x, uint32_t y, uint32_t xsize, uint32_t ysize) {
    uint32_t ret = SetQHYCCDResolution(pCamhandle, x, y, xsize/params.mWbin, ysize/params.mHbin);
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::getImageSize(uint32_t& startX, uint32_t& startY, uint32_t& sizeX, uint32_t& sizeY) {
    uint32_t ret = GetQHYCCDEffectiveArea(pCamhandle, &startX, &startY, &sizeX, &sizeY);
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::setImageBitMode(BitMode bit){
    if(bit == bit8) {
        uint32_t ret = IsQHYCCDControlAvailable(pCamhandle, CAM_8BITS);
        if(ret == QHYCCD_SUCCESS)
            ret = SetQHYCCDBitsMode(pCamhandle, 8);
        return (ret == QHYCCD_SUCCESS);
    }
    else {
        uint32_t ret = IsQHYCCDControlAvailable(pCamhandle, CAM_16BITS);
        if(ret == QHYCCD_SUCCESS)
            ret = SetQHYCCDBitsMode(pCamhandle, 16);
        return (ret == QHYCCD_SUCCESS);
    }
}

uint32_t CameraQHYCCD::getImageBitMode(){
    return ((uint32_t)GetQHYCCDParam(pCamhandle, CONTROL_TRANSFERBIT));
}

bool CameraQHYCCD::setGain(double value) {
    uint32_t ret = SetQHYCCDParam(pCamhandle, CONTROL_GAIN, value);
    return (ret == QHYCCD_SUCCESS);
}

double CameraQHYCCD::getGain(void) {
    return GetQHYCCDParam(pCamhandle, CONTROL_GAIN);
}

bool CameraQHYCCD::setExposure(double ms) {
    uint32_t ret = SetQHYCCDParam(pCamhandle, CONTROL_EXPOSURE, ms * 1000.0);
    return (ret == QHYCCD_SUCCESS);
}

double CameraQHYCCD::getExposure(void) {
    // return in ms
    return (GetQHYCCDParam(pCamhandle, CONTROL_EXPOSURE) / 1000);
}

uint32_t CameraQHYCCD::getImgLength() {
    return GetQHYCCDMemLength(pCamhandle);
}

bool CameraQHYCCD::startSingleCapture() {
    if(params.mIsLiveMode)
        return false;

    params.mStatus = singleCapture;
    uint32_t ret = ExpQHYCCDSingleFrame(pCamhandle);
    if(ret == QHYCCD_SUCCESS)
        return true;
    else {
        params.mStatus = failed;
        return false;
    }
}

bool CameraQHYCCD::stopSingleCapture() {
    uint32_t ret = CancelQHYCCDExposing(pCamhandle);
    if(ret == QHYCCD_SUCCESS) {
        params.mStatus = idle;
        return true;
    }
    else {
        params.mStatus = failed;
        return false;
    }
}

bool CameraQHYCCD::startLiveCapture() {
    uint32_t ret = BeginQHYCCDLive(pCamhandle);
    if(ret == QHYCCD_SUCCESS) {
        params.mStatus = liveCapture;
        return true;
    }
    else {
        params.mStatus = failed;
        return false;
    }
}

bool CameraQHYCCD::stopLiveCapture() {
    uint32_t ret = StopQHYCCDLive(pCamhandle);
    if(ret == QHYCCD_SUCCESS) {
        params.mStatus = idle;
        return true;
    }
    else {
        params.mStatus = failed;
        return false;
    }
}

bool CameraQHYCCD::getImage(uint32_t& w, uint32_t& h, uint32_t& bpp, uint32_t& channels, uint8_t* imgdata) {
    uint32_t ret = 0;
    if(params.mStatus == singleCapture) {
        ret = GetQHYCCDSingleFrame(pCamhandle, &w, &h, &bpp, &channels, imgdata);
        params.mStatus = idle;
    }
    else if(params.mStatus == liveCapture)
        ret = GetQHYCCDLiveFrame(pCamhandle, &w, &h, &bpp, &channels, imgdata);
    else
        return false;

    return (ret == QHYCCD_SUCCESS);
}

CameraQHYCCD::~CameraQHYCCD() {
    if(params.mIsConnected)
        disconnect();
}

bool CameraQHYCCD::disconnect() {
    if(params.mStatus == liveCapture)
        stopLiveCapture();
    else if(params.mStatus == singleCapture)
        stopSingleCapture();
    uint32_t ret = CloseQHYCCD(pCamhandle);
    params.mIsConnected = false;
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::ReleaseSDK() {
    uint32_t ret = ReleaseQHYCCDResource();
    mIsSdkInited = false;
    return (ret == QHYCCD_SUCCESS);
}



