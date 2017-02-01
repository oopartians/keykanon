#include <Windows.h>
#include <stdio.h>

#include "dllmain.h"

HWND hwnd1;
HWND hwnd2;
HWND hwnd3;
HWND hwnd4;
HWND hwnd5;
HWND hwnd6;
HWND hwnd7;
HWND hwnd8;

bool usage;
char ip[32]="";
int port;
wchar_t lyricslocation[1024]=L"";
char startscript[128]="";
char endscript[128]="";
int offset;
bool offsetisminus;

bool saveflag=false;
bool closeflag=false;

wchar_t portstr[32]=L"";
wchar_t offsetstr[32]=L"";

void setdefault(){
	char* padds;
	size_t sadds;
	_dupenv_s(&padds, &sadds, "AppData");
	char path[1024]="";
	strcpy_s(path, sizeof(path), padds);
	strcat_s(path, sizeof(path), "\\Winamp\\Plugins\\keykanon\\keykanon.dat");
	FILE* fp;
	fopen_s(&fp, path, "rb");
	int notcrash=1;
	if(fp==NULL){
		notcrash=0;
	}
	else{
		if(notcrash!=0)
			notcrash=fread(&usage, sizeof(bool), 1, fp);
		if(notcrash!=0)
			notcrash=fread(ip, sizeof(char), 32, fp);
		if(notcrash!=0)
			notcrash=fread(&port, sizeof(int), 1, fp);
		if(notcrash!=0)
			notcrash=fread(lyricslocation, sizeof(wchar_t), 1024, fp);
		if(notcrash!=0)
			notcrash=fread(startscript, sizeof(char), 128, fp);
		if(notcrash!=0)
			notcrash=fread(endscript, sizeof(char), 128, fp);
		if(notcrash!=0)
			notcrash=fread(&offset, sizeof(int), 1, fp);
	}
	if(notcrash==0){
		usage=defaultusage;
		strcpy_s(ip, 32, defaultip);
		port=defaultport;
		wcscpy_s(lyricslocation, 1024, defaultlyricslocation);
		strcpy_s(startscript, 128, defaultstartscript);
		strcpy_s(endscript, 128, defaultendscript);
		offset=defaultoffset;
	}
	if(fp)
		fclose(fp);
	if(offset<0){
		offset=-offset;
		offsetisminus=true;
	}
	else
		offsetisminus=false;
}

void savedefault(){

	if(SendMessage(hwnd1, BM_GETCHECK, 0, 0)==BST_CHECKED)
		usage=true;
	else
		usage=false;
	GetWindowTextA(hwnd2, ip, 32);
	GetWindowText(hwnd3, portstr, 32);
	GetWindowText(hwnd4, lyricslocation, 1024);
	GetWindowTextA(hwnd5, startscript, 128);
	GetWindowTextA(hwnd6, endscript, 128);
	GetWindowText(hwnd7, offsetstr, 32);

	int k;
	if(swscanf_s(portstr, L"%d", &k)>0)
		port=k;
	if(swscanf_s(offsetstr, L"%d", &k)>0)
		offset=k;
	bool _offsetisminus;
	if(SendMessage(hwnd8, BM_GETCHECK, 0, 0)==BST_CHECKED)
		_offsetisminus=true;
	else
		_offsetisminus=false;
	if(_offsetisminus)
		offset=-offset;
	char* padds;
	size_t sadds;
	_dupenv_s(&padds, &sadds, "AppData");
	char path[1024]="";
	strcpy_s(path, sizeof(path), padds);
	strcat_s(path, sizeof(path), "\\Winamp\\Plugins\\keykanon\\keykanon.dat");
	FILE* fp;
	fopen_s(&fp, path, "wb");
	if(fp==NULL){
		system("mkdir %Appdata%\\Winamp\\Plugins\\keykanon");
	fopen_s(&fp, path, "wb");
		if(fp==NULL)
			return;
	}
	fwrite(&usage, sizeof(bool), 1, fp);
	fwrite(ip, sizeof(char), 32, fp);
	fwrite(&port, sizeof(int), 1, fp);
	fwrite(lyricslocation, sizeof(wchar_t), 1024, fp);
	fwrite(startscript, sizeof(char), 128, fp);
	fwrite(endscript, sizeof(char), 128, fp);
	fwrite(&offset, sizeof(int), 1, fp);
	if(fp)
		fclose(fp);
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
	case WM_DESTROY:
		if(saveflag)
			savedefault();
		PostQuitMessage(0);
		return 0;
	case WM_CREATE:
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case 1:
			saveflag=true;
			closeflag=true;
			break;
		case 2:
			closeflag=true;
			break;
		default:
			break;
		}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT){
	WNDCLASSEX wc={sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, hInst, NULL, LoadCursor(NULL, IDC_ARROW), (HBRUSH)LTGRAY_BRUSH, 
		NULL, L"keykanon", NULL};
	RegisterClassEx(&wc);

	HWND hWnd=CreateWindow(L"keykanon", L"keykanon", WS_OVERLAPPED|WS_MINIMIZEBOX|WS_SYSMENU, 0, 0, 200, 100, NULL, NULL, wc.hInstance, NULL);
		
	RECT rect;
	GetClientRect(hWnd, &rect);
	int x=550-rect.right;
	int y=590-rect.bottom;
	HDC hdc=GetDC(NULL);
	int dx=GetDeviceCaps(hdc, HORZRES);
	int dy=GetDeviceCaps(hdc, VERTRES);

	dx=(dx-x)/2;
	dy=(dy-y)/2;

	SetWindowPos(hWnd, HWND_NOTOPMOST, dx, dy, x, y, SWP_NOZORDER);
	ReleaseDC(hWnd, hdc);

	setdefault();


	
	wsprintf(portstr, L"%d", port);
	wsprintf(offsetstr, L"%d", offset);
	
//	CreateWindow(L"edit",L"Click Me",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,	20,20,100,25,hWnd,(HMENU)0,wc.hInstance,NULL);

	CreateWindow(L"static",L"사용",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|ES_CENTER|WS_BORDER,	25,25,45,25,hWnd,(HMENU)0,wc.hInstance,NULL);
	hwnd1=CreateWindow(L"button",L"",WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,75,28,14,15,hWnd,(HMENU)0,wc.hInstance,NULL);
	if(usage){
		SendMessage(hwnd1, BM_SETCHECK, BST_CHECKED, 0);
	}
	else{
		SendMessage(hwnd1, BM_SETCHECK, BST_UNCHECKED, 0);
	}

	CreateWindow(L"static",L"IP",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|ES_CENTER|WS_BORDER,	25,60,45,25,hWnd,(HMENU)0,wc.hInstance,NULL);
	hwnd2=CreateWindowA("edit",ip,WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|WS_BORDER,	75,60,100,25,hWnd,(HMENU)0,wc.hInstance,NULL);

	CreateWindow(L"static",L"Port",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|ES_CENTER|WS_BORDER,	25,95,45,25,hWnd,(HMENU)0,wc.hInstance,NULL);
	hwnd3=CreateWindow(L"edit",portstr,WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|ES_NUMBER|WS_BORDER,	75,95,100,25,hWnd,(HMENU)0,wc.hInstance,NULL);

	CreateWindow(L"static",L"Offset",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|ES_CENTER|WS_BORDER,	25,130,45,25,hWnd,(HMENU)0,wc.hInstance,NULL);
	hwnd7=CreateWindow(L"edit",offsetstr,WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|ES_NUMBER|WS_BORDER,	75,130,100,25,hWnd,(HMENU)0,wc.hInstance,NULL);
	
	hwnd8=CreateWindow(L"button",L"느리게",WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,180,135,70,15,hWnd,(HMENU)0,wc.hInstance,NULL);
	if(offsetisminus){
		SendMessage(hwnd8, BM_SETCHECK, BST_CHECKED, 0);
	}
	else{
		SendMessage(hwnd8, BM_SETCHECK, BST_UNCHECKED, 0);
	}

	CreateWindow(L"static",L"가사 위치 (없음:음악파일 위치와 같음)",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|WS_BORDER|ES_CENTER,	25,190,300,25,hWnd,(HMENU)0,wc.hInstance,NULL);
	hwnd4=CreateWindow(L"edit",lyricslocation,WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|ES_AUTOHSCROLL|WS_BORDER,	25,225,300,25,hWnd,(HMENU)0,wc.hInstance,NULL);

	CreateWindow(L"static",L"시작 스크립트",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|WS_BORDER|ES_CENTER,	25,270,150,25,hWnd,(HMENU)0,wc.hInstance,NULL);
	hwnd5=CreateWindowA("edit",startscript,WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|ES_AUTOHSCROLL|WS_BORDER,	25,305,300,25,hWnd,(HMENU)0,wc.hInstance,NULL);

	CreateWindow(L"static",L"종료 스크립트",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|WS_BORDER|ES_CENTER,	25,350,150,25,hWnd,(HMENU)0,wc.hInstance,NULL);
	hwnd6=CreateWindowA("edit",endscript,WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON|ES_AUTOHSCROLL|WS_BORDER,	25,385,300,25,hWnd,(HMENU)0,wc.hInstance,NULL);

	
	CreateWindow(L"button",L"OK",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 100,440,100,25,hWnd,(HMENU)1,wc.hInstance,NULL); 
	CreateWindow(L"button",L"Cancel",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 225,440,100,25,hWnd,(HMENU)2,wc.hInstance,NULL); 

	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);



	////////////////

//	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

//	_CrtSetBreakAlloc(191);

	//////////
	/////

	MSG msg;
	msg.message = 0;

	while(msg.message!=WM_QUIT){
		if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if(closeflag)
			break;
		Sleep(17);
	}
	if(saveflag)
		savedefault();
	

	UnregisterClass(L"keykanon", wc.hInstance);

	return 0;
}
