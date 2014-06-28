#ifndef FILESCACHE_H
#define FILESCACHE_H
#include <vector>
using std::vector;


class DataBase;

class FilesCache
{
public:
    FilesCache();
    void add(int id,const char*file,unsigned long last_modified);
    bool check_new(const char* file,unsigned long &last_modified) ;
    void delete_missing(DataBase *db);
    virtual ~FilesCache();
protected:
private:
    struct FileEntry
    {
        int id;
        const char*file;
        unsigned long last_modified;
        bool seen;
    };
    vector<FileEntry> cache;
};

#endif // FILESCACHE_H
