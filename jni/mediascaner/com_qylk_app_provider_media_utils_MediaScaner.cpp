#include <map>
#include <jni.h>
#include <unistd.h>
#include "MediaScanner.h"
#include "MediaScannerClient.h"
#include "Utils.h"
#include "DataBase.h"
#include "FilesCache.h"
#define DBPATH "/sdcard/media.db"

static const char* dbfile;

typedef map<const  char *,const  char *>::value_type  MAP_ITEM;
static const char* const kClassMediaScannerClient =
    "com/qylk/app/provider/media/utils/MediaScanerClient";

static const char* const kClassMediaScanner =
    "com/qylk/app/provider/media/utils/MediaScaner";

static const char* const kRunTimeException =
    "java/lang/RuntimeException";

static const char* const kIllegalArgumentException =
    "java/lang/IllegalArgumentException";

using namespace android;
static void throwException(JNIEnv* env,const char* clazz,const char* msg)
{
    jclass extclazz=env->FindClass(clazz);
    if(extclazz==NULL)
    {
        MLOGE("class %s can not found",clazz);
        return;
    }
    env->ThrowNew(extclazz,msg);
}

class MyMediaScannerClient  : public MediaScannerClient
{
public:
    MyMediaScannerClient(JNIEnv* env, jobject client):
        mEnv(env),
        mClient(env->NewGlobalRef(client)),
        db(NULL)
    {
        jclass interface=env->FindClass(kClassMediaScannerClient);
        mPostFileMethodID=env->GetMethodID(interface,"postFile","(Ljava/lang/String;)V");
        mPostTagMethodID=env->GetMethodID(interface,"postTag","(Ljava/lang/String;Ljava/lang/String;)V");
        db=new DataBase(dbfile,&filecache);
    }

    virtual ~MyMediaScannerClient()
    {
        mEnv->DeleteGlobalRef(mClient);
        delete[] dbfile;
        delete db;
    }

    status_t doPostTag(const char* key,const char *value)const
    {
        jstring nameStr,valueStr;
        if ((nameStr = mEnv->NewStringUTF(key) )== NULL)
        {
            mEnv->ExceptionClear();
            return NO_MEMORY;
        }

        if ((valueStr = mEnv->NewStringUTF(value)) == NULL)
        {
            mEnv->DeleteLocalRef(nameStr);
            mEnv->ExceptionClear();
            return NO_MEMORY;
        }
        mEnv->CallVoidMethod(mClient, mPostTagMethodID, nameStr, valueStr);
        mEnv->DeleteLocalRef(nameStr);
        mEnv->DeleteLocalRef(valueStr);
        return OK;
    }

    virtual status_t postTag(sp<MetaData> &meta)
    {
        const void *data;
        size_t size;

        struct  Map
        {
            uint32_t KEYID;
            const char *key;
        };

        static const Map kMaps[]=//数据库键值映射表
        {
            {KeyTitle,FIELD_TITLE},
            {KeyTitle_PINYIN,FIELD_TITLE_KEY},
            {KeyArtist,FIELD_ARTIST},
            {KeyArtist_PINYIN,FIELD_ARTIST_KEY},
            {KeyAlbum,FIELD_ALBUM},
            {KeyAlbum_PINYIN,FIELD_ALBUM_KEY},
            {KeyDuratoion,FIELD_DURATION},
            {KeyPATH,FIELD_PATH},
            {KeyLAST_MODIFIED,FILED_LAST_MODIFIED}
        };

        static const size_t KNUM=NELEM(kMaps);
        if(meta!=NULL && meta->size()!=0)
        {
            map<const char *,const char*> item;
            for(size_t i=0; i<KNUM; i++)
            {
                if(meta->getItem(kMaps[i].KEYID,&data,&size)==false)
                {
                    continue;
                }
                item.insert(MAP_ITEM(kMaps[i].key,(const char*)data));
            }
            db->insertVlaues(item);
        }
        return OK;
    }

    virtual bool shouldSkip(const char* file,unsigned long &last_modified)
    {
        return filecache.check_new(file,last_modified);
    }

    void postScan()
    {
        filecache.delete_missing(db);
    }

    virtual  status_t postFile(const char* file)
    {
        jstring pathStr;
        MLOGV("posting media file:%s",file);
        if ((pathStr = mEnv->NewStringUTF(file)) == NULL)
        {
            mEnv->ExceptionClear();
            return NO_MEMORY;
        }
        mEnv->CallVoidMethod(mClient, mPostFileMethodID, pathStr);
        mEnv->DeleteLocalRef(pathStr);
        return OK;
    }

    bool status_ok()const
    {
        return db!=NULL;
    }

private:
    JNIEnv *mEnv;
    jobject mClient;
    jmethodID mPostFileMethodID;
    jmethodID mPostTagMethodID;
    DataBase *db;
    FilesCache filecache;
};

//static void creatOrOpenDB()
//{
//    const static char * SQL_CREAT_TB_AUDIO= "CREATE TABLE [audio](_id integer primary key,title text,title_key text,artist text,artist_id integer not null,artist_key text,album text,album_id integer not null,album_key text,duration long not null,_data text not null)";
//    const static char * SQL_CREAT_TB_TAG ="CREATE TABLE [tag](_id integer,lang int,genre int,rhythm int,sing_method int,age int,subject int,instrument int,lib_id int,time_modified long,play_times int);"
//    char * pErrMsg;
//    int ret = 0;
//    const char* const dbname = "/sdcard/gtest3.db";
//    ret = sqlite3_open(dbname, &db);
//    if ( ret != SQLITE_OK )
//    {
//        printf("open error! : %s", sqlite3_errmsg(db));
//        return;
//    }
//    printf("open db OK!\n");
//    sqlite3_exec( db, sSQL1, 0, 0, &pErrMsg );
//    if ( ret != SQLITE_OK )
//    {
//        printf("SQL error: %s\n", pErrMsg);
//        sqlite3_free(pErrMsg);
//    }
//}


static bool setup(JNIEnv *env, jobject thiz, jstring dbpath)
{
    const char *pathStr = env->GetStringUTFChars(dbpath, NULL);
    if (pathStr == NULL)    // Out of memory
    {
        MLOGE("DataBase path is null");
        return false;
    }
    if(access(pathStr,F_OK)!=0)
    {
        env->ReleaseStringUTFChars(dbpath,pathStr);
        return false;
    }
    size_t len=strlen(pathStr);
    char *db=new char[len+1];
    strcpy(db,pathStr);
    db[len]='\0';
    dbfile=db;
    env->ReleaseStringUTFChars(dbpath,pathStr);
    return true;
}

static bool processDirectory(JNIEnv *env, jobject thiz, jstring path, jobject client)
{
    if (path == NULL)
    {
        throwException(env,kIllegalArgumentException,NULL);
        return false;
    }

    const char *pathStr = env->GetStringUTFChars(path, NULL);
    if (pathStr == NULL)    // Out of memory
    {
        return false;
    }
    MLOGV("will scan from top dir :%s\n",pathStr);
    MyMediaScannerClient myclient(env,client);
    if(!myclient.status_ok())
    {
        env->ReleaseStringUTFChars(path, pathStr);
        return false;
    }
    MediaScanner *ms= new MediaScanner();
    if (ms == NULL)
    {
        env->ReleaseStringUTFChars(path, pathStr);
        throwException(env,kRunTimeException,"No scanner available");
        return false;
    }
    MediaScanResult result =ms->processDirectory(pathStr,myclient);
    if (result == MEDIA_SCAN_RESULT_ERROR)
    {
        MLOGE("An error occurred while scanning directory '%s'.", pathStr);
    }
    myclient.postScan();
    env->ReleaseStringUTFChars(path, pathStr);
    delete ms;
    ms=NULL;
    return result==MEDIA_SCAN_RESULT_OK;
}

static JNINativeMethod gMethods[] =
{
    {
        "processDirectory",
        "(Ljava/lang/String;Lcom/qylk/app/provider/media/utils/MediaScanerClient;)Z",
        (void *)processDirectory
    },
    {
        "setup",
        "(Ljava/lang/String;)Z",
        (void *)setup
    }
};

int register_MediaScanner(JNIEnv *env)
{
    jclass clazz=env->FindClass(kClassMediaScanner);
    if(clazz==NULL)
    {
        MLOGE("native registration can not find class:%s",kClassMediaScanner);
        return JNI_ERR;
    }
    return env->RegisterNatives(clazz, gMethods,NELEM(gMethods));
}

JNIEXPORT jint JNI_OnLoad(JavaVM*vm,void*reserved)
{
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **)&env, JNI_VERSION_1_4) != JNI_OK)
    {
        MLOGE("Error GetEnv");
        return JNI_ERR;
    }
    if (register_MediaScanner(env) < 0)
    {
        MLOGE("methods registration failed.");
        return JNI_ERR;
    }
    return JNI_VERSION_1_4;
}


