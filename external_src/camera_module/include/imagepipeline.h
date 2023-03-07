#ifndef IMAGEPIPELINE_H
#define IMAGEPIPELINE_H
#include "camstruct.h"
#include <list>
#include <iterator>
#include <shared_mutex>

class ImagePipeline
{
public:
    ImagePipeline();

    ~ImagePipeline();

    void clearPipeline();

    void setFrame(CamImage* frame);

    const std::list <CamImage>::const_iterator& getFirstFrame();

    const std::list <CamImage>::const_iterator& nextFrame(const std::list <CamImage>::const_iterator& it);

private:
    int32_t count = 1;
    const size_t mSize = 2;

    std::list<CamImage> mList;
    std::shared_mutex mMutex;

};

#endif // IMAGEPIPELINE_H
