
#pragma once
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <psapi.h>
#include <stdint.h>
#include <minwindef.h>
#include <time.h>

#define MIN(a,b) (((a)<(b))?(a):(b))

#define ASSERT(arg) if (!(arg)) { ASSError(__FILE__, __LINE__, #arg); }

static void GetLastWinErrStr(char* str, uint32_t strSize)
{
    DWORD err = GetLastError();
    if (err == 0)
        return;
    LPSTR msg = NULL;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&msg, 0, NULL);
    const uint32_t sizeToCopy = MIN(strlen(msg) + 1, strSize);
    memcpy(str, msg, sizeof(char) * sizeToCopy);
    str[strSize - 1] = '\0';
    LocalFree(msg);
}

static void ASSError(const char* file, uint32_t line, const char* assertStr)
{
    time_t mytime = time(NULL);
    char* timeStr = ctime(&mytime);
    timeStr[strlen(timeStr) - 1] = '\0';
    static char winMsg[512];
    GetLastWinErrStr(winMsg, 512);
    SetLastError(0);

    FILE* f = fopen("./AltAppSwitcherLog.txt", "ab");
    if (f == NULL)
        return;
    fprintf(f, "%s:\nFile: %s, line: %u:\n", timeStr, file, line);
    fprintf(f, "Assert: %s\n", assertStr);
    fprintf(f, "Last winapi error: %s\n\n", winMsg[0] == '\0' ? "None" : winMsg);
    fclose(f);

    printf( "Assert: %s\n", assertStr);
    printf("Last winapi error: %s\n\n", winMsg[0] == '\0' ? "None" : winMsg);

    DebugBreak();
}