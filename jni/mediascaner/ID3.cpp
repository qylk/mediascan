#include "ID3.h"
#include <string.h>
#include "Utils.h"
#include <stdio.h>
#include <utils/String8.h>
//#include <byteswap.h>
using namespace android;
static const size_t kMaxMetadataSize = 3 * 1024 * 1024;

ID3::ID3(const sp<DataSource> &file): mIsValid(false),mData(NULL),mSize(0),mFirstFrameOffset(0),mVersion(ID3_UNKNOWN)
{
    mIsValid = parseV2(file);
    if (!mIsValid)
    {
        mIsValid = parseV1(file);
    }
}

ID3::~ID3()
{
    if (mData)
    {
        free(mData);
        mData = NULL;
    }
}

static size_t ParseSyncsafeInteger(const uint8_t encoded[4])
{
    return ((encoded[0] & 0x7f) << 21) | ((encoded[1] & 0x7f) << 14) | ((encoded[2] & 0x7f) << 7)  | (encoded[3] & 0x7f);
}


bool ID3::parseV2(const sp<DataSource>& file)
{
    struct id3_header  //10 bytes
    {
        char id[3];
        uint8_t version_major;//主版本号
        uint8_t version_minor;//副版本号
        uint8_t flags;
        uint8_t enc_size[4];//长度
    };
    id3_header header;
    //读取标签头
    if (file->readAt(
                0, &header, sizeof(header)) != (ssize_t)sizeof(header))
    {
        return false;
    }

    //检测ID3标志
    if (memcmp(header.id, "ID3", 3))
    {
        return false;
    }

    //错误的版本
    if (header.version_major == 0xff || header.version_minor == 0xff)
    {
        return false;
    }

    //不处理flag情况，如非同步，扩展头等情形
    if (header.flags & 0xff)
    {
        return false;
    }

    size_t size=ParseSyncsafeInteger(header.enc_size);

    if (size > kMaxMetadataSize)
    {
        MLOGE("skipping huge ID3 metadata of size %d", size);
        return false;
    }
    mData = (char *)malloc(size);

    if (mData == NULL)
    {
        return false;
    }

    mSize = size;
    //读取整个标签内容，不包括ID3头
    if (file->readAt(sizeof(header), mData, mSize) != (ssize_t)mSize)
    {
        free(mData);
        mData = NULL;
        return false;
    }
    mFirstFrameOffset = 0;
    if (header.version_major == 2)
    {
        mVersion = ID3_V2_2;
    }
    else if (header.version_major == 3)
    {
        mVersion = ID3_V2_3;
    }
    else
    {
        //CHECK_EQ(header.version_major, 4);
        mVersion = ID3_V2_4;
    }
    return true;
}

ID3::Iterator::Iterator(const ID3& parent): mParent(parent)
{
    mOffset=parent.mFirstFrameOffset;
    mFrameData=NULL;
    mFrameSize=0;
}

ID3::Iterator::~Iterator()
{
}

bool ID3::Iterator::findNextFrame()
{
    if (mParent.mVersion == ID3_V2_2)
    {
        if (mOffset + 6 > mParent.mSize)  //数据区越界
        {
            return false;
        }

        mFrameSize =
            (mParent.mData[mOffset + 3] << 16)
            | (mParent.mData[mOffset + 4] << 8)
            | mParent.mData[mOffset + 5];


        mFrameSize+=6;

        if (mOffset + mFrameSize> mParent.mSize)//错误的标签
        {
            return false;
        }

        //错误标识符
        if (!memcmp(&mParent.mData[mOffset], "\0\0\0", 3))
        {
            mOffset+=mFrameSize;
            return findNextFrame();
        }

        mFrameData = &mParent.mData[mOffset + 6];
    }
    else if (mParent.mVersion == ID3_V2_3
             || mParent.mVersion == ID3_V2_4)
    {
        if (mOffset + 10 > mParent.mSize)  //越界
        {
            return false;
        }
        if (mParent.mVersion == ID3_V2_4)
        {
            mFrameSize= ParseSyncsafeInteger( (uint8_t *)&mParent.mData[mOffset + 4]);
        }
        else
        {
            mFrameSize = U32_AT((uint8_t *)&mParent.mData[mOffset + 4]);
        }
        mFrameSize+=10;
        if(mOffset+mFrameSize>mParent.mSize)
        {
            return false;
        }
        //错误标识符
        if (!memcmp(&mParent.mData[mOffset], "\0\0\0\0", 4))
        {
            mOffset+=mFrameSize;
            return findNextFrame();
        }

        uint16_t flags = U16_AT((uint8_t *)&mParent.mData[mOffset + 8]);

        if ((mParent.mVersion == ID3_V2_4 && (flags & 0x000c))
                || (mParent.mVersion == ID3_V2_3 && (flags & 0x00c0)))  //压缩或加密，则不处理
        {
            mOffset+=mFrameSize;
            return findNextFrame();
        }

        mFrameData = &mParent.mData[mOffset + 10];
    }
    else
    {
        //CHECK(mParent.mVersion == ID3_V1 || mParent.mVersion == ID3_V1_1);
        if (mOffset >= mParent.mSize)
        {
            return false;
        }
        mFrameData = &mParent.mData[mOffset];
        switch (mOffset)
        {
        case 3:
        case 33:
        case 63:
            mFrameSize = 30;
            break;
        case 93:
            mFrameSize = 4;
            break;
        case 97:
            if (mParent.mVersion == ID3_V1)
            {
                mFrameSize = 30;
            }
            else
            {
                mFrameSize = 29;
            }
            break;
        case 126:
            mFrameSize = 1;
            break;
        case 127:
            mFrameSize = 1;
            break;
        default:
            break;
        }
    }
    return true;
}

const char* ID3::Iterator::getID() const
{
    if (mFrameData == NULL)
    {
        return NULL;
    }
    if (mParent.mVersion == ID3_V2_2 || mParent.mVersion == ID3_V2_3 || mParent.mVersion == ID3_V2_4)
    {
        return mParent.mData+mOffset;
    }
    else
    {
        //assert(mParent.mVersion == ID3_V1 || mParent.mVersion == ID3_V1_1);
        const char *id;
        switch (mOffset)
        {
        case 3:
            id="TIT2";
            break;
        case 33:
            id="TPE1";
            break;
        case 63:
            id="TALB";
            break;
        case 93:
            id="TYER";
            break;
        case 97:
            id="COMM";
            break;
        case 126:
            id="TRCK";
            break;
        case 127:
            id="TCON";
            break;
        default:
            id="ERRO";
            break;
        }
        return id;
    }
}

char *ID3::Iterator::getContent(uint8_t *enc,size_t* length) const
{
    *length = 0;
    if (mFrameData == NULL)
    {
        return NULL;
    }
    *length = mFrameSize - getHeaderLength()-1;

    if(mParent.mVersion==ID3_V1_1)
    {
        *enc=0x00;
        return mFrameData;
    }
    else
    {
        *enc=mFrameData[0];
        return mFrameData+1;
    }
}

bool ID3::Iterator::next()
{
    mOffset+=mFrameSize;
    return findNextFrame();
}

size_t ID3::Iterator::getHeaderLength() const
{
    if (mParent.mVersion == ID3_V2_2)
    {
        return 6;
    }
    else if (mParent.mVersion == ID3_V2_3 || mParent.mVersion == ID3_V2_4)
    {
        return 10;
    }
    else
    {
        //CHECK(mParent.mVersion == ID3_V1 || mParent.mVersion == ID3_V1_1);
        return 0;
    }
}

bool ID3::isValid() const
{
    return mIsValid;
}

ID3::Version ID3::version() const
{
    return mVersion;
}

bool ID3::parseV1(const sp<DataSource> & file)
{
    const size_t V1_TAG_SIZE = 128;

    off64_t size;
    if (file->getSize(&size) != OK || size < (off64_t)V1_TAG_SIZE)
    {
        return false;
    }

    mData = (char *)malloc(V1_TAG_SIZE);
    if (file->readAt(size - V1_TAG_SIZE, mData, V1_TAG_SIZE)
            != (ssize_t)V1_TAG_SIZE)
    {
        free(mData);
        mData = NULL;
        return false;
    }

    if (memcmp("TAG", mData, 3))
    {
        free(mData);
        mData = NULL;
        return false;
    }

    mSize = V1_TAG_SIZE;
    mFirstFrameOffset = 3;

    if (mData[V1_TAG_SIZE - 3] != 0)
    {
        mVersion = ID3_V1;
    }
    else
    {
        mVersion = ID3_V1_1;
    }
    return true;
}

/*
去除非同步块
*/
void ID3::removeUnsynchronization()
{
    for (size_t i = 0; i + 1 < mSize; ++i)
    {
        if (mData[i] == 0xff && mData[i + 1] == 0x00)
        {
            memmove(&mData[i + 1], &mData[i + 2], mSize - i - 2);
            --mSize;
        }
    }
}
