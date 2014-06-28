#include "MediaExtractor.h"
#include <media/stagefright/MediaDefs.h>
#include "Mp3Extractor.h"
sp<MediaExtractor> MediaExtractor::Create(const sp<DataSource>& source, const char* mime)
{
    MediaExtractor *ret = NULL;
    if (!strcasecmp(mime, MEDIA_MIMETYPE_AUDIO_MPEG))
    {
        ret = new Mp3Extractor(source);
    }//see more source in MediaExtractor.cpp(android src)
    return ret;
}


