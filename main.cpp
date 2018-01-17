/*
* main.cpp
*
*  Created on: 2017年12月13日
*      Author: JamzumSum
*/

#include "GUI.h"
#include <windows.h>
#include <regex>
#include <wininet.h>
#include <urlmon.h>
#include "opencv2/highgui.hpp"

#define MAXSIZE 1024
#define overtime 5000
#pragma comment(lib,"urlmon.lib")
#pragma comment(lib, "Wininet.lib")

std::string httpGet(std::string);
std::string regurl(std::string);
std::string download(std::string);
void setWallpaper(std::string);

double start, end;
std::string filename;
form frmain((char*)"JamzumSum", (char*)"bingPicker");

class CBindCallback : public IBindStatusCallback
{
public:
	CBindCallback() {}
	~CBindCallback() {}
	//IBindStatusCallback的方法。除了OnProgress外的其他方法都返回E_NOTIMPL
	STDMETHOD(OnStartBinding)(DWORD dwReserved,IBinding __RPC_FAR *pib)
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

	STDMETHOD(OnStopBinding)(HRESULT hresult,LPCWSTR szError)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(GetBindInfo)(DWORD __RPC_FAR *grfBINDF,BINDINFO __RPC_FAR *pbindinfo)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(OnDataAvailable)(DWORD grfBSCF,DWORD dwSize,FORMATETC __RPC_FAR *pformatetc,STGMEDIUM __RPC_FAR *pstgmed)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(OnObjectAvailable)(REFIID riid,IUnknown __RPC_FAR *punk)
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
	STDMETHOD(QueryInterface)(REFIID riid,void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		return E_NOTIMPL;
	}
};

HINSTANCE hi;

void info_onClick(Label* me) {
	if (me->tag == "1") {
		cv::destroyAllWindows();
		cv::Mat image = cv::imread(filename.c_str(), cv::IMREAD_COLOR);
		cv::namedWindow(filename.c_str(), cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_NORMAL);
		frmain.minimum(); 
		cv::imshow(filename.c_str(), image);
		cv::resizeWindow(filename.c_str(), 1280, 720);
		cv::waitKey(0);
	}
}

void frmain_Loaded(form* me) {
	Label* info = (Label*)(frmain.tab[0]);
	std::string httpD = httpGet("http://cn.bing.com");
	std::string pic = regurl(httpD);
	if (pic == "") {
		info->name("No pic found. ask for dev. ");
		return;
	}
	filename = download(pic);
	Sleep(500);
	
	((ProgressBar*)frmain.tab[1])->full();
	cv::Mat image = cv::imread(filename.c_str(), cv::IMREAD_COLOR);
	cv::namedWindow(filename.c_str(), cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_NORMAL);
	frmain.minimum();
	((button*)frmain.tab[2])->show();
	cv::imshow(filename.c_str(),image);
	cv::resizeWindow(filename.c_str(), 1280, 720);
	cv::waitKey(0);
	info->name("Today's Bing image is ready. Enjoy it.");
	info->tag = "1";
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow){
	hi = hInstance;
	frmain.Event_Load_Complete = frmain_Loaded;
	frmain.create();

	Label info(&frmain, 8, 500, 500, 36, (char*)"");
	ProgressBar dld(&frmain, 8, 304, 1120, 16, (char*)"downloading...");
	button setWall(&frmain, 400, 80, 280, 168, (char*)"Set as wallpaper");
	info.tag = "0";
	info.create();
	dld.create();
	setWall.create();
	setWall.hide();
	info.Event_On_Click = info_onClick;
	setWall.Event_On_Click = [](button* me) {if (filename.c_str())setWallpaper(filename); };
	frmain.show();
}

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

std::string regurl(std::string httpD) {
	std::regex rr("/az.*?_1920x1080.jpg");
	std::smatch rm;
	if (std::regex_search(httpD, rm, rr)) return "http://cn.bing.com" + rm.str();
	else {
		((Label*)frmain.tab[0])->name("Sorry. But no picture is crawled. Ask for your dev.");
		return "";
	}
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

void setWallpaper(std::string path) {
	HRESULT hr = S_OK;

	//设置壁纸风格和展开方式  
	//在Control Panel\Desktop中的两个键值将被设置  
	// TileWallpaper  
	//  0: 图片不被平铺   
	//  1: 被平铺   
	// WallpaperStyle  
	//  0:  0表示图片居中，1表示平铺  
	//  2:  拉伸填充整个屏幕  
	//  6:  拉伸适应屏幕并保持高度比  
	//  10: 图片被调整大小裁剪适应屏幕保持纵横比  

	//以可读可写的方式打开HKCU\Control Panel\Desktop注册表项  
	HKEY hKey = NULL;
	hr = HRESULT_FROM_WIN32(RegOpenKeyEx(HKEY_CURRENT_USER,
		"Control Panel\\Desktop", 0, KEY_READ | KEY_WRITE, &hKey));
	if (SUCCEEDED(hr))
	{
		LPCSTR pszWallpaperStyle;
		LPCSTR pszTileWallpaper;
		pszWallpaperStyle = "6";
		pszTileWallpaper = "0";
		// 设置 WallpaperStyle 和 TileWallpaper 到注册表项.  
		DWORD cbData = lstrlen(pszWallpaperStyle) * sizeof(*pszWallpaperStyle);
		hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, "WallpaperStyle", 0, REG_SZ,
			reinterpret_cast<const BYTE *>(pszWallpaperStyle), cbData));
		if (SUCCEEDED(hr))
		{
			cbData = lstrlen(pszTileWallpaper) * sizeof(*pszTileWallpaper);
			hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, "TileWallpaper", 0, REG_SZ,
				reinterpret_cast<const BYTE *>(pszTileWallpaper), cbData));
		}
		RegCloseKey(hKey);
	}
	char current[MAXSIZE];
	if (GetCurrentDirectoryA(MAXSIZE, current)) {
		std::string fullpath = current;
		fullpath += "//";
		fullpath += filename;
		if (SUCCEEDED(hr)) if (!SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (PVOID)fullpath.c_str(), SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE)) {
			//hr = HRESULT_FROM_WIN32(GetLastError());
			char s[10];
			_itoa_s(GetLastError(), s, 10);
			MessageBox(NULL, TEXT("设置壁纸失败！"), s, MB_OK);
		}
	}
	
}
