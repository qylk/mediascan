#include "MediaScannerClient.h"
MediaScannerClient::MediaScannerClient()
{

}

MediaScannerClient::~MediaScannerClient()
{

}

void MediaScannerClient::beginFile(const char*file)
{

}

void MediaScannerClient::endFile(const char*file)
{
    postFile(file);
}


