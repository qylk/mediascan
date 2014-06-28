#ifndef DATABASE_H
#define DATABASE_H
#include <map>
#include "sqlite3.h"
#define FIELD_TITLE  "title"
#define FIELD_TITLE_KEY  "title_key"
#define FIELD_ARTIST  "artist"
#define FIELD_ARTIST_KEY  "artist_key"
#define FIELD_ALBUM  "album"
#define FIELD_ALBUM_KEY  "album_key"
#define FIELD_DURATION "duration"
#define FIELD_PATH  "_data"

#define TABLE "audio"
using std::map;

class DataBase
{
public:
    DataBase(const char *dbname,FilesCache *filescache);
    void insertVlaues(map<const char*, const char* > &item);
    void finishDataBase();
    virtual ~DataBase();
private:
    void catcheFiles(FilesCache *filecache);
    sqlite3 *db;
    char    *sqlbuf;
};

#endif // DATABASE_H
