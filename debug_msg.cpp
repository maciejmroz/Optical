
#include "debug_msg.h"
#include <stdio.h>
#include <stdarg.h>
#include <direct.h>
#ifdef _WIN32
#include<windows.h>
#endif

static int frame_num=0;

void DebugSetup() {
	frame_num=0;
	_mkdir("logs");
	FILE* f;
	f=fopen("logs/debug.log","w");
	fclose(f);
}

void DebugFileSetup(const char* filename) {
	char name_buf[1024];
	name_buf[0]='\0';
	strcat(name_buf,"logs/");
	strcat(name_buf,filename);
	FILE* f;
	f=fopen(name_buf,"w");
	fclose(f);
}
void DebugFrameSetup() {
	char num_buf[20];
	char name_buf[20];
	itoa(frame_num,num_buf,10);
	name_buf[0]='\0';
	strcat(name_buf,"logs/frame");
	strcat(name_buf,num_buf);
	strcat(name_buf,".log");
	FILE* f;
	f=fopen(name_buf,"w");
	fclose(f);
}

void DebugMessage(const char* format, ...) {
	FILE *f;
	va_list arg_list;
	va_start(arg_list,format);
	f=fopen("logs/debug.log","a");
	vfprintf(f,format,arg_list);
	fclose(f);
	va_end(arg_list);
}
void DebugFileMessage(const char* filename,const char* format, ...) {
	char name_buf[1024];
	name_buf[0]='\0';
	strcat(name_buf,"logs/");
	strcat(name_buf,filename);
	FILE *f;
	va_list arg_list;
	va_start(arg_list,format);
	f=fopen(name_buf,"a");
	vfprintf(f,format,arg_list);
	fclose(f);
	va_end(arg_list);
}

void DebugFrameMessage(const char* format, ...) {
	char num_buf[20];
	char name_buf[20];
	itoa(frame_num,num_buf,10);
	name_buf[0]='\0';
	strcat(name_buf,"logs/frame");
	strcat(name_buf,num_buf);
	strcat(name_buf,".log");
	FILE *f;
	va_list arg_list;
	va_start(arg_list,format);
	f=fopen(name_buf,"a");
	vfprintf(f,format,arg_list);
	fclose(f);
	va_end(arg_list);
}

#ifdef _WIN32
extern "C++" HWND win_handle;
#endif

void AppCriticalError(const char* format, ...) {
#ifdef _WIN32
	va_list arg_list;
	va_start(arg_list,format);
	PrintfBox(format,arg_list);
	va_end(arg_list);
    PostMessage(win_handle, WM_CLOSE, 0, 0);
#else
	//to be done !!!!!!!!!!!!!!!!!!!!!!!!!!!!1
#endif
}

void DebugSetFrame(int frame) {
	frame_num=frame;
}

int DebugGetFrame() {
	return frame_num;
}

#ifdef _WIN32
void PrintfBox(const char* format, ...) {
	char buffer[4096];
	va_list arg_list;
	va_start(arg_list,format);
	vsprintf(buffer,format,arg_list);
	va_end(arg_list);
	MessageBox(NULL,buffer,"Message",MB_TASKMODAL);
}
#endif