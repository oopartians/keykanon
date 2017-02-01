#include "lyricsmgr.h"


bool sortalgo(lyrics* a, lyrics* b){
	return a->time<b->time;
}

lyricsmgr::lyricsmgr(void)
{
}


lyricsmgr::~lyricsmgr(void)
{
	if(isconnected){
		lyricsclosesocket();
		socketdisconnect();
	}
	lyvectorcleanup();
}


void lyricsmgr::init(){
	wcscpy_s(filename, 1024, L"");
	int nowtime=-1;
	int prevtime=-1;
	int lastsendednumber=-1;

	vector<lyrics*> lyvector;

	isconnected=false;


	setdefault();
	
	if(usage){
		if(socketconnect()){
			isconnected=lyricsopensocket();
		}
		sendscript(startscript);
		lyricsclosesocket();
		isconnected=lyricsopensocket();
	}
}

void lyricsmgr::setfilename(wchar_t* _filename){
	bool doflag=true;
	if(wcslen(filename)==0){
		doflag=true;
	}
	else{
		if(wcscmp(filename, _filename)!=0){
			lyvectorcleanup();
			doflag=true;
		}
		else
			doflag=false;
	}
	if(doflag){
		wcscpy_s(filename, 1024, _filename);
		wchar_t openfilename[256]=L"";
		wchar_t* truefilename=wcsrchr(filename, L'\\');
		wchar_t* trash=wcsrchr(truefilename, L'.');

		if(wcslen(lyricslocation)!=0){
			wcscpy_s(openfilename, 256, lyricslocation);
			if(trash==NULL){
				wcscat_s(openfilename, 256, truefilename);
			}
			else{
				wcsncat_s(openfilename, 256, truefilename, wcslen(truefilename)-wcslen(trash));
				wcscat_s(openfilename, 256, L".txt");
			}
		}
		else{
			if(trash==NULL){
				wcscpy_s(openfilename, 256, _filename);
			}
			else{
				wcsncpy_s(openfilename, 256, _filename, wcslen(_filename)-wcslen(trash));
				wcscat_s(openfilename, 256, L".txt");
			}
		}
		FILE* fp;
		_wfopen_s(&fp, openfilename, L"r");
		if(fp!=NULL){
			lastsendednumber=-1;
			char buf[128];
			while(true){
				if(fgets(buf, sizeof(buf), fp)==0)
					break;
				if(buf[strlen(buf)-1]=='\n')
					buf[strlen(buf)-1]=0;
				if(strlen(buf)==0)
					continue;
				lyrics* _lyrics=new lyrics;
				char* script=strchr(buf, ' ');
				strcpy_s(_lyrics->script, 128, script+1);
				
				int _time;
				sscanf_s(buf, "%d%*s", &_time);
				_lyrics->time=_time;

				lyvector.push_back(_lyrics);
			}
			sort(lyvector.begin(), lyvector.end(), sortalgo);
		}
		if(fp)
			fclose(fp);
	}
}

char* lyricsmgr::getlyrics(int time){
	setnowtime(time);
	int _time=nowtime+offset;
	int res=lastsendednumber;
	int istart=lastsendednumber<0?0:lastsendednumber;
	for(int i=istart;i<lyvector.size();i++){
		if(i==lastsendednumber)
			continue;
		else if(_time<0)
			return NULL;
		if(lyvector[i]->time<_time){
			res=i;
		}
		else
			break;
	}
	if(res==lastsendednumber)
		return NULL;
	else if(res<0)
		return NULL;
	else{
		lastsendednumber=res;
		if(lyvector[res]->time>prevtime+offset)
			return lyvector[res]->script;
		else
			return NULL;
	}
}

void lyricsmgr::setnowtime(int time){
	if(prevtime>time){
		lastsendednumber=-1;
		prevtime=time;
	}
	else
		prevtime=nowtime;
	nowtime=time;
}

void lyricsmgr::lyvectorcleanup(){
	for(int i=0;i<lyvector.size();i++){
		delete lyvector[i];
	}
	lyvector.clear();
}

void lyricsmgr::setdefault(){
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
}


bool lyricsmgr::socketconnect(){
	if(WSAStartup(MAKEWORD(2,2), &wsadata)!=0)
		return false;

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=inet_addr(ip);
	servaddr.sin_port=htons(port);
	return true;
}

bool lyricsmgr::lyricsopensocket(){
	hsocket=socket(PF_INET, SOCK_STREAM, 0);

	if(hsocket==INVALID_SOCKET)
		return false;
	if(connect(hsocket, (SOCKADDR*)&servaddr, sizeof(servaddr))==SOCKET_ERROR)
		return false;

	return true;
}

void lyricsmgr::sendscript(char* script){
	if(isconnected){
		char src[512]="";

		char* version="1.4";
		char* sender=PROGRAM_NAME;
		char* targetghost="";


		MakeSstpMsg(src, sizeof(src), version, sender, script);

		send(hsocket, src, strlen(src), 0);
	}
}

void lyricsmgr::lyricsclosesocket(){
	closesocket(hsocket);
}

void lyricsmgr::socketdisconnect(){
	WSACleanup();
}

void lyricsmgr::sendlyrics(int time){
	char* script=getlyrics(time);
	if(script!=NULL){
		sendscript(script);
		lyricsclosesocket();
		isconnected=lyricsopensocket();
	}
}

void lyricsmgr::setcloseevent(){
	if(usage)
		sendscript(endscript);
	if(isconnected){
		lyricsclosesocket();
		socketdisconnect();
	}
	usage=false;
}




void MakeSstpMsg(char* dest, size_t destsize, char* version, char* sender, char* script, char* targetghost){
	char sendsstp[64]="SEND SSTP/";
	char senderstr[64]="\r\nSender: ";
	char ifghoststr[64]="\r\nIfGhost";
	char scriptstr[64]="\r\nScript: ";
	char lastscript[64]="\r\nOption: nontranslate\r\n";

	char modifiedscript[128]="";
	int scriptlen=strlen(script);
	strcat_s(modifiedscript, sizeof(modifiedscript), script);
	if(scriptlen>=2){
		if(script[scriptlen-1]!='e'||script[scriptlen-2]!='\\'){
			strcat_s(modifiedscript, sizeof(modifiedscript), "\\e");
		}
	}
	else{
		strcat_s(modifiedscript, sizeof(modifiedscript), "\\e");
	}
	
	strcat_s(dest, destsize, sendsstp);
	strcat_s(dest, destsize, version);
	strcat_s(dest, destsize, senderstr);
	strcat_s(dest, destsize, sender);
	if(strlen(targetghost)!=0){
		strcat_s(dest, destsize, ifghoststr);
		strcat_s(dest, destsize, targetghost);
	}
	strcat_s(dest, destsize, scriptstr);
	strcat_s(dest, destsize, modifiedscript);
	strcat_s(dest, destsize, lastscript);
}