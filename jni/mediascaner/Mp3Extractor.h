#ifndef MP3EXTRACTOR_H
#define MP3EXTRACTOR_H
#include <media/stagefright/DataSource.h>
#include "MediaExtractor.h"
//#include "iconv.h"
class UConverter;
using namespace android;
class Mp3Extractor : public MediaExtractor
{
public:
    Mp3Extractor(const sp<DataSource> &source);
    virtual sp<MetaData> getMetaData();
    virtual bool hasInited() const;
protected:
    virtual ~Mp3Extractor() {};
private:
    sp<DataSource> mDataSource;
    sp<MetaData> mMeta;
    uint32_t mFixedHeader;
    status_t mInitCheck;
    off64_t mFirstFramePos;
    //iconv_t cd;
};

#endif // MP3EXTRACTOR_H
