/*
* main.cpp
*
*  Created on: 2017年12月13日
*      Author: JamzumSum
*/
#pragma once
#include "GUI.h"
#include <regex>

#include "opencv2/highgui.hpp"

#define IDI_WINDOW1 102
#define IDI_ICON1 101

extern std::string httpGet(std::string);
extern std::string download(std::string);
extern void about_onCreate(form* me);
std::string regurl(std::string);
void setWallpaper(std::string);

HINSTANCE hi;

double start, end;
std::string filename;
extern form about;
form frmain((char*)"JamzumSum", (char*)"bingPicker");


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
		info->name = "No pic found. ask for dev. ";
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
	info->name="Today's Bing image is ready. Enjoy it.";
	info->tag = "1";
}

void frmain_onCreate(form* me) {
	
	
	me->pushRBmenu([](char* menu) {about.Event_On_Create = about_onCreate; about.create(); about.show(); }, (char*)"About");
	
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow){
	hi = hInstance;
	frmain.Event_Load_Complete = frmain_Loaded;
	frmain.Event_On_Create = frmain_onCreate;
	frmain.setIcon(MAKEINTRESOURCE(IDI_WINDOW1), MAKEINTRESOURCE(IDI_ICON1));
	if (frmain.create() < 0) return 1;

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
	return 0;
}



std::string regurl(std::string httpD) {
	std::regex rr("/az.*?_1920x1080.jpg");
	std::smatch rm;
	if (std::regex_search(httpD, rm, rr)) return "http://cn.bing.com" + rm.str();
	else {
		((Label*)frmain.tab[0])->name = "Sorry. But no picture is crawled. Ask for your dev.";
		return "";
	}
} 

void setWallpaper(std::string path) {
	HRESULT hr = S_OK;
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
