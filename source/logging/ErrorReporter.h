//
// ErrorReporter.h - MrsWatson
// Created by Nik Reiman on 9/22/12.
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

#ifndef MrsWatson_ErrorReporter_h
#define MrsWatson_ErrorReporter_h

#include "base/CharString.h"
#include "plugin/PluginChain.h"

typedef struct {
  CharString reportName;
  CharString reportDirPath;
  CharString desktopPath;
  boolByte started;
  boolByte completed;
} ErrorReporterMembers;
typedef ErrorReporterMembers* ErrorReporter;

ErrorReporter newErrorReporter(void);
void errorReporterInitialize(ErrorReporter self);

void errorReporterCreateLauncher(ErrorReporter self, int argc, char* argv[]);
void errorReporterRemapPath(ErrorReporter self, CharString path);
boolByte errorReportCopyFileToReport(ErrorReporter self, CharString path);
boolByte errorReporterShouldCopyPlugins(void);
boolByte errorReporterCopyPlugins(ErrorReporter self, PluginChain pluginChain);

void errorReporterClose(ErrorReporter self);
void freeErrorReporter(ErrorReporter self);

#endif
