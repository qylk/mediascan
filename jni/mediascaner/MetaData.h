#ifndef METADATA_H
#define METADATA_H
//#include <utils/KeyedVector.h>
#include <utils/RefBase.h>
#include <map>
using  android::RefBase;
using std::map;
enum
{
    KeyDuratoion='DURA',
    KeyAlbum='TALB',
    KeyAlbum_PINYIN=KeyAlbum+1,
    KeyTitle='TIT2',
    KeyTitle_PINYIN=KeyTitle+1,
    KeyArtist='TPE1',
    KeyArtist_PINYIN=KeyArtist+1,
    KeyPATH='PATH'
};

//using android::RefBase;
using std::map;
//using android::KeyedVector;

enum Type
{
    TYPE_NONE,
    TYPE_C_STRING,
    TYPE_INT32,
    TYPE_INT64,
    TYPE_FLOAT,
    TYPE_POINTER,
    TYPE_RECT
};

class MetaData : public RefBase
{
public:
    MetaData();
    void clear();
    size_t size() const;
    bool hasKey(const uint32_t &key)const;
    void putItem(uint32_t type,uint32_t key, const void* value,size_t size);
    bool getItem(uint32_t key, const void**data, size_t* size);
protected:
    virtual ~MetaData();
private:
    struct typed_data
    {
    public:
        typed_data();
        ~typed_data();

        typed_data(const typed_data & data);
        void clear();
        void setData(uint32_t type,const void *data,size_t size);
        void getData(const void **data,size_t *size) const;
    private :
        size_t mSize;
        char *extData;
    };
    map<uint32_t, typed_data> mItems;
    //KeyedVector<uint32_t, typed_data> mItems;
};

#endif // METADATA_H
