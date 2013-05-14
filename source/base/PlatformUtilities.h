//
// PlatformUtilities.h - MrsWatson
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

#ifndef MrsWatson_PlatformInfo_h
#define MrsWatson_PlatformInfo_h

#include "base/CharString.h"
#include "base/LinkedList.h"
#include "base/Types.h"

#if LINUX || MACOSX
#define UNIX 1
#endif

// Substitutes for POSIX functions not found on Windows
#if WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define strcasecmp _stricmp
#define strdup _strdup
#define unlink _unlink
#define snprintf _snprintf
#define isatty _isatty
#define chdir _chdir
#define unlink _unlink
#elif MACOSX
#include <CoreFoundation/CFBundle.h>
#elif LINUX
#include <strings.h>
#endif

#if MACOSX
typedef CFBundleRef LibraryHandle;
#elif LINUX
typedef void* LibraryHandle;
#elif WINDOWS
typedef HMODULE LibraryHandle;
#else
typedef void* LibraryHandle;
#endif

typedef enum {
  PLATFORM_UNSUPPORTED,
  PLATFORM_MACOSX,
  PLATFORM_WINDOWS,
  PLATFORM_LINUX,
  NUM_PLATFORMS
} PlatformType;

PlatformType getPlatformType(void);
CharString getPlatformName(void);
const char* getShortPlatformName(void);

CharString getExecutablePath(void);
CharString getCurrentDirectory(void);

boolByte isHost64Bit(void);
boolByte isHostLittleEndian(void);

unsigned short flipShortEndian(const unsigned short value);
unsigned short convertBigEndianShortToPlatform(const unsigned short value);
unsigned int convertBigEndianIntToPlatform(const unsigned int value);
unsigned int convertLittleEndianIntToPlatform(const unsigned int value);
float convertBigEndianFloatToPlatform(const float value);

unsigned short convertByteArrayToUnsignedShort(const byte* value);
unsigned int convertByteArrayToUnsignedInt(const byte* value);

#endif
