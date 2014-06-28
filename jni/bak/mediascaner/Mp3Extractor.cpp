#include "Mp3Extractor.h"
#include "ID3.h"
#include "Utils.h"
#include "MetaData.h"
#include "utfconv.h"
#include "MpegUtils.h"
#include "XINGSeeker.h"
#include "VBRISeeker.h"

using namespace android;
static const uint32_t kMask = 0xfffe0c00;
static bool Resync(const sp<DataSource> &source, uint32_t match_header,off64_t *inout_pos, off64_t *post_id3_pos, uint32_t *out_header)
{
    if (post_id3_pos != NULL)
    {
        *post_id3_pos = 0;
    }

    if (*inout_pos == 0)
    {
        for (;;)
        {
            uint8_t id3header[10];
            if (source->readAt(*inout_pos, id3header, sizeof(id3header))
                    < (ssize_t)sizeof(id3header))
            {
                return false;
            }
            if (memcmp("ID3", id3header, 3))  //无ID3
            {
                break;
            }
            // Skip the ID3v2 header.
            size_t len =
                ((id3header[6] & 0x7f) << 21)
                | ((id3header[7] & 0x7f) << 14)
                | ((id3header[8] & 0x7f) << 7)
                | (id3header[9] & 0x7f);

            len += 10;
            *inout_pos += len;//跳过ID3
        }

        if (post_id3_pos != NULL)
        {
            *post_id3_pos = *inout_pos;
        }
    }

    off64_t pos = *inout_pos;
    bool valid = false;

    const size_t kMaxReadBytes = 1024;
    const size_t kMaxBytesChecked = 128 * 1024;
    uint8_t buf[kMaxReadBytes];
    ssize_t bytesToRead = kMaxReadBytes;
    ssize_t totalBytesRead = 0;
    ssize_t remainingBytes = 0;
    bool reachEOS = false;
    uint8_t *tmp = buf;

    do
    {
        if (pos >= *inout_pos + kMaxBytesChecked)
        {
            MLOGV("giving up at offset %lld", pos);
            break;
        }

        if (remainingBytes < 4)
        {
            if (reachEOS)
            {
                break;
            }
            else
            {
                memcpy(buf, tmp, remainingBytes);
                bytesToRead = kMaxReadBytes - remainingBytes;

                /*
                 * The next read position should start from the end of
                 * the last buffer, and thus should include the remaining
                 * bytes in the buffer.
                 */
                totalBytesRead = source->readAt(pos + remainingBytes,buf + remainingBytes,bytesToRead);
                if (totalBytesRead <= 0)
                {
                    break;
                }
                reachEOS = (totalBytesRead != bytesToRead);
                totalBytesRead += remainingBytes;
                remainingBytes = totalBytesRead;
                tmp = buf;
                continue;
            }
        }

        uint32_t header = U32_AT(tmp);

        if (match_header != 0 && (header & kMask) != (match_header & kMask))
        {
            ++pos;
            ++tmp;
            --remainingBytes;
            continue;
        }

        size_t frame_size;
        int sample_rate, num_channels, bitrate;
        if (!GetMPEGAudioFrameSize(header, &frame_size,&sample_rate, &num_channels, &bitrate,NULL))
        {
            ++pos;
            ++tmp;
            --remainingBytes;
            continue;
        }
        MLOGV("found possible 1st frame at %lld (header = 0x%08x)", pos, header);

        // We found what looks like a valid frame,
        // now find its successors.

        off64_t test_pos = pos + frame_size;//下一帧位置

        valid = true;
        for (int j = 0; j < 3; ++j)  //连续检查下3个帧是否合法
        {
            uint8_t tmp[4];
            if (source->readAt(test_pos, tmp, 4) < 4)
            {
                valid = false;
                break;
            }

            uint32_t test_header = U32_AT(tmp);

            if ((test_header & kMask) != (header & kMask))
            {
                valid = false;
                break;
            }

            size_t test_frame_size;
            if (!GetMPEGAudioFrameSize(test_header, &test_frame_size))
            {
                valid = false;
                break;
            }
            MLOGV("found subsequent frame #%d at %lld", j + 2, test_pos);
            test_pos += test_frame_size;
        }

        if (valid)
        {
            *inout_pos = pos;
            if (out_header != NULL)
            {
                *out_header = header;
            }
        }
        else
        {
            MLOGV("no dice, no valid sequence of frames found.");
        }

        ++pos;
        ++tmp;
        --remainingBytes;
    }
    while (!valid);
    return valid;
}

//static bool skipID3(const sp<DataSource>& source ,size_t *posAfterSkip)
//{
//    *posAfterSkip=0;
//    uint8_t id3header[10];
//    if (source->readAt(0, id3header, sizeof(id3header))< (ssize_t)sizeof(id3header))
//    {
//        return false;
//    }
//    if (memcmp("ID3", id3header, 3))  //无ID3
//    {
//        return true;
//    }
//    // Skip the ID3v2 header.
//    size_t len =
//        ((id3header[6] & 0x7f) << 21)
//        | ((id3header[7] & 0x7f) << 14)
//        | ((id3header[8] & 0x7f) << 7)
//        | (id3header[9] & 0x7f);
//
//    len += 10;
//    *posAfterSkip=len;
//    return true;
//}

Mp3Extractor::Mp3Extractor(const sp<DataSource>& source):mDataSource(source),mInitCheck(NO_INIT)
{
    off64_t post_ID3_pos;//ID3标签末尾
    off64_t pos = 0;//找到的MPEG第一帧位置
    uint32_t header;//MPEG头
    bool success = Resync(mDataSource, 0, &pos, &post_ID3_pos, &header);//寻找MPEG第一帧
    if(!success)
        return;
    mFirstFramePos=pos;//第一帧位置
    int bitrate;//位速
    //size_t frameSize;
    if(!GetMPEGAudioFrameSize(header, NULL, NULL, NULL,&bitrate, NULL)) //获取位速
    {
        MLOGE("not a valid data frame");
        return;
    }
    MLOGV("bitrate :%d kps",bitrate);
    //find VBR
    sp<Mp3Seeker> mSeeker = XINGSeeker::CreateFromSource(mDataSource, mFirstFramePos);//从第一帧寻找XING
    if (mSeeker == NULL)
    {
        mSeeker = VBRISeeker::CreateFromSource(mDataSource, post_ID3_pos);//从ID3末尾寻找VBRI
    }

    int32_t durationMs;//播放时间
    if (mSeeker == NULL || !mSeeker->getDuration(&durationMs))//CBR asserted
    {
        off64_t fileSize;
        if (mDataSource->getSize(&fileSize) == OK)
        {
            durationMs = 8000/1000 * (fileSize - mFirstFramePos) / bitrate;
        }
        else
        {
            durationMs = -1;
        }
    }
    mMeta = new MetaData;

    if (durationMs >= 0)
    {
        mMeta->putItem(TYPE_INT32,KeyDuratoion,&durationMs,0);
    }
    mInitCheck = OK;
}

static bool TagAcceptable(const uint32_t tag)
{
    static const uint32_t kAcceptable[] =
    {
        'TIT2','TPE1','TALB'
    };
    static const size_t kNum =sizeof(kAcceptable) / sizeof(kAcceptable[0]);

    for (size_t i = 0; i < kNum; ++i)
    {
        if (tag==kAcceptable[i])
        {
            return true;
        }
    }
    return false;
}

//static status_t code_convert(const char *from_charset,const char *to_charset,char *inbuf,size_t inlen,char *outbuf,size_t *outleft)
//{
//    char **pin = &inbuf;
//    char **pout = &outbuf;
//    iconv_t cd = iconv_open(to_charset,from_charset);
//    if (cd==NULL)
//    {
//        MLOGE("OPEN ICONV ERROR");
//        return UNKNOWN_ERROR;
//    }
//    if (iconv(cd,pin,&inlen,pout,outleft)==(size_t)-1)
//    {
//        MLOGE("failed to  convert£¬error code is %d",errno);
//        iconv_close(cd);
//        return UNKNOWN_ERROR;
//    }
//    iconv_close(cd);
//    return OK;
//}


static size_t code_convert2UTF8(const char *from_charset,const char *src,const size_t srclen,char *outbuf,size_t *outlen)
{
    size_t completeNum=0;
    if(strcmp(from_charset,"GB2312")==0)
    {
        completeNum=conv_cp936_utf8((const uint8_t*)src,srclen,outbuf,outlen);
    }
    else if(strcmp(from_charset,"UTF-16")==0)
    {
        completeNum= conv_utf16le_utf8((const uint16_t*)src,srclen/2,outbuf,outlen);
    }
    else if(strcmp(from_charset,"UTF-16BE")==0)
    {
        completeNum=conv_utf16be_utf8((const uint16_t*)src,srclen/2,outbuf,outlen);
    }
    return completeNum;
}

sp<MetaData> Mp3Extractor::getMetaData()
{
    ID3 id3(mDataSource);
    if (!id3.isValid())
    {
        return mMeta;
    }

    struct Map
    {
        int key;
        const char *tag1;
        const char *tag2;
    };
    ID3::Iterator it(id3);
    while (it.next())
    {
        const char *id=it.getID();
        uint32_t key=U32_AT((const uint8_t*)id);
        if(!TagAcceptable(key))
            continue;
        size_t contentSize;
        uint8_t encoding;
        char *base=it.getContent(&encoding,&contentSize);
        if(contentSize==0)
            continue;
        size_t bufferSize=0;
        char *buffer=NULL;
        const char *charset=NULL;
        switch(encoding)
        {
        case 0x00:
        {
            charset="GB2312";
            break;
        }
        case 0x01:
        {
            charset="UTF-16";
            break;
        }
        case 0x02:
        {
            charset="UTF-16BE";
            break;
        }
        case 0x03:
        {
            buffer=base;
            bufferSize=contentSize;
            break;
        }
        }

        if(charset!=NULL)
        {
            bufferSize=contentSize*3;
            buffer=new char[bufferSize];
            size_t outlen;
            memset(buffer,0x00,bufferSize);
            size_t completeNum=code_convert2UTF8(charset,base,contentSize,buffer,&outlen);
            if(completeNum==0)
            {
                delete[] buffer;
                buffer=NULL;
                continue;
            }
            bufferSize =outlen;
        }
        mMeta->putItem(TYPE_C_STRING,key,buffer,bufferSize);

        buffer[bufferSize]='\0';
        MLOGE("%d--%s",bufferSize,buffer);
        if(key==KeyTitle || key==KeyArtist || key==KeyAlbum)
        {
            uint16_t ucs2buf[bufferSize];
            size_t outlen;
            conv_utf8_ucs2((const uint8_t*)buffer,bufferSize,ucs2buf,&outlen);
            char pinyin[bufferSize];
            size_t letterNum;
            ucs2_getLetter(ucs2buf,outlen,pinyin,&letterNum);
            mMeta->putItem(TYPE_C_STRING,key+1,pinyin,letterNum);
        }
        if(buffer!=base)
        {
            delete[] buffer;
            buffer=NULL;
        }
    }
    return mMeta;
}

bool Mp3Extractor::hasInited() const
{
    return mInitCheck==OK;
}

