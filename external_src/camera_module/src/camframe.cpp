#include "camframe.h"

CamFrame::CamFrame(uint32_t length) {
    pData = new uint8_t[length];

    qDebug() << "created" << pData;


    mTime = std::chrono::steady_clock::now();
    mWidth = 0;
    mHeight = 0;
    mBpp = 0;
    mChannels = 0;
    mLength = length;
}

CamFrame::CamFrame(const CamFrame &frame) {
    pData = new uint8_t[frame.mLength];

    qDebug() << "created" << pData;

    std::memcpy(pData, frame.pData, frame.mLength);

    mTime = frame.mTime;
    mWidth = frame.mWidth;;
    mHeight = frame.mHeight;;
    mBpp = frame.mBpp;;
    mChannels = frame.mChannels;;
    mLength = frame.mLength;;
}

CamFrame::~CamFrame() {
    qDebug() << "deleted" << pData;
    delete[] pData;
}


