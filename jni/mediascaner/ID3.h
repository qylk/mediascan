#ifndef ID3_H
#define ID3_H
#include <sys/types.h>
#include <utils/String8.h>
#include <media/stagefright/DataSource.h>
using namespace android;
class ID3
{
public:
    enum Version
    {
        ID3_UNKNOWN,
        ID3_V1,
        ID3_V1_1,
        ID3_V2_2,
        ID3_V2_3,
        ID3_V2_4
    };

    enum FRAME_ID
    {
        ID3_FRAME_
    };

    ID3(const sp<DataSource> & file);
    bool isValid() const;
    Version version() const;

    struct Iterator
    {
        Iterator(const ID3 &parent);
        ~Iterator();

        const char* getID() const;
        void getValue(String8 *value) const;
        char *getContent(uint8_t *enc,size_t* length) const;
        bool next();
    private:
        const ID3 &mParent;
        size_t mOffset;

        char *mFrameData;
        size_t mFrameSize;

        bool findNextFrame();
        size_t getHeaderLength() const;
    };
    virtual ~ID3();


private:
    bool mIsValid;
    char *mData;
    size_t mSize;
    size_t mFirstFrameOffset;
    Version mVersion;

    bool parseV1(const sp<DataSource> &file);
    bool parseV2(const sp<DataSource> &file);
    void removeUnsynchronization();
    //static bool ParseSyncsafeInteger(const uint8_t encoded[4], size_t *x);

};

#endif // ID3_H
