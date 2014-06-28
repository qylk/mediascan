#include "DataBase.h"
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include "Utils.h"
const static  char* DB_INSERT="insert into "TABLE"(";

int catcheFiles(void* para ,int nCount,char** pValue,char** pName)
{
    FilesCache *cache=(FilesCache *)para;
    cache->add(atoi(pValue[0]),pValue[1],(unsigned long)atol(pValue[2]));
    return 0;
}

DataBase::DataBase(const char* dbname,FilesCache* filescache)
{
    struct stat statbuf;
    stat(dbname,&statbuf);
    sqlite3_open(dbname,&db);
    if(db==NULL)
    {
        MLOGE("database open failed,erron is %d",errno);
        return;
    }
    int nResult =sqlite3_exec(db,"select _id,_data,last_modified from audio;",catcheFiles,(void*)filescache,0);
    if (nResult != SQLITE_OK)
    {
        sqlite3_close(db);
        return;
    }
    sqlite3_exec(db, "begin;", 0,0,0);
    sqlbuf=new char[512];
    sqlbuf[0]='\0';//SQLÓï¾ä»º³åÇø
}

void DataBase::postDB()
{
//    sqlite3_exec(db, "CREATE TABLE artists(_id integer primary key,name text,tracknum integer default 0,artindex text);", 0,0,0);
//    sqlite3_exec(db, "CREATE TABLE albums(_id integer primary key,name text,tracknum integer default 0);", 0,0,0);
//    sqlite3_exec(db, "INSERT INTO artists(name) select distinct artist from audio;", 0,0,0);
//    sqlite3_exec(db, "INSERT INTO albums(name) select distinct album from audio;", 0,0,0);
//    sqlite3_exec(db, "UPDATE audio set artist_id=(select _id from artists where audio.artist=artists.name);", 0,0,0);
//    sqlite3_exec(db, "UPDATE audio set album_id=(select _id from albums where audio.album=albums.name);", 0,0,0);
}

DataBase::~DataBase()
{
    if(db!=NULL)
    {
        //finishDataBase();
        sqlite3_exec(db, "commit;", 0,0,0);
        sqlite3_close(db);
        delete[] sqlbuf;
        sqlbuf=NULL;
    }
}

void DataBase::deleteValues(int& rowid)
{
    char *charpos=sqlbuf;
    sprintf(charpos,"delete from audio where _id=%d;",rowid);
    int ret = sqlite3_exec(db,sqlbuf,0,0,0);
    if(ret !=SQLITE_OK)
    {
        MLOGE("fail to delete an item from database,sql=%s",sqlbuf);
    }
}

void DataBase::insertVlaues(map<const char*, const char*> &item)
{
    char *charpos=sqlbuf;
    strcpy(charpos,DB_INSERT);
    map<const char*, const char*>::const_iterator iter=item.begin();
    charpos+=strlen(sqlbuf);
    while(iter != item.end())
    {
        charpos+=sprintf(charpos," '%s',",iter->first);
        iter++;
    }
    charpos--;//È¥µôÄ©Î²µÄ¡®£¬¡¯
    charpos+=sprintf(charpos,") values(");
    iter=item.begin();
    while(iter != item.end())
    {
        charpos+=sprintf(charpos," '%s',",iter->second);
        iter++;
    }
    sprintf(charpos-1,");");
    charpos--;//È¥µôÄ©Î²µÄ¡®£¬¡¯
    int ret = sqlite3_exec(db,sqlbuf,0,0,0);
    //MLOGD("%s",sqlbuf);
    if(ret !=SQLITE_OK)
    {
        MLOGE("fail to insert an media item into database,sql=%s",sqlbuf);
    }
    sqlbuf[0]='\0';
}
