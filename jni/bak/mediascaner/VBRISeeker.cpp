#include "VBRISeeker.h"
#include "Utils.h"
#include "MpegUtils.h"
VBRISeeker::VBRISeeker()
{
    //ctor
}

VBRISeeker::~VBRISeeker()
{
    //dtor
}

sp<VBRISeeker> VBRISeeker::CreateFromSource(const sp<DataSource>& source, off64_t first_frame_pos)
{
    off64_t pos = first_frame_pos;
    uint8_t header[4];
    if (source->readAt(pos, header, sizeof(header)) < (ssize_t)sizeof(header))
    {
        return NULL;
    }

    uint32_t headerInt = U32_AT(&header[0]);
    int sampleRate;
    if (!GetMPEGAudioFrameSize(headerInt, NULL, &sampleRate))
    {
        return NULL;
    }

    // VBRI header follows 32 bytes after the header _ends_.
    pos += sizeof(header) + 32;
    uint8_t vbriHeader[26];
    if (source->readAt(pos, vbriHeader, sizeof(vbriHeader)) < (ssize_t)sizeof(vbriHeader))
    {
        return NULL;
    }
    if (memcmp(vbriHeader, "VBRI", 4))
    {
        return NULL;
    }
    size_t numFrames = U32_AT(&vbriHeader[14]);
    int32_t durationMs = numFrames * 1000L * (sampleRate >= 32000 ? 1152 : 576) / sampleRate;
    sp<VBRISeeker> seeker = new VBRISeeker;
    seeker->mDurationMs = durationMs;
    return seeker;
}

bool VBRISeeker::getDuration(int32_t* durationMs)
{
    if (mDurationMs < 0)
    {
        return false;
    }
    *durationMs = mDurationMs;
    return true;
}
