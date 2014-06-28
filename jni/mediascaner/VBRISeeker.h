#ifndef VBRISEEKER_H
#define VBRISEEKER_H
#include "Mp3Seeker.h"
#include <media/stagefright/DataSource.h>
#include <utils/RefBase.h>

class VBRISeeker  : public Mp3Seeker
{
public:
    static sp<VBRISeeker> CreateFromSource(const sp<DataSource> &source, off64_t post_id3_pos);
    virtual bool getDuration(int32_t *durationMs) ;
protected:
    virtual ~VBRISeeker();
private:
    VBRISeeker();
    int32_t mDurationMs;
};

#endif // VBRISEEKER_H
