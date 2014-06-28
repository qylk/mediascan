#include "MediaScanner.h"
#include "MediaScannerClient.h"
#include <jni.h>
#include <stdio.h>
#include "sqlite3.h"
#include "Utils.h"
#include "MetaData.h"
#define MAXPATHLEN 256
#define NELEM(x) ((int)sizeof(x)/sizeof(x[0]))

using namespace android;
sqlite3 * db = 0;
class MyMediaScannerClient  : public MediaScannerClient
{
public:
    MyMediaScannerClient()
    {
    }

    virtual ~MyMediaScannerClient()
    {
    }

    virtual status_t postTag(sp<MetaData> &meta)
    {

        size_t size;
        const static char* deriredList[]= {"TIT2","TPE1","TALB"};
        const void* tags[3];
        const size_t listLen=sizeof(deriredList)/sizeof(deriredList[0]);
        if(meta!=NULL && meta->size()!=0)
        {
            for(size_t i=0; i<listLen; i++)
            {
                uint32_t key=U32_AT((const uint8_t*)deriredList[i]);
                if(meta->getItem(key,&tags[i],&size)==false)
                {
                    continue;
                }
            }
            char sql[256];
            sprintf(sql,"insert into media values('%s','%s','%s');",(const char*)tags[0],(const char*)tags[1],(const char*)tags[2]);
            printf("%s\n",sql);
            sqlite3_exec( db, sql, 0, 0, 0);
        }

        return OK;
    }

    virtual  status_t postFile(const char* file)
    {
        printf("parsing mp3 file:%s\n",file);
        return OK;
    }
};


void android_media_MediaScanner_processDirectory(const char *path)
{
    MediaScanner *ms= new MediaScanner();
    if (ms == NULL)
    {
        return;
    }

    if (path == NULL)
    {
        return;
    }
    printf("will scan from dir :%s\n",path);
    MyMediaScannerClient myclient;
    MediaScanResult result =ms->processDirectory(path,myclient);
    if (result == MEDIA_SCAN_RESULT_ERROR)
    {
        MLOGE("An error occurred while scanning directory '%s'.", path);
    }
    delete ms;
    ms=NULL;
}


//static int _sql_callback(void * notused, int argc, char ** argv, char ** szColName)
//{
//    int i;
//    for ( i=0; i < argc; i++ )
//    {
//        printf( "%s = %s\n", *szColName, argv == 0 ? "NUL" : *argv );
//    }
//    return 0;
//}


void creatDB()
{
    const static char * sSQL1 = "create table if not exists media(title varchar(20),artist varchar(20),album varchar(20));";
    char * pErrMsg;
    int ret = 0;
    const char* const dbname = "/sdcard/gtest3.db";
    ret = sqlite3_open(dbname, &db);
    if ( ret != SQLITE_OK )
    {
        printf("open error! : %s", sqlite3_errmsg(db));
        return;
    }
    printf("open db OK!\n");
    ret =  sqlite3_exec( db, sSQL1, 0, 0, &pErrMsg );
    if ( ret != SQLITE_OK )
    {
        printf("SQL error: %s\n", pErrMsg);
        sqlite3_free(pErrMsg);
    }
}

void closeDB()
{
    if(db!=NULL)
    {
        sqlite3_close(db);
    }
}
int main(int argc, char **argv)
{
    //creatDB();
    android_media_MediaScanner_processDirectory("/sdcard/");
    // closeDB();
//    //const char * sSQL2 = "insert into users values('无害化',20,'2009-7-16');";
//    //const char * sSQL3 = "select * from users;";
//
//    //sqlite3 * db = 0;


    // 执行插入记录SQL
    //sqlite3_exec( db, sSQL2, 0, 0, 0);
//    printf("1222\n");
//    // 查询数据表
//    //sqlite3_exec( db, sSQL3, _sql_callback, 0, 0);
//    printf("3333\n");
//    // 关闭数据库
//    //sqlite3_close(db);
//    printf("4444\n");
//    //db = 0;
//    printf("5555\n");
    return 0;
}





