#ifndef MEDIASCANNERCLIENT_H
#define MEDIASCANNERCLIENT_H
#include <utils/StringArray.h>
#include <utils/Errors.h>
#include "MetaData.h"
using namespace android;
class MediaScannerClient
{
public:
    MediaScannerClient();
    virtual ~MediaScannerClient();
    void beginFile(const char*file);
    void endFile(const char*file);

    virtual status_t postTag(sp<MetaData> &meta) = 0;
    virtual status_t postFile(const char* file) = 0;

};

#endif // MEDIASCANNERCLIENT_H
