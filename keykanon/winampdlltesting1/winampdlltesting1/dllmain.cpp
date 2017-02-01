/*
 
Winamp generic plugin template code.
This code should be just the basics needed to get a plugin up and running.
You can then expand the code to build your own plugin.
 
Updated details compiled June 2009 by culix, based on the excellent code examples
and advice of forum members Kaboon, kichik, baafie, burek021, and bananskib.
Thanks for the help everyone!
 
*/
 
#include <windows.h>
#include "dllmain.h"

 
 
// these are callback functions/events which will be called by Winamp
int  init(void);
void config(void);
void quit(void);
 
// this structure contains plugin information, version, name...
// GPPHDR_VER is the version of the winampGeneralPurposePlugin (GPP) structure
winampGeneralPurposePlugin plugin = {
  GPPHDR_VER,  // version of the plugin, defined in "gen_myplugin.h"
  PLUGIN_NAME, // name/title of the plugin, defined in "gen_myplugin.h"
  init,        // function name which will be executed on init event
  config,      // function name which will be executed on config event
  quit,        // function name which will be executed on quit event
  0,           // handle to Winamp main window, loaded by winamp when this dll is loaded
  0            // hinstance to this dll, loaded by winamp when this dll is loaded
};
 
// event functions follow

lyricsmgr* _lyricsmgr=NULL;

int init() {
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(plugin.hDllInstance, path, MAX_PATH);
	PathRemoveFileSpecW(path);
	PathAppendW(path, L"in_mp3.dll");

	in_mp3_lib = LoadLibraryW(path);
	if (in_mp3_lib)
	{
		PluginGetter pluginGetter = (PluginGetter)GetProcAddress(in_mp3_lib, "winampGetInModule2");
		if (pluginGetter)
		{
			in_mp3 = pluginGetter();
		}
	}
	if(in_mp3){
	  _lyricsmgr=new lyricsmgr();
	  _lyricsmgr->init();
	}
	initthreadmgr();
  return 0;
}
 
void config() {

}

 
void quit() {

	if(_lyricsmgr)
		_lyricsmgr->setcloseevent();
	WaitForSingleObject(thread_handle, INFINITE);
	if(_lyricsmgr)
		delete _lyricsmgr;
	if (in_mp3_lib)
		FreeLibrary(in_mp3_lib);
	in_mp3_lib=0;
	in_mp3=0;
}
 
extern "C" __declspec(dllexport) winampGeneralPurposePlugin * winampGetGeneralPurposePlugin() {
  return &plugin;
}

////////////////////////////////
//
//
// 여기까지는 winamp sdk에서 복붙함
//
//
////////////////////////////////



void initthreadmgr()
{
	thread_handle = (HANDLE) CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) threadrun,NULL,0,&threadid);
}


void endthreadmgr(void)
{
	
		if (in_mp3_lib)
		FreeLibrary(in_mp3_lib);
	in_mp3_lib=0;
	in_mp3=0;
}


DWORD WINAPI threadrun(LPVOID _lpparam){
	while(_lyricsmgr){
		if(!_lyricsmgr->getusage())
			break;
		threadfunc(_lpparam);
		Sleep(17);
	}
	return 0;
}

int threadfunc(LPVOID _lpparam){
	wchar_t* fname=NULL;
	int nowtime;
	if(in_mp3){
		if(in_mp3->is_seekable!=0){
			fname=(wchar_t*)SendMessage(plugin.hwndParent, WM_WA_IPC, 0, IPC_GET_PLAYING_FILENAME);
			if(fname!=NULL)
				_lyricsmgr->setfilename(fname);
			nowtime=in_mp3->outMod->GetOutputTime();
			_lyricsmgr->sendlyrics(nowtime);
		}
	}
	return 0;
}



