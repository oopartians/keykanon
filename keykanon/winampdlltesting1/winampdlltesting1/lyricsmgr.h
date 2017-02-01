#pragma once

/******************************

파일을 읽고, 현재 시간에서 가장 가까운 스크립트를 전송하는 클래스이다.
sstp를 사용해 전송한다.

******************************/

#include <vector>
#include <algorithm>


#include <winsock.h>

#include "pluginname.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

typedef struct lyrics{
	int time;//ms
	char script[128];
}lyrics;



bool sortalgo(lyrics* a, lyrics* b);

class lyricsmgr
{
public:
	lyricsmgr(void);
	~lyricsmgr(void);

	void init();

	void setfilename(wchar_t* _filename);

	char* getlyrics(int time);

	void setnowtime(int time);

	void lyvectorcleanup();

	void setdefault();


	bool socketconnect();
	bool lyricsopensocket();
	void sendscript(char* script);
	void lyricsclosesocket();
	void socketdisconnect();

	void sendlyrics(int time);

	bool getusage(){return usage;}
	void setcloseevent();
private:
	bool usage;
	char ip[32];
	int port;

	wchar_t lyricslocation[1024];
	
	char startscript[128];
	char endscript[128];

	int offset;

	wchar_t filename[1024];
	int nowtime;
	int prevtime;
	int lastsendednumber;

	vector<lyrics*> lyvector;

	bool isconnected;
	WSADATA wsadata;
	SOCKET hsocket;
	SOCKADDR_IN servaddr;
};

void MakeSstpMsg(char* dest, size_t destsize, char* version, char* sender, char* script, char* targetghost="");