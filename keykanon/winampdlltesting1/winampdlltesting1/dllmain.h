#pragma once

#ifndef gen_myplugin_h
//---------------------------------------------------------------------------
#define gen_myplugin_h

#include <windows.h>
#include <stdio.h>
 
#include <Winamp/IN2.H>
#include <Winamp/wa_ipc.h>

#include <shlwapi.h>

#include "lyricsmgr.h"

#pragma comment(lib, "shlwapi.lib")
// plugin version (don't touch this)
#define GPPHDR_VER 0x10

#define PLUGIN_NAME "Kanon winamp SSTP plugin"
 
// plugin name/title (change this to something you like)

 HMODULE in_mp3_lib = 0;
In_Module *in_mp3 = 0;

typedef In_Module *(*PluginGetter)();
 
// main structure with plugin information, version, name...
typedef struct {
  int version;                   // version of the plugin structure
  char *description;             // name/title of the plugin 
  int (*init)();                 // function which will be executed on init event
  void (*config)();              // function which will be executed on config event
  void (*quit)();                // function which will be executed on quit event
  HWND hwndParent;               // hwnd of the Winamp client main window (stored by Winamp when dll is loaded)
  HINSTANCE hDllInstance;        // hinstance of this plugin DLL. (stored by Winamp when dll is loaded) 
} winampGeneralPurposePlugin;
 


HANDLE thread_handle;
DWORD threadid;

void initthreadmgr();

void endthreadmgr(void);

DWORD WINAPI threadrun(LPVOID _lpparam);

int threadfunc(LPVOID _lpparam);


bool defaultusage=true;
char defaultip[32]="127.0.0.1";
int defaultport=9801;

wchar_t defaultlyricslocation[1024]=L"";	
	
char defaultstartscript[128]="\\0\\s0\\시작해요\\e";
char defaultendscript[128]="\\0\\s0\\끝\\e";

int defaultoffset=0;


 
#endif //gen_myplugin_h