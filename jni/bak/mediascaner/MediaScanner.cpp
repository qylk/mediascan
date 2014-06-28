#include "MediaScanner.h"
#include <dirent.h>
#include <sys/stat.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/FileSource.h>
#include <media/stagefright/DataSource.h>
#include <media/stagefright/MediaDebug.h>
#include "MediaExtractor.h"
#include "ID3.h"
#include "Utils.h"

#define MAX_DEPTH_SCAN 3//最大深度控制
static int depth=0;//深度控制

static bool FileHasAcceptableExtension(const char *extension)
{
    return !strcasecmp(extension, ".mp3");
}

MediaScanner::MediaScanner()
{

}

MediaScanner::~MediaScanner()
{
}


/*
扫描前准备工作
*/
MediaScanResult MediaScanner::processDirectory(const char* path, MediaScannerClient& client)
{
    int pathLength = strlen(path);
    if (pathLength >= PATH_MAX)  //超长路径
    {
        return MEDIA_SCAN_RESULT_SKIPPED;
    }
    char* pathBuffer = (char *)malloc(PATH_MAX + 1);
    if (!pathBuffer)
    {
        return MEDIA_SCAN_RESULT_ERROR;
    }

    int pathRemaining = PATH_MAX - pathLength;
    strcpy(pathBuffer, path);
    if (pathLength > 0 && pathBuffer[pathLength - 1] != '/')  //路径末尾补充'/'
    {
        pathBuffer[pathLength] = '/';
        pathBuffer[pathLength + 1] = 0;
        --pathRemaining;
    }

    depth=0;
    MediaScanResult result = doProcessDirectory(pathBuffer, pathRemaining, client, false);
    free(pathBuffer);
    return result;
}

/*
    扫描路径
*/
MediaScanResult MediaScanner::doProcessDirectory(char* path, int pathRemaining, MediaScannerClient& client, bool noMedia)
{
    bool childnoMedia=noMedia;
    if((++depth)>MAX_DEPTH_SCAN)
        return MEDIA_SCAN_RESULT_SKIPPED;
    printf("scanning path:%d-%s\n",depth-1,path);

    char* fileSpot = path + strlen(path);//路径结尾
    struct dirent* entry;
    // 检测该路径下是否有.nomedia文件
    if (pathRemaining >= 8 /* strlen(".nomedia") */ )
    {
        strcpy(fileSpot, ".nomedia");//测试文件path/.nomedia
        if (access(path, F_OK) == 0)
        {
            MLOGV("found .nomedia, setting noMedia flag\n");
            childnoMedia=true;
            //return MEDIA_SCAN_RESULT_SKIPPED;
        }
        // 恢复路径
        fileSpot[0] = 0;
    }

    DIR* dir = opendir(path);
    if (!dir)
    {
        MLOGW("Error opening directory '%s', skipping: %s.", path, strerror(errno));//获取系统错误信息
        return MEDIA_SCAN_RESULT_SKIPPED;
    }
    while ((entry = readdir(dir)))
    {
        doProcessDirectoryEntry(path, pathRemaining, client, childnoMedia, entry, fileSpot);
    }
    closedir(dir);
    return MEDIA_SCAN_RESULT_OK;
}

/*
    扫描子项目
*/
MediaScanResult MediaScanner::doProcessDirectoryEntry(char* path, int pathRemaining, MediaScannerClient& client, bool noMedia, struct dirent* entry, char* fileSpot)
{
    struct stat statbuf;
    const char* name = entry->d_name;//文件名，不含路径
    int nameLength = entry->d_reclen;//文件名长度

    // ignore "." and ".."
    if (name[0] == '.' && (name[1] == 0 || (name[1] == '.' && name[2] == 0)))
    {
        return MEDIA_SCAN_RESULT_SKIPPED;
    }



    if (nameLength + 1 > pathRemaining)
    {
        return MEDIA_SCAN_RESULT_SKIPPED;// path too long!
    }
    strcpy(fileSpot, name);//文件完整路径

    int type = entry->d_type;
    if (type == DT_UNKNOWN)
    {
        // If the type is unknown, stat() the file instead.
        // This is sometimes necessary when accessing NFS mounted filesystems, but
        // could be needed in other cases well.
        if (stat(path, &statbuf) == 0)
        {
            if (S_ISREG(statbuf.st_mode))
            {
                type = DT_REG;
            }
            else if (S_ISDIR(statbuf.st_mode))
            {
                type = DT_DIR;
            }
        }
        else
        {
            MLOGW("stat() failed for file %s:,%s", path, strerror(errno) );
        }
    }
    if (type == DT_DIR)  //普通目录
    {
        if (name[0] == '.')//.开头的目录，忽略
            return MEDIA_SCAN_RESULT_SKIPPED;
        // and now process its contents
        strcat(fileSpot, "/");//增加路径隔符
        //扫描子目录，递归
        MediaScanResult result = doProcessDirectory(path, pathRemaining - nameLength - 1,
                                 client, false);
        depth--;
        if (result == MEDIA_SCAN_RESULT_ERROR)
        {
            return MEDIA_SCAN_RESULT_ERROR;
        }
    }
    else if (type == DT_REG)    //普通文件
    {
        if(noMedia)
            return MEDIA_SCAN_RESULT_SKIPPED;

        MediaScanResult result =processFile(path,client);
        if (result == MEDIA_SCAN_RESULT_ERROR)
        {
            return MEDIA_SCAN_RESULT_ERROR;
        }
    }
    return MEDIA_SCAN_RESULT_OK;
}

MediaScanResult MediaScanner::processFile(const char* path,MediaScannerClient& client)
{
    MLOGV("processFile '%s'.", path);
    const char *extension = strrchr(path, '.');//获取后缀名
    if (!extension)
    {
        return MEDIA_SCAN_RESULT_SKIPPED;
    }

    if (!FileHasAcceptableExtension(extension))
    {
        return MEDIA_SCAN_RESULT_SKIPPED;
    }

    client.beginFile(path);
    MediaScanResult result = ExtractMetaData(path,client);
    if(result==MEDIA_SCAN_RESULT_OK)
        client.endFile(path);
    return result;
}

MediaScanResult MediaScanner::ExtractMetaData(const char* path,MediaScannerClient& client)
{
    sp<DataSource> source = new FileSource(path);
    sp<MediaExtractor> extractor=MediaExtractor::Create(source,MEDIA_MIMETYPE_AUDIO_MPEG);
    if(extractor==NULL || !extractor->hasInited())
    {
        return MEDIA_SCAN_RESULT_ERROR;
    }
    sp<MetaData> meta=extractor->getMetaData();
    meta->putItem(TYPE_C_STRING,KeyPATH,path,strlen(path));
    client.postTag(meta);
    return MEDIA_SCAN_RESULT_OK;
}

