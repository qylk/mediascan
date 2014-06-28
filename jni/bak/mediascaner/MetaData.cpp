#include "MetaData.h"
#include "Utils.h"
MetaData::MetaData()
{

}

MetaData::~MetaData()
{
    clear();
}

void MetaData::clear()
{
    mItems.clear();
}

void MetaData::putItem(uint32_t type,uint32_t key, const void* value,size_t size)
{
    //ssize_t i=mItems.indexOfKey(key);
     map<uint32_t,typed_data>::iterator  index=mItems.find(key);
    //if(i<0)
     if(index==mItems.end())
    {
        typed_data item;
        std::pair<map<uint32_t,typed_data>::iterator,bool>  xx=mItems.insert(std::pair(key,item));
       // i=mItems.add(key,item);
    }
    typed_data &item = mItems.editValueAt(i);
    item.setData(type,value,size);
}

size_t MetaData::size() const
{
    return mItems.size();
}

bool MetaData::getItem(uint32_t key, const void**data, size_t* size)
{
   // ssize_t index=mItems.indexOfKey(key);
    iterator<uint32_t,typed_data> index=mItems.find(key);
    if(index==mItems.end())
    //if(index < 0)
    {
        return false;
    }
    const typed_data &item = mItems.valueAt(index);
    item.getData(data,size);
    return true;
}

MetaData::typed_data::typed_data():mSize(0)
{
}

MetaData::typed_data::~typed_data()
{
    clear();
}

MetaData::typed_data::typed_data(const typed_data& from):mSize(0)
{
    if(from.mSize==0)
        return;
    mSize=from.mSize;
    extData=(char*)malloc(mSize+1);
    memcpy(extData, from.extData, mSize+1);
}


void MetaData::typed_data::clear()
{
    if(mSize!=0)
    {
        free(extData);
        extData=NULL;
    }
    mSize=0;
}

void MetaData::typed_data::setData(uint32_t type, const void* data, size_t size)
{
    clear();
    mSize=size;
    if(type==TYPE_C_STRING)
    {
        extData=(char*)malloc(size+1);
        memcpy(extData,data,size);
    }
    else if(type==TYPE_INT32)
    {
        extData=(char*)malloc(11);
        mSize=sprintf(extData,"%d",*(const int32_t*)data);
    }
    else if(type==TYPE_INT64)
    {
        extData=(char*)malloc(20);
        mSize=sprintf(extData,"%lld",*(const int64_t*)data);
    }
    extData[mSize]='\0';
}

void MetaData::typed_data::getData(const void** data, size_t* size) const
{
    *size=mSize;
    *data=extData;
}
