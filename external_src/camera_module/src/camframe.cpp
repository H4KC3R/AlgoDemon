#include "camframe.h"

CamFrame::CamFrame() {
    mTime_start = time(NULL);
    mWidth = 0;
    mHeight = 0;
    mBpp = 0;
    mChannels = 0;
    mLength = 0;
}

CamFrame::CamFrame(const CamFrame &frame) {
    if(pData != nullptr)
        delete[] pData;

    pData = new uint8_t[frame.mLength];

    std::memcpy(pData, frame.pData, frame.mLength);

    mTime_start = frame.mTime_start;
    mTime_end = frame.mTime_end;
    mWidth = frame.mWidth;;
    mHeight = frame.mHeight;;
    mBpp = frame.mBpp;;
    mChannels = frame.mChannels;;
    mLength = frame.mLength;;
}

void CamFrame::allocateFrame(uint32_t length) {
    if(pData != nullptr)
        delete[] pData;

    pData = new uint8_t[length];
    mLength = length;;
}

CamFrame::~CamFrame() {
    if(pData != nullptr)
        delete[] pData;
}


