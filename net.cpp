#pragma once
#include "GUI.h"
#include <wininet.h>
#include <urlmon.h>
#include <windows.h>

#include <time.h>
#define MAXSIZE 1024
#define overtime 5000
#pragma comment(lib,"urlmon.lib")
#pragma comment(lib, "Wininet.lib")
std::string httpGet(std::string);
std::string download(std::string);
extern double start, end;
extern form frmain;
class CBindCallback : public IBindStatusCallback
{
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
		(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR wszStatusText) {
		end = clock();
		if (end - start > overtime) return INET_E_DOWNLOAD_FAILURE;
		if (ulProgressMax == 0) ((Label*)frmain.tab[0])->name("unknown file size.");
		else {
			((ProgressBar*)frmain.tab[1])->setRange(ulProgressMax);
			((ProgressBar*)frmain.tab[1])->setPos(ulProgress);
		}
		return S_OK;
	}

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
std::string httpGet(std::string url) {
	std::string r = "";
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

std::string download(std::string url) {
	CBindCallback cbc;
	char da[64];
	_strdate_s(da);
	std::string filename = "./";
	filename += replace(da);
	filename += ".jpg";
	DeleteUrlCacheEntryA(url.c_str());
	((Label*)frmain.tab[0])->name("Download started.");
	switch (URLDownloadToFileA(NULL, url.c_str(), filename.c_str(), 0, &cbc)) {
	case S_OK:
		start = clock();
		filename = replace(da);
		filename += ".jpg";
		return filename;
	case E_OUTOFMEMORY:
		((Label*)frmain.tab[0])->name("The buffer length is invalid, or there is insufficient memory to complete the operation.");
		return "";
	case INET_E_DOWNLOAD_FAILURE:
		((Label*)frmain.tab[0])->name("The specified resource or callback interface was invalid.");
		return "";
	default:
		((Label*)frmain.tab[0])->name("Ooops...it seems that some error has occured. Error code: ");
	}
	return "";
}