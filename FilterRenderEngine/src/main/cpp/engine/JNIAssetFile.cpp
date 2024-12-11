//
// Created by Joe.Bi on 2024/11/28.
//

#include "JNIAssetFile.h"
#include <errno.h>
#include <android/asset_manager.h>

#ifdef __cplusplus
extern "C" {
#endif

static AAssetManager* g_android_asset_manager = nullptr;

void setAssetManager(void* manager)
{
    if(g_android_asset_manager != manager)
        g_android_asset_manager = (AAssetManager*)manager;
}

void* getAssetManager(void)
{
    return g_android_asset_manager;
}

static int android_read(void* cookie, char* buf, int size)
{
    return AAsset_read((AAsset*)cookie, buf, size);
}

static int android_write(void* cookie, const char* buf, int size)
{
    return EACCES; // can't provide write access to the apk
}

static fpos_t android_seek(void* cookie, fpos_t offset, int whence)
{
    return AAsset_seek((AAsset*) cookie, offset, whence);
}

static int android_close(void *cookie)
{
    AAsset_close((AAsset *) cookie);
    return 0;
}

FILE* fileOpen(const char* name, const char* mode, uint64_t* fileSize)
{
    if (mode[0] == 'w')
        return nullptr;

    AAsset* asset = AAssetManager_open(g_android_asset_manager, name, 0);
    if (!asset)
        return nullptr;

    if(fileSize)
        *fileSize = AAsset_getLength(asset);

    return funopen(asset, android_read, android_write, android_seek, android_close);
}

uint64_t fileRead(FILE* mFile, long* readPos, void* buffer, uint32_t size, uint32_t count)
{
    uint64_t r = 0;
    if (mFile)
    {
        fseek((FILE*)mFile, readPos ? *readPos : 0, SEEK_SET);
        r = fread(buffer, size, count, (FILE*)mFile);
        if(readPos)
            *readPos = ftell((FILE*)mFile);
    }

    return r;
}

char* fileGets(FILE* mFile, long* readPos, char* buffer, int32_t bufferSize)
{
    char* r = nullptr;
    if (mFile)
    {
        fseek((FILE*)mFile, readPos ? *readPos : 0, SEEK_SET);
        r = fgets(buffer, bufferSize, (FILE*)mFile);
        if(readPos)
            *readPos = ftell((FILE*)mFile);
    }
    return r;
}


void fileClose(FILE* mFile)
{
    if(mFile)
        fclose(mFile);
}

#ifdef __cplusplus
}
#endif