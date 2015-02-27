#include <windows.h>		//Windows编程的头文件
#include <tchar.h>			//字符转换处理头文件
#include <string.h>			//字符串处理头文件
#include <shellapi.h>		//托盘图标头文件
#include "resource.h"		//资源头文件
#include <Digitalv.h>		//音乐播放头文件
#include <MMSystem.h>		//音乐播放头文件
#include <comdef.h>			//初始化一下com口
#include "GdiPlus.h"
using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")
#pragma comment(lib , "winmm.lib")//以上为添加gdi+库的代码


typedef struct PNG
{
	Image	*pImage;//图片指针，指向一个内存加载的png图片
	POINT   point;//点，用于储存图片在屏幕上的位置
	int		xop,yop,xs,ys;//*op用于存储图片的运动方向，*s为图片的速度
} PNG;		//定义机构体
typedef BOOL (WINAPI *lpfn) (HWND hWnd, COLORREF cr, BYTE bAlpha, DWORD dwFlags);//定义了一个函数指针

#define WND_CLASSNAME	_T("HeartWnd")
#define WND_TITLENAME	_T("LOVE")
#define	WM_ICON_NOTIFY	WM_APP+10
#define SZINFO			_T("宝贝臭臭，I Love You！")
#define SZINFOTITLE		_T("宝贝臭臭，我就是爱你！")
#define	FILENAME		_T("Heart_Together.mp3")
#define SZTIP			SZINFO
#define	CRA			1
#define	DEL			2
#define MAXSPEED	5
//定义了一些常量

HWND		hWnd = NULL;
WNDCLASS	wc;
const int	wnd_width=GetSystemMetrics(SM_CXSCREEN),
			wnd_height=GetSystemMetrics(SM_CYSCREEN);
NOTIFYICONDATA	notify={0};
MSG				msg;
HMENU			hMenu=NULL;
HICON			hIcon=NULL;
MCIDEVICEID		mciId;
TCHAR			filename[260]={0};
HANDLE			hThread=NULL;
//定义了一些全局变量

DWORD WINAPI		Paint(LPVOID lpvoid);
LRESULT	CALLBACK	WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void	CALLBACK	TimerProc(HWND hWnd,UINT uMsg,UINT nTimerID,DWORD dwTime);
void	init(HINSTANCE hInstance);
void	end(HINSTANCE hInstance);
int		musicOperation(int uFlag);
BOOL	fileOperation(int operate);
BOOL ImageFromIDResource(UINT nID, LPCTSTR sTR,Image *&pImg);
//函数声明

ULONG_PTR m_pGdiToken;
GdiplusStartupInput m_gdiplusStartupInput;//gdi+启动代码