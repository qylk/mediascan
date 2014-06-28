#ifndef XINGSEEKER_H
#define XINGSEEKER_H
#include "Mp3Seeker.h"

class XINGSeeker :public Mp3Seeker
{
public:
    static sp<XINGSeeker> CreateFromSource(const sp<DataSource> &source, off64_t first_frame_pos);
    virtual bool getDuration(int32_t *durationMs) ;

protected:
    virtual ~XINGSeeker();
private:
    XINGSeeker();
    int32_t mDurationMs;
};

#endif // XINGSEEKER_H
