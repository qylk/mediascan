#ifndef MEDIA_EXTRACTOR_H
#define MEDIA_EXTRACTOR_H

#include <utils/RefBase.h>
#include <media/stagefright/DataSource.h>
#include "MetaData.h"

using namespace android;
class MediaExtractor : public RefBase
{
public:
    static sp<MediaExtractor> Create(const sp<DataSource> &source, const char *mime = NULL);
    //virtual sp<MetaData> getTrackMetaData(size_t index, uint32_t flags = 0) = 0;
    virtual sp<MetaData> getMetaData()=0;
    virtual bool hasInited() const=0;

protected:
    MediaExtractor() {};
    virtual ~MediaExtractor() {};
};

#endif // MEDIA_EXTRACTOR_H
