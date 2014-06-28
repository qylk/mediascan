#ifndef MEDIASCANNER_H
#define MEDIASCANNER_H
#include "MediaScannerClient.h"
enum MediaScanResult
{
    MEDIA_SCAN_RESULT_OK,
    MEDIA_SCAN_RESULT_SKIPPED,
    MEDIA_SCAN_RESULT_ERROR
};

class MediaScanner
{
public:
    MediaScanner();
    virtual ~MediaScanner();

    virtual MediaScanResult processFile(
        const char *path, MediaScannerClient &client) ;

    virtual MediaScanResult processDirectory(
        const char *path, MediaScannerClient &client);

private:
    MediaScanResult doProcessDirectory(
        char *path, int pathRemaining, MediaScannerClient &client, bool noMedia);
    MediaScanResult doProcessDirectoryEntry(
        char *path, int pathRemaining, MediaScannerClient &client, bool noMedia,
        struct dirent* entry, char* fileSpot);

    MediaScanResult ExtractMetaData( const char* path,MediaScannerClient& client);
    MediaScanner(const MediaScanner &);
    MediaScanner &operator=(const MediaScanner &);
};

#endif // MEDIASCANNER_H
