#pragma once
#include <windows.h>
#include <vector>
#include <algorithm>

#define CLR_DEFAULT             0xFF000000L
#define PBS_SMOOTH              0x01
#define PBM_SETRANGE            (WM_USER+1)
#define PBM_SETPOS              (WM_USER+2)
#define PBM_SETSTEP             (WM_USER+4)
#define PBM_STEPIT              (WM_USER+5)
#define PBM_GETPOS              (WM_USER+8)
#define PBM_SETBARCOLOR         (WM_USER+9)

extern HINSTANCE hi;
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

static void* getControl(HWND formHwnd, HWND controlHwnd);
static void* getForm(HWND hWnd);
static std::vector<void*> formSet;

class form {
private:
	//窝已经尽量私有了QAQ
	LPCSTR className;
	LPCSTR title;
	HWND hwnd;
	unsigned int id;
public:
	//构造
	form() {}
	form(char* className, char* title) {
		this->title = title;
		this->className = className;
	}
	//属性
	int brush = 0;
	int x;
	int y;
	int w;
	int h;
	std::vector<void*> tab;
	//方法
	HWND hWnd() { return this->hwnd; };
	//int create();
	//unsigned long long show();
	void minimum() {
		ShowWindow(hwnd, SW_SHOWMINNOACTIVE);
	}
	int create() {
		WNDCLASSA wndclass;
		wndclass.style = CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc = WinProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hbrBackground = (HBRUSH)GetStockObject(brush);
		wndclass.hCursor = LoadCursorA(NULL, IDC_ARROW);
		wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wndclass.hInstance = hi;
		wndclass.lpszClassName = className;
		wndclass.lpszMenuName = NULL;
		if (!RegisterClassA(&wndclass)) {
			char s[10];
			_itoa_s(GetLastError(), s, 10);
			MessageBox(NULL, TEXT("注册类名失败！"), s, MB_OK);
			return -1;
		}
		id = (UINT)formSet.size();
		formSet.push_back((void*)this);
		hwnd = CreateWindowA(className, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hi, NULL);
		if (!hwnd)
		{
			char s[10];
			_itoa_s(GetLastError(), s, 10);
			MessageBox(NULL, TEXT("创建窗口失败！"), s, MB_OK);
			UnregisterClassA(className, hi);
			formSet.pop_back();
			return -1;
		}
		if (this->Event_On_Create) this->Event_On_Create(this);
		return id;
	}
	unsigned long long show() {				//在此处主程序挂起
		MSG msg;
		ShowWindow(hwnd, SW_SHOW); UpdateWindow(hwnd);
		ZeroMemory(&msg, sizeof(msg));
		if (this->Event_Load_Complete) this->Event_Load_Complete(this);
		while (GetMessage(&msg, hwnd, 0, 0)>0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		UnregisterClassA(className, hi);
		return msg.wParam;
	}
	//事件
	void(*Event_On_Create)(form*) = NULL;
	void(*Event_On_Unload)(form*) = NULL;
	void(*Event_Load_Complete)(form*) = NULL;
};






class control {				//继承类
private:
	LPCSTR Name = "";
public:
	//属性
	int x;
	int y;
	int w;
	int h;
	char type;
	form* parent = NULL;
	HWND hWnd = NULL;
	unsigned int id;
	//方法

	//虚的
	virtual int create() = 0;
	//实的
	LPCSTR name(LPCSTR newName = "") {
		if (newName == "") return this->Name;
		if(hWnd) SetWindowTextA(hWnd,newName);
		this->Name = newName;
		return this->Name;
	}
	void hide() {
		ShowWindow(hWnd,0);
	}
	void show() {
		ShowWindow(hWnd, 1);
	}
};

class button :public control {
public:
	button() {}
	button(form* parent, int x, int y, int w, int h, char* Name) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		this->type = 'b';
		this->parent = parent;
		this->name(Name);				//x
		id = (UINT)parent->tab.size();
		parent->tab.push_back(this);
	}
	//int create();
	void(*Event_On_Click)(button*) = NULL;
	int create() {
		hWnd = CreateWindowA(
			"BUTTON",   // predefined class  
			this->name(),       // button text  
			WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // styles  
			this->x,         // starting x position  
			this->y,         // starting y position  
			this->w,        // button width  
			this->h,        // button height  
			this->parent->hWnd(),       // parent window  
			NULL,       // No menu  
			hi,
			NULL);      // pointer not needed  
		if (!hWnd) {
			char s[10];
			_itoa_s(GetLastError(), s, 10);
			MessageBox(NULL, TEXT("创建按钮失败！"), s, MB_OK);
			return -1;
		}
		return id;
	}
};



class Textbox :public control {
public:
	bool Multiline = true;
	Textbox() {}
	Textbox(form* parent, int x, int y, int w, int h, char* Name) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		this->type = 't';
		this->parent = parent;
		this->name(Name);				//x
		id = (UINT)parent->tab.size();
		parent->tab.push_back(this);
	}
	//int create();
	void(*Event_Text_Change)(Textbox*) = NULL;
	int create() {
		hWnd = CreateWindowExA(
			NULL,
			"Edit",
			this->name(),
			(this->Multiline ? ES_MULTILINE | WS_CHILD : WS_CHILD) | WS_VISIBLE | WS_BORDER | WS_GROUP | WS_TABSTOP | ES_WANTRETURN,
			this->x, this->y, this->w, this->h,
			this->parent->hWnd(),
			NULL,
			hi,
			NULL);
		if (!hWnd) {
			char s[10];
			_itoa_s(GetLastError(), s, 10);
			MessageBox(NULL, TEXT("创建文本框失败！"), s, MB_OK);
			return -1;
		}
		return id;
	}
};



class Label :public control {
public:
	std::string tag;
	Label() {}
	Label(form* parent, int x, int y, int w, int h, char* Name) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		this->type = 'l';
		this->parent = parent;
		this->name(Name);				//x
		id = (UINT)parent->tab.size();
		parent->tab.push_back(this);
	}
	//int create();
	void(*Event_On_Click)(Label*) = NULL;
	int create() {
		hWnd = CreateWindowA(
			"STATIC",
			this->name(),
			WS_CHILD | WS_VISIBLE | SS_NOTIFY,
			this->x, this->y, this->w, this->h,
			this->parent->hWnd(),
			NULL,
			hi,
			NULL);
		if (!hWnd) {
			char s[10];
			_itoa_s(GetLastError(), s, 10);
			MessageBox(NULL, TEXT("创建标签框失败！"), s, MB_OK);
			return -1;
		}
		return id;
	}
};



class Picture :public control {
	//.bmp only
public:
	LPCSTR path;
	Picture() {}
	Picture(form* parent, int x, int y, int w, int h, char* Name,char* picPath) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		this->type = 'p';
		this->parent = parent;
		this->name(Name);				//x
		this->path = picPath;
		id = (UINT)parent->tab.size();
		parent->tab.push_back(this);
	}
	//int create();
	void(*Event_On_Click)(Picture*) = NULL;
	int create() {
		hWnd = CreateWindowA(
			"STATIC",
			this->name(),
			WS_CHILD | SS_BITMAP | WS_VISIBLE,
			this->x, this->y, this->w, this->h,
			this->parent->hWnd(),
			NULL,
			hi,
			NULL);
		if (hWnd) {
			HBITMAP hbmp = (HBITMAP)LoadImage(NULL, this->path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			if (!hbmp) {
				char s[10];
				_itoa_s(GetLastError(), s, 10);
				MessageBox(NULL, TEXT("读取位图失败！"), s, MB_OK);
				return -1;
			}
			HBITMAP holdmap = (HBITMAP)SendMessage(hWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbmp);
			if (holdmap) {
				DeleteObject(holdmap);
			}
			DeleteObject(hbmp);
		}
		else {
			char s[10];
			_itoa_s(GetLastError(), s, 10);
			MessageBox(NULL, TEXT("创建图片失败！"), s, MB_OK);
			return -1;
		}
		return id;
	}
};



class ProgressBar :public control {
public:
	int step = 10;
	int range = 100;
	ProgressBar() {}
	ProgressBar(form* parent, int x, int y, int w, int h, char* Name) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		this->type = 'P';
		this->parent = parent;
		this->name(Name);				//x
		id = (UINT)parent->tab.size();
		parent->tab.push_back(this);
	}
	//int create();
	void stepIn() {
		SendMessage(hWnd, PBM_STEPIT, 0, 0);
	}
	int setStep(int newStep = 0) {
		if (newStep) { 
			SendMessage(hWnd, PBM_SETSTEP, (WPARAM)newStep, 0);
			step = newStep;
		}
		return step;
	}
	int setRange(int newRange = 0) {
		if (newRange) {
			SendMessage(hWnd, PBM_SETRANGE, 0, MAKELPARAM(0, newRange));
			range = newRange;
		}
		return range;
	}
	void setPos(int pos) {
		SendMessage(hWnd, PBM_SETPOS, pos, 0);
	}
	void empty() {
		setPos(0);
	}
	void full() {
		setPos((int)SendMessage(hWnd, PBM_GETPOS, 0, 0));
	}
	void setColor(ULONG color = CLR_DEFAULT) {
		//back to default if void
		SendMessage(hWnd, PBM_SETBARCOLOR,0,color);
	}
	int create() {
		hWnd = CreateWindowExA(
			NULL,
			"msctls_progress32",
			this->name(),
			WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
			this->x, this->y, this->w, this->h,
			this->parent->hWnd(),
			NULL,
			hi,
			NULL);
		if (!hWnd) {
			char s[10];
			_itoa_s(GetLastError(), s, 10);
			MessageBox(NULL, TEXT("创建进度条失败！"), s, MB_OK);
			return -1;
		}
		return id;
	}
};



static LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		//创建事件的思路还没有头绪。。暂时丢到CreateWindow后面去
		break;
	case WM_SIZE:
	{
		form* t = (form*)getForm(hwnd);
		t->w = LOWORD(lParam);
		t->h = HIWORD(lParam);
		break;
	}
	case WM_MOVE:
	{
		form* t = (form*)getForm(hwnd);
		t->x = LOWORD(lParam);
		t->y = HIWORD(lParam);
	}
	case WM_COMMAND:
		if (lParam)
		{
			void* p = getControl(hwnd,(HWND)lParam);
			if (p) switch (((control*)p)->type) {
			case 'b':
				if (((button*)p)->Event_On_Click) ((button*)p)->Event_On_Click((button*)p);
				break;
			case 'l':
				if (((Label*)p)->Event_On_Click) ((Label*)p)->Event_On_Click((Label*)p);
				break;
			case 'p':
				if (((Picture*)p)->Event_On_Click) ((Picture*)p)->Event_On_Click((Picture*)p);
				break;
			case 'B':
				break;
			}
		}
		break;
	case WM_CTLCOLORSTATIC://拦截WM_CTLCOLORSTATIC消息
	{
		SetBkMode((HDC)wParam, TRANSPARENT);//设置背景透明
		std::vector<void*>::iterator p = find_if(formSet.begin(), formSet.end(), [hwnd](const void* x) -> bool {return hwnd == ((form*)x)->hWnd(); });
		if(p != formSet.end()) 
			return (INT_PTR)GetStockObject(((form*)(*p))->brush);//返回父窗画刷
	}
	case WM_CLOSE:
		for (unsigned int i = 0; i < formSet.size(); i++) if (hwnd == ((form*)formSet[i])->hWnd()) {
			void(*p)(form*) = ((form*)formSet[i])->Event_On_Unload;
			if (p) p((form*)formSet[i]);
		}
		DestroyWindow(hwnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

static void* getForm(HWND hWnd) {
	std::vector<void*>::iterator r = find_if(formSet.begin(), formSet.end(), [hWnd](const void* x) -> bool {return ((form*)x)->hWnd() == hWnd; });
	if (r == formSet.end()) return NULL;
	else return *r;
}

static void* getControl(HWND formHwnd,HWND controlHwnd){
	std::vector<void*> controls = ((form*)getForm(formHwnd))->tab;
	std::vector<void*>::iterator r= find_if(controls.begin(),controls.end(), [controlHwnd](const void* x) -> bool { return ((control*)x)->hWnd == controlHwnd; });
	if (r == controls.end()) return NULL; 
	else return *r;
}