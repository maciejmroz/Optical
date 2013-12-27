/********************************************************************************************
 Simple logging services available in both debug and release (WIN32 Hybrid Relase
 configuration) builds
 (c) 2000 Maciej Mroz
********************************************************************************************/

#ifndef _DEBUG_MSG_H_
#define _DEBUG_MSG_H_

#ifdef _LOGGED_DEBUG
#define _LOG_EVERYTHING
#endif

#ifdef _HYBRID_RELEASE
#define _LOG_EVERYTHING
#endif

void DebugSetup();
void DebugFileSetup(const char* filename);
void DebugFrameSetup();
void DebugMessage(const char* format, ...);
void DebugFileMessage(const char* filename,const char* format, ...);
void DebugFrameMessage(const char* format, ...);
void DebugSetFrame(int frame=0);
int DebugGetFrame();
void AppCriticalError(const char* format, ...);

#ifdef _WIN32
void PrintfBox(const char* format, ...);
#endif

#endif