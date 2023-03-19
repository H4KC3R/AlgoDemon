#ifndef CAMFRAME_H
#define CAMFRAME_H
#include <stdint.h>
#include <chrono>
#include <algorithm>
#include <cstring>

class CamFrame
{
public:
    CamFrame();
    CamFrame(const CamFrame& frame);
    void allocateFrame(uint32_t length);
    ~CamFrame();

public:
   uint8_t* pData = nullptr;
   std::chrono::time_point<std::chrono::steady_clock> mTime;

   uint32_t mWidth = 0;
   uint32_t mHeight = 0;
   uint32_t mBpp = 0;
   uint32_t mChannels = 0;

   int32_t mLength;
};

#endif // CAMFRAME_H
