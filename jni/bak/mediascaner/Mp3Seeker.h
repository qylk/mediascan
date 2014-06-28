#ifndef MP3_SEEKER_H
#define MP3_SEEKER_H
#include <utils/RefBase.h>
#include <media/stagefright/DataSource.h>
using namespace android;
class Mp3Seeker:public RefBase
{
public:
    Mp3Seeker() {};
    virtual  bool getDuration(int32_t *durationMs)=0 ;
protected:
    virtual ~Mp3Seeker() {};
private:
};

#endif // MP3_SEEKER_H
