//
// Created by Joe.Bi on 2024/11/28.
//

#ifndef __JNIAssetFile_h__
#define __JNIAssetFile_h__
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void setAssetManager(void* manager);

void* getAssetManager(void);

FILE* fileOpen(const char* name, const char* mode, uint64_t* fileSize);

uint64_t fileRead(FILE* mFile, long* readPos, void* buffer, uint32_t size, uint32_t count);

char* fileGets(FILE* mFile, long* readPos, char* buffer, int32_t bufferSize);

void fileClose(FILE* mFile);

#ifdef __cplusplus
    }
#endif

#endif //__JNIAssetFile_h__
