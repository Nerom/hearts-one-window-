#include <windows.h>		//Windows��̵�ͷ�ļ�
#include <tchar.h>			//�ַ�ת������ͷ�ļ�
#include <string.h>			//�ַ�������ͷ�ļ�
#include <shellapi.h>		//����ͼ��ͷ�ļ�
#include "resource.h"		//��Դͷ�ļ�
#include <Digitalv.h>		//���ֲ���ͷ�ļ�
#include <MMSystem.h>		//���ֲ���ͷ�ļ�
#include <comdef.h>			//��ʼ��һ��com��
#include "GdiPlus.h"
using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")
#pragma comment(lib , "winmm.lib")//����Ϊ���gdi+��Ĵ���


typedef struct PNG
{
	Image	*pImage;//ͼƬָ�룬ָ��һ���ڴ���ص�pngͼƬ
	POINT   point;//�㣬���ڴ���ͼƬ����Ļ�ϵ�λ��
	int		xop,yop,xs,ys;//*op���ڴ洢ͼƬ���˶�����*sΪͼƬ���ٶ�
} PNG;		//���������
typedef BOOL (WINAPI *lpfn) (HWND hWnd, COLORREF cr, BYTE bAlpha, DWORD dwFlags);//������һ������ָ��

#define WND_CLASSNAME	_T("HeartWnd")
#define WND_TITLENAME	_T("LOVE")
#define	WM_ICON_NOTIFY	WM_APP+10
#define SZINFO			_T("����������I Love You��")
#define SZINFOTITLE		_T("�����������Ҿ��ǰ��㣡")
#define	FILENAME		_T("Heart_Together.mp3")
#define SZTIP			SZINFO
#define	CRA			1
#define	DEL			2
#define MAXSPEED	5
//������һЩ����

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
//������һЩȫ�ֱ���

DWORD WINAPI		Paint(LPVOID lpvoid);
LRESULT	CALLBACK	WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void	CALLBACK	TimerProc(HWND hWnd,UINT uMsg,UINT nTimerID,DWORD dwTime);
void	init(HINSTANCE hInstance);
void	end(HINSTANCE hInstance);
int		musicOperation(int uFlag);
BOOL	fileOperation(int operate);
BOOL ImageFromIDResource(UINT nID, LPCTSTR sTR,Image *&pImg);
//��������

ULONG_PTR m_pGdiToken;
GdiplusStartupInput m_gdiplusStartupInput;//gdi+��������