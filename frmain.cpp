/*
* main.cpp
*
*  Created on: 2017年12月13日
*      Author: JamzumSum
*/
#pragma once
#include "GUI.hpp"
#include "resource.h"
#include <io.h>
#include <ShellAPI.h>

using namespace std;

extern string httpGet(string);
extern string regurl(string, string&);
extern bool download(string, string);
static void setWallpaper(string);
bool isExist(string filepath);

Label* notify = NULL;
ProgressBar* progress = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow){
	form frmain(NULL, "JamzumSum", "Bing Picker", 0, 0, 640, 360);
	form about(&frmain, "JamzumSum", "About");
	frmain.Event_On_Create = [&about, &frmain]() {
		frmain.enableRBmenu();
		frmain.createMenu(frmain.RBmenu, "About", [&]() {
			Label I(&about, 24, 24, 344, 216,
				"Developer:\r\n"
				"	JamzumSum\n\n"
				"Credit:\n"
				//"	@opencv - OpenCV Project\n"
				"	@sorcerer - Bing icon\n"
				"\nThanks for using.\n");
			about.Event_Load_Complete = [&]() {
				about.resize(392, 280);
				about.move(336, 112);
				I.setFont("微软雅黑", 20);
			};
			about.create();
			about.run();
			about.msgLoop.detach();
		});
	};
	frmain.setIcon(MAKEINTRESOURCE(IDI_ICON1), MAKEINTRESOURCE(IDI_ICON2));

	Label info(&frmain, 8, -60, 500, 36, "Tap `Download`");
	ProgressBar dld(&frmain, 8, -80, -32, 16, "downloading...");
	Button setWall(&frmain, 280, 120, 80, 45, "Download");
	//Picture setting(&frmain, 0, 0, 24, 24);

	string filename;
	frmain.feature ^= WS_MAXIMIZEBOX;
	//setting.feature |= SS_ICON | SS_REALSIZEIMAGE;
	bool downloaded = false;
	notify = &info;
	progress = &dld;

	auto showPic = [&frmain, &downloaded](string pic) {
		if (!downloaded) return;
		ShellExecute(frmain.hWnd(), "open", pic.c_str(), NULL, NULL, SW_SHOW);
	};

	info.Event_On_Click = [&showPic, &filename]() {
		showPic(filename + ".jpg");
	};

	/*setting.Event_On_Click = [&info]() {
		info.name = "Setting Clicked";
	};*/

	frmain.Event_Load_Complete = [&]() {
		info.setFont("微软雅黑", 18);
		setWall.setFont("微软雅黑", 18);
		//setting.loadIcon(MAKEINTRESOURCE(IDI_ICON3));
	};
	setWall.Event_On_Click = [&]() {
		if (downloaded) setWallpaper(filename + ".jpg");
		else {
			string httpD = httpGet("https://cn.bing.com");
			string pic;
			try {
				pic = regurl(httpD, filename);
				if (isExist(filename + ".jpg")) {
					if (MessageBox(frmain.hWnd(), (filename + ".jpg 已经存在，要继续吗？").c_str(), "BingPicker", MB_YESNO) == IDNO) return;
				}
				download(pic, filename);
			}
			catch (string msg) {
				info.name = msg.c_str();
				return;
			}

			Sleep(500);
			downloaded = true;
			dld.full();
			showPic(filename + ".jpg");
			setWall.name = "Set as Wallpaper";
			setWall.resize(160, 0);
			setWall.move(240, 0);
			info.name = ("Today's Bing image: " + filename).c_str();
		}
	};
	frmain.run();
	frmain.msgLoop.join();
	return 0;
}

void setWallpaper(string path) {
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
		if (SUCCEEDED(hr)) {
			cbData = lstrlen(pszTileWallpaper) * sizeof(*pszTileWallpaper);
			hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, "TileWallpaper", 0, REG_SZ,
				reinterpret_cast<const BYTE *>(pszTileWallpaper), cbData));
		}
		RegCloseKey(hKey);
	}
	char current[MAXSIZE];
	if (GetCurrentDirectory(MAXSIZE, current)) {
		string fullpath = current;
		fullpath += "\\";
		fullpath += path;
		if (SUCCEEDED(hr)) if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (PVOID)fullpath.c_str(), SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE)) {
			popError();
		}
	}
	
}

bool isExist(string filepath) {
	return _access(filepath.c_str(), 0) >= 0;
}