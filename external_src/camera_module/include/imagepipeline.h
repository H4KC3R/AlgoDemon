#ifndef IMAGEPIPELINE_H
#define IMAGEPIPELINE_H
#include "camstruct.h"
#include <list>
#include <iterator>
#include <shared_mutex>

class ImagePipeline
{
public:
    ImagePipeline(uint32_t length);

    ~ImagePipeline();

    int getPipelineSize();

    const std::list <CamImage>::iterator getFirstFrame();

    const std::list <CamImage>::iterator nextFrame(const std::list <CamImage>::iterator& it);

    uint32_t getFrameCount() const;

    int32_t getCount() const;

private:
    int32_t count = 1;

    const size_t mSize = 2;
    CamImage firstFrame;
    CamImage secondFrame;

    std::list<CamImage> mList;
    std::shared_mutex mMutex;

};

#endif // IMAGEPIPELINE_H
