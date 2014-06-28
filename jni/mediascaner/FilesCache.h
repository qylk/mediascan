#ifndef FILESCACHE_H
#define FILESCACHE_H
#include <vector>

using std::vector;

class FilesCache
{
    public:
        FilesCache(const size_t num);
        void add(uint16_t id,const char*file,long last_modified);
        bool check_new(const char* file,long last_modified) const;
        virtual ~FilesCache();
    protected:
    private:
        struct FileEntry
        {
                uint16_t id;
                const char*file;
                long last_modified;
        };
        vector<FileEntry> cache;
};

#endif // FILESCACHE_H
