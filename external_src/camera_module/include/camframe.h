#ifndef CAMFRAME_H
#define CAMFRAME_H
#include <stdint.h>
#include <time.h>
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
   time_t mTime_start;
   time_t mTime_end;

   uint32_t mWidth = 0;
   uint32_t mHeight = 0;
   uint32_t mBpp = 0;
   uint32_t mChannels = 0;

   int32_t mLength;
};

#endif // CAMFRAME_H
