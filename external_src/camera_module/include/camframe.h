#ifndef CAMFRAME_H
#define CAMFRAME_H
#include <stdint.h>
#include <chrono>
#include <algorithm>
#include <cstring>
#include <QCoreApplication>

class CamFrame
{
public:
    CamFrame(uint32_t length);
    CamFrame(const CamFrame& frame);
    ~CamFrame();

public:
   uint8_t* pData;
   std::chrono::time_point<std::chrono::steady_clock> mTime;

   uint32_t mWidth = 0;
   uint32_t mHeight = 0;
   uint32_t mBpp = 0;
   uint32_t mChannels = 0;

   int32_t mLength;
};

#endif // CAMFRAME_H
