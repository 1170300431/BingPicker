#pragma once
#include "GUI.hpp"
#include <wininet.h>
#include <urlmon.h>
#include <time.h>
#include <string>
#include <regex>

#define MAXSIZE 1024
#define overtime 5000

using namespace std;

string httpGet(string);
bool download(string, string);
extern Label* notify;
extern ProgressBar* progress;

class CBindCallback : public IBindStatusCallback {
public:
	CBindCallback() {}
	~CBindCallback() {}
	//IBindStatusCallback的方法。除了OnProgress外的其他方法都返回E_NOTIMPL
	STDMETHOD(OnStartBinding)(DWORD dwReserved, IBinding __RPC_FAR *pib)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(GetPriority)(LONG __RPC_FAR *pnPriority)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(OnLowResource)(DWORD reserved)
	{ 
		return E_NOTIMPL;
	}

	//OnProgress在这里
	STDMETHOD(OnProgress)
		(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR wszStatusText) = 0;

	STDMETHOD(OnStopBinding)(HRESULT hresult, LPCWSTR szError)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(GetBindInfo)(DWORD __RPC_FAR *grfBINDF, BINDINFO __RPC_FAR *pbindinfo)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(OnDataAvailable)(DWORD grfBSCF, DWORD dwSize, FORMATETC __RPC_FAR *pformatetc, STGMEDIUM __RPC_FAR *pstgmed)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(OnObjectAvailable)(REFIID riid, IUnknown __RPC_FAR *punk)
	{
		return E_NOTIMPL;
	}
	STDMETHOD_(ULONG, AddRef)()
	{
		return 0;
	}
	STDMETHOD_(ULONG, Release)()
	{
		return 0;
	}
	STDMETHOD(QueryInterface)(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		return E_NOTIMPL;
	}
};
class myCBC : public CBindCallback {
private:
	clock_t start, end;
public:
	STDMETHOD(OnProgress)
		(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR wszStatusText) {
		end = clock();
		if (end - start > overtime) return INET_E_DOWNLOAD_FAILURE;
		if (ulProgressMax == 0) notify->name = "unknown file size.";
		else {
			progress->setRange(ulProgressMax);
			progress->setPos(ulProgress);
		}
		return S_OK;
	}

	void begin() {
		start = clock();
	}
};

string httpGet(string url) {
	string r = "";
	HINTERNET hSession = InternetOpen("UrlTest", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hSession)
	{
		HINTERNET hHttp = InternetOpenUrlA(hSession, url.c_str(), NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
		if (hHttp)
		{
			char Temp[MAXSIZE];
			ULONG Number = 1;
			while (Number > 0)
			{
				InternetReadFile(hHttp, Temp, MAXSIZE - 1, &Number);
				Temp[Number] = '\0';
				r += Temp;
			}
			InternetCloseHandle(hHttp);
			hHttp = NULL;
		}
		InternetCloseHandle(hSession);
		hSession = NULL;
	}
	return r;
}

char* replace(char* a) {
	for (unsigned int i = 0; i < strlen(a); i++) if (a[i] == '/') a[i] = '-';
	return a;
}

bool download(string url, string name) throw(string){
	myCBC cbc;
	
	name += ".jpg";
	DeleteUrlCacheEntry(url.c_str());
	notify->name = "Download started.";
	switch (URLDownloadToFile(NULL, url.c_str(), name.c_str(), 0, &cbc)) {
	case S_OK:
		cbc.begin();
		return true;
	case E_OUTOFMEMORY:
		throw string("The buffer length is invalid, or there is insufficient memory to complete the operation.");
	case INET_E_DOWNLOAD_FAILURE:
		throw string("The specified resource or callback interface was invalid.");
	default:
		throw string("Ooops...it seems that some error has occured. Error code: ");
	}
	return false;
}

string regurl(string httpD, string& name) throw(string){
	smatch rurl, rname;
	if (!std::regex_search(httpD, rurl, regex("/az.*?_1920x1080.jpg"))) throw string("Sorry. But no picture is crawled. Ask for your dev.");
	name = rurl.str();
	std::regex_search(name, rname, regex("rb/.*?_"));
	name = rname.str().substr(3);
	name.pop_back();
	return "http://cn.bing.com" + rurl.str();
}