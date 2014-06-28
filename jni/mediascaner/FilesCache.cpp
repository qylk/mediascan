#include "FilesCache.h"

FilesCache::FilesCache(const size_t num)
{
    cache.resize(num);
}

FilesCache::~FilesCache()
{
    size_t num=cache.size();
    for(size_t i=0; i<num; i++)
    {
        delete[ ] (cache[i].file);
    }
}


void FilesCache::add(uint16_t id, const char* file, long last_modified)
{
    char *pfile=new char[strlen(file)+1];
    strcpy(pfile,file);
    FileEntry entry(id,pfile,last_modified);
    cache.push_back(entry);
}

bool FilesCache::check_new(const char* file, long last_modified) const
{
    size_t num=cache.size();
    for(size_t i=0; i<num; i++)
    {
        if(strcmp(file,cache[i].file)==0)
        {
            if(last_modified>cache[i].last_modified)
                return true;
        }
    }
    return false;
}


