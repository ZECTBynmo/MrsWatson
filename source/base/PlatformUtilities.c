//
// PlatformUtilities.c - MrsWatson
// Created by Nik Reiman on 1/2/12.
// Copyright (c) 2012 Teragon Audio. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "base/FileUtilities.h"
#include "base/PlatformUtilities.h"
#include "logging/EventLogger.h"

#if WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif UNIX
#include <dirent.h>
#include <unistd.h>
#if MACOSX
#include <CoreServices/CoreServices.h>
#include <mach-o/dyld.h>
#elif LINUX
#define LSB_DISTRIBUTION "DISTRIB_DESCRIPTION"
#include <sys/utsname.h>
#include <errno.h>
#endif
#endif

PlatformType getPlatformType() {
#if MACOSX
  return PLATFORM_MACOSX;
#elif WINDOWS
  return PLATFORM_WINDOWS;
#elif LINUX
  return PLATFORM_LINUX;
#else
  return PLATFORM_UNSUPPORTED;
#endif
}

const char* getShortPlatformName(void) {
#if MACOSX
  return "Mac OS X";
#elif WINDOWS
  if(isHost64Bit()) {
    return "Windows 64-bit";
  }
  else {
    return "Windows 32-bit";
  }
#elif LINUX
  if(isHost64Bit()) {
    return "Linux-x86_64";
  }
  else {
    return "Linux-i686";
  }
#else
  return "Unsupported";
#endif
}

CharString getPlatformName(void) {
  CharString result = newCharString();
#if MACOSX
  SInt32 major, minor, bugfix;
  Gestalt(gestaltSystemVersionMajor, &major);
  Gestalt(gestaltSystemVersionMinor, &minor);
  Gestalt(gestaltSystemVersionBugFix, &bugfix);
  snprintf(result->data, result->length, "Mac OS X %ld.%ld.%ld", major, minor, bugfix);
#elif LINUX
  CharString line = newCharString();
  char *lineDelimiter = NULL;
  char *distributionStringStart = NULL;
  char *distributionStringEnd = NULL;
  CharString distributionName = newCharString();
  struct utsname systemInfo;
  FILE *lsbRelease = NULL;

  if(uname(&systemInfo) != 0) {
    logWarn("Could not get system information from uname");
    charStringCopyCString(result, "Linux (Unknown platform)");
    freeCharString(distributionName);
    freeCharString(line);
    return result;
  }
  charStringCopyCString(distributionName, "(Unknown distribution)");

  if(fileExists("/etc/lsb-release")) {
    lsbRelease = fopen("/etc/lsb-release", "r");
    if(lsbRelease != NULL) {
      while(fgets(line->data, line->length, lsbRelease) != NULL) {
        lineDelimiter = strchr(line->data, '=');
        if(lineDelimiter != NULL) {
          if(!strncmp(line->data, LSB_DISTRIBUTION, strlen(LSB_DISTRIBUTION))) {
            distributionStringStart = strchr(lineDelimiter + 1, '"');
            if(distributionStringStart != NULL) {
              distributionStringEnd = strchr(distributionStringStart + 1, '"');
              if(distributionStringEnd != NULL) {
                charStringClear(distributionName);
                strncpy(distributionName->data, distributionStringStart + 1,
                  distributionStringEnd - distributionStringStart - 1);
              }
            }
          }
        }
      }
    }
  }

  if(charStringIsEmpty(result)) {
    snprintf(result->data, result->length, "Linux %s, kernel %s %s",
      distributionName->data, systemInfo.release, systemInfo.machine);
  }

  fclose(lsbRelease);
  freeCharString(distributionName);
  freeCharString(line);
#elif WINDOWS
  OSVERSIONINFOEX versionInformation;
  memset(&versionInformation, 0, sizeof(OSVERSIONINFOEX));
  versionInformation.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  GetVersionEx((OSVERSIONINFO*)&versionInformation);
  // Generic string which will also work with newer versions of windows
  snprintf(result->data, result->length, "Windows %d.%d",
    versionInformation.dwMajorVersion, versionInformation.dwMinorVersion);

  // TODO: This is a bit lame, is there a better way to get the OS version name?
  switch(versionInformation.dwMajorVersion) {
    case 6:
      switch(versionInformation.dwMinorVersion) {
      case 2:
        charStringCopyCString(result, "Windows 8");
        break;
      case 1:
        charStringCopyCString(result, "Windows 7");
        break;
      case 0:
        charStringCopyCString(result, "Windows Vista");
        break;
      }
      break;
    case 5:
      switch(versionInformation.dwMinorVersion) {
      case 2:
        charStringCopyCString(result, "Windows Server 2003");
        break;
      case 1:
        charStringCopyCString(result, "Windows XP");
        break;
      case 0:
        charStringCopyCString(result, "Windows 2000");
        break;
      }
      break;
  }
#else
  charStringCopyCString(result, "Unsupported platform");
#endif
  return result;
}

CharString getExecutablePath(void) {
  CharString executablePath = newCharString();
#if LINUX
  ssize_t result = readlink("/proc/self/exe", executablePath->data, executablePath->length);
  if(result < 0) {
    logWarn("Could not find executable path, error '%s'", strerror(errno));
    return NULL;
  }
#elif MACOSX
  _NSGetExecutablePath(executablePath->data, (uint32_t*)&executablePath->length);
#elif WINDOWS
  GetModuleFileNameA(NULL, executablePath->data, executablePath->length);
#endif
  return executablePath;
}

CharString getCurrentDirectory(void) {
  CharString currentDirectory = newCharString();
#if UNIX
  charStringCopyCString(currentDirectory, getenv("PWD"));
#elif WINDOWS
  GetCurrentDirectoryA(currentDirectory->length, currentDirectory->data);
#endif
  return currentDirectory;
}

boolByte isHost64Bit(void) {
  return (sizeof(void*) == 8);
}

boolByte isHostLittleEndian(void) {
  int num = 1;
  boolByte result = (*(char*)&num == 1);
  return result;
}

unsigned short flipShortEndian(const unsigned short value) {
  return ((value << 8) & 0x0000ff00) + ((value >> 8) & 0x000000ff);
}

unsigned short convertBigEndianShortToPlatform(const unsigned short value) {
  if(isHostLittleEndian()) {
    return (value << 8) | (value >> 8);
  }
  else {
    return value;
  }
}

unsigned int convertBigEndianIntToPlatform(const unsigned int value) {
  if(isHostLittleEndian()) {
    return (value << 24) | ((value << 8) & 0x00ff0000) | ((value >> 8) & 0x0000ff00) | (value >> 24);
  }
  else {
    return value;
  }
}

unsigned int convertLittleEndianIntToPlatform(const unsigned int value) {
  if(!isHostLittleEndian()) {
    return (value << 24) | ((value << 8) & 0x00ff0000) | ((value >> 8) & 0x0000ff00) | (value >> 24);
  }
  else {
    return value;
  }
}

unsigned short convertByteArrayToUnsignedShort(const byte* value) {
  if(isHostLittleEndian()) {
    return ((value[1] << 8) & 0x0000ff00) | value[0];
  }
  else {
    return ((value[0] << 8) & 0x0000ff00) | value[1];
  }
}

unsigned int convertByteArrayToUnsignedInt(const byte* value) {
  if(isHostLittleEndian()) {
    return ((value[3] << 24) | ((value[2] << 16) & 0x00ff0000) |
      ((value[1] << 8) & 0x0000ff00) | value[0]);
  }
  else {
    return ((value[0] << 24) | ((value[1] << 16) & 0x00ff0000) |
      ((value[2] << 8) & 0x0000ff00) | value[0]);
  }
}

float convertBigEndianFloatToPlatform(const float value) {
  float result = 0.0f;
  byte* floatToConvert = (byte*)&value;
  byte* floatResult = (byte*)&result;
  floatResult[0] = floatToConvert[3];
  floatResult[1] = floatToConvert[2];
  floatResult[2] = floatToConvert[1];
  floatResult[3] = floatToConvert[0];
  return result;
}
