#include "XINGSeeker.h"
#include "Utils.h"
XINGSeeker::XINGSeeker()
{
    mDurationMs=0LL;
}

XINGSeeker::~XINGSeeker()
{
}

static bool parse_xing_header(const sp<DataSource> &source, off64_t first_frame_pos, int32_t *frame_number, int32_t *file_size,unsigned char *table_of_contents, int32_t *quality_indicator,int32_t *duration)
{
    if (frame_number)
    {
        *frame_number = 0;
    }
    if (file_size)
    {
        *file_size = 0;
    }
    if (table_of_contents)
    {
        table_of_contents[0] = 0;
    }
    if (quality_indicator)
    {
        *quality_indicator = 0;
    }
    if (duration)
    {
        *duration = 0;
    }

    uint8_t buffer[4];
    int offset = first_frame_pos;
    if (source->readAt(offset, &buffer, 4) < 4)   //MPEG header
    {
        return false;
    }
    offset += 4;

    uint8_t version, layer, sample_rate_index, stero;
    layer = (buffer[1] >> 1) & 3;//层级

    version = (buffer[1] >> 3) & 3;//版本
    sample_rate_index = (buffer[2] >> 2) & 3;//采样率
    stero = (buffer[3] >> 6) & 3;//声道
    if (layer == 0)
    {
        return false;
    }
    if (version == 1)  //未定义版本
    {
        return false;
    }
    if (sample_rate_index == 3)  //未定义采样率
    {
        return false;
    }
    // determine offset of XING header
    if(version&1)        // mpeg1
    {
        if (stero != 3)
            offset += 32;//在偏移MPEG头32处
        else
            offset += 17;
    }
    else            // mpeg2
    {
        if (stero != 3)
            offset += 17;
        else
            offset += 9;
    }

    if (source->readAt(offset, &buffer, 4) < 4)   // XING header ID
    {
        return false;
    }

    // Check XING ID
    if ((buffer[0] != 'X') || (buffer[1] != 'i')
            || (buffer[2] != 'n') || (buffer[3] != 'g'))
    {
        if ((buffer[0] != 'I') || (buffer[1] != 'n')
                || (buffer[2] != 'f') || (buffer[3] != 'o'))
        {
            return false;
        }
    }

    MLOGV("xing frame found");
    offset += 4;
    if (source->readAt(offset, &buffer, 4) < 4)   // flags
    {
        return false;
    }
    offset += 4;
    uint32_t flags = U32_AT(buffer);

    if (flags & 0x0001)    // Number of Frames field is present
    {
        if (source->readAt(offset, buffer, 4) < 4)
        {
            return false;
        }
        int32_t frame_num = U32_AT(buffer);

        if (frame_number)
        {
            *frame_number = frame_num;
        }
        // Samples per Frame: 1. index = MPEG Version ID, 2. index = Layer
        const int samplesPerFrames[2][3] =
        {
            { 384, 1152, 576  }, // MPEG 2, 2.5: layer1, layer2, layer3
            { 384, 1152, 1152 }, // MPEG 1: layer1, layer2, layer3
        };
        // sampling rates in hertz: 1. index = MPEG Version ID, 2. index = sampling rate index
        const int samplingRates[4][3] =
        {
            { 11025, 12000, 8000,  },    // MPEG 2.5
            { 0,     0,     0,     },    // reserved
            { 22050, 24000, 16000, },    // MPEG 2
            { 44100, 48000, 32000, }     // MPEG 1
        };
        if (duration)
        {
            *duration = (int32_t) (1000LL*frame_num * samplesPerFrames[version&1][3-layer] / samplingRates[version][sample_rate_index]);
        }
        offset += 4;
    }
    if (flags & 0x0002)    // file size is present
    {
        if (file_size)
        {
            if (source->readAt(offset, buffer, 4) < 4)
            {
                return false;
            }
            *file_size = U32_AT(buffer);
        }
        offset += 4;
    }
    if (flags & 0x0004)    // TOC field is present
    {
        if (table_of_contents)
        {
            if (source->readAt(offset + 1, table_of_contents, 99) < 99)
            {
                return false;
            }
        }
        offset += 100;
    }
    if (flags & 0x0008)    // Quality indicator field is present
    {
        if (quality_indicator)
        {
            if (source->readAt(offset, buffer, 4) < 4)
            {
                return false;
            }
            *quality_indicator = U32_AT(buffer);
        }
    }
    return true;
}

sp<XINGSeeker> XINGSeeker::CreateFromSource(const sp<DataSource>& source, off64_t first_frame_pos)
{
    sp<XINGSeeker> seeker = new XINGSeeker;
    if (!parse_xing_header(source, first_frame_pos,NULL, NULL, NULL,NULL, &(seeker->mDurationMs)))  //只需要Duration信息
    {
        return NULL;
    }
    return seeker;
}

bool XINGSeeker::getDuration(int32_t* durationMs)
{
    if (mDurationMs < 0)
    {
        return false;
    }
    *durationMs = mDurationMs;
    return true;
}
