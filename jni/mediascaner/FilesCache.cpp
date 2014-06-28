#include "FilesCache.h"
#include <string.h>
#include "DataBase.h"
#include "Utils.h"
FilesCache::FilesCache()
{
}

FilesCache::~FilesCache()
{
    size_t num=cache.size();
    for(size_t i=0; i<num; i++)
    {
        delete[ ] (cache[i].file);
    }
    cache.clear();
}

void FilesCache::delete_missing(DataBase *db)
{
    size_t num=cache.size();
    for(size_t i=0; i<num; i++)
    {
        if(cache[i].seen==false)
        {
            db->deleteValues(cache[i].id);
        }
    }
}

void FilesCache::add(int id, const char* file, unsigned long last_modified)
{
    char *pfile=new char[strlen(file)+1];
    strcpy(pfile,file);
    FileEntry entry=
    {
id:
        id,
file:
        pfile,
last_modified:
        last_modified,
seen:
        false
    };
    cache.push_back(entry);
}

bool FilesCache::check_new(const char* file, unsigned long &last_modified)
{
    size_t num=cache.size();
    for(size_t i=0; i<num; i++)
    {
        if(strcmp(file,cache[i].file)==0)
        {
            cache[i].seen=true;
            if(last_modified<=cache[i].last_modified)
                return true;
        }
    }
    return false;
}


