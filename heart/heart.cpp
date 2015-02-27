#include "heart.h"
//------------------------------------------------------
//	程序入口
//------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nCmdShow)
{
	if(FindWindow(WND_CLASSNAME,WND_TITLENAME))
	{
		MessageBox(NULL,_T("宝贝,程序已经运行了！"),_T("Warning！"),MB_OK);
		exit(0);
	}//用于检测是否已经运行了一个同窗体类、同窗体名的程序，即此程序是否已经运行
	lpfn pSetLayeredWindowAttributes;//定义设定窗体透明属性的函数
	HMODULE hUser32 = GetModuleHandle(_T("USER32.DLL"));
	pSetLayeredWindowAttributes = (lpfn)GetProcAddress(hUser32, "SetLayeredWindowAttributes");
	if (pSetLayeredWindowAttributes == NULL)
	{
		MessageBox(NULL,_T("此程序必须在winXP及以上系统运行！"),_T("Error!"),MB_OK);
		::PostQuitMessage(0);
	}
	if (hUser32 != NULL)
		FreeLibrary(hUser32); 
	//获取SetLayeredWindowAttributes值，如果获取失败则程序不能运行，窗口透明是在xp及以后加入的新属性，旧版本不支持
	init(hInstance);//初始化程序

	if (!RegisterClass(&wc)) 
		return 0;
	//注册窗体类

	RECT rcWnd = { 0, 0, wnd_width, wnd_height };
	AdjustWindowRect(&rcWnd, WS_POPUP, FALSE);//调整窗体大小
	//该函数依据所需客户矩形的大小，计算需要的窗口矩形的大小。计算出的窗口矩形随后可以传递给CreateWindow函数，
	//用于创建一个客户区所需大小的窗口。

	hWnd = CreateWindowEx(WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_TRANSPARENT,
		WND_CLASSNAME,
		WND_TITLENAME,
		WS_POPUP,
		0,	// SM_CXSCREEN：以像素为单位的屏幕的宽度
		0,	// SM_CYSCREEN：以像素为单位的屏幕的高度
		(rcWnd.right-rcWnd.left), (rcWnd.bottom-rcWnd.top),
		NULL,
		NULL,
		hInstance,
		NULL);//创建窗体
	notify.hWnd=hWnd;
	if(!Shell_NotifyIcon(NIM_ADD,&notify))
	{
		exit(0);
	}//创建托盘图标
//	::SetLayeredWindowAttributes(hWnd, 0, 100, LWA_ALPHA);			// LWA_ALPHA使整个窗口半透明
	pSetLayeredWindowAttributes(hWnd, RGB(255,255,255), 170, LWA_COLORKEY | LWA_ALPHA );	// LWA_COLORKEY使指定关键色半透明

	DWORD threadID=0;
	hThread=CreateThread(NULL,NULL,Paint,0,NULL,&threadID);//创建绘图线程
	SetWindowPos(hWnd,HWND_TOPMOST,0,0,0,0,3);//设置窗口位置
	ShowWindow(hWnd, SW_SHOWNOACTIVATE);//显示窗口
	UpdateWindow(hWnd);//自动更新窗口
	fileOperation(CRA);//创建临时音乐文件
	musicOperation(MCI_OPEN);//打开音乐
	musicOperation(MCI_PLAY);//播放音乐

	SetTimer(hWnd,1,15,TimerProc);//设置定时器，让窗口每隔一段时间设置到最前端显示

	ZeroMemory(&msg, sizeof(msg));//初始化消息

	while (GetMessage(&msg, NULL, 0, 0))
	{

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//消息循环
	end(hInstance);//结束工作
	return 0;
}

DWORD WINAPI Paint(LPVOID lpvoid)//绘图线程定义
{
	HDC hdc = GetDC(hWnd);//获取窗口的绘图hdc
	HDC hMdc=CreateCompatibleDC(hdc);//创建内存dc
	HBITMAP hMbitmap=CreateCompatibleBitmap(hdc,wnd_width,wnd_height);//创建内存绘图图片
	SelectObject(hMdc,hMbitmap);//关联dc和图片
	PNG	 png[8]={0};//要显示的8张图片
	HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);//获取程序实例句柄
	RECT clientRect={0,0,wnd_width,wnd_height};//定义窗口大小
	HBRUSH whitBru=(HBRUSH)GetStockObject(WHITE_BRUSH);//获取白色画刷，用于清空绘图内容的
	Graphics graphics(hMdc);//创建graphics绘图对象
	int pw,ph;//定义绘制图片的宽和高
	pw=ph=70;//都设置为70
	srand(timeGetTime());//随机种子

	for(int i=0;i<8;i++)
	{
		if(! ImageFromIDResource(IDB_PNG1+i,_T("PNG"),png[i].pImage))
			MessageBox(NULL,_T("加载图片失败！"),_T("错误！"),MB_OK);//初始化图片指针
		png[i].point.x=rand()%wnd_width;
		png[i].point.y=rand()%wnd_height;//初始化图片出现的位置，随机
		png[i].xop=png[i].yop=1;//初始化图片的运动方向，均是正方向
		png[i].xs=rand()%MAXSPEED+1;
		png[i].ys=rand()%MAXSPEED+1;//初始化图片的速度
	}
	//初始化png，即要显示的内容的初始化
//	GetClientRect(hWnd,&clientRect);
//	SetBkMode(hMdc, TRANSPARENT);

	while(true)
	{
		FillRect(hMdc,&clientRect,whitBru);//内存图片填充白色
//		TextOut(hMdc,100,100,_T(";dikfnaodrlgknldfjhglsjbrdglahdriugn"),36);
		
		for(int i=0;i<8;i++)
		{
			if (png[i].point.y<=0)
			{
				png[i].yop=1;
				png[i].ys=rand()%MAXSPEED+1;
			}
			if (png[i].point.x<=0)
			{
				png[i].xop=1;
				png[i].xs=rand()%MAXSPEED+1;
			}
			if (png[i].point.y+ph>=wnd_height)
			{
				png[i].yop=-1;
				png[i].ys=rand()%MAXSPEED+1;
			}
			if (png[i].point.x+pw>=wnd_width)
			{
				png[i].xop=-1;
				png[i].xs=rand()%MAXSPEED+1;
			}//判断图片是否运动到屏幕边界，如果是，则运动方向反向，随机该方向上的运动速度
			graphics.DrawImage(png[i].pImage, png[i].point.x,png[i].point.y,pw,ph);//在该位置绘制图片
			png[i].point.x+=(png[i].xs*png[i].xop);
			png[i].point.y+=(png[i].ys*png[i].yop);//改变图片坐标，即移动图片
		}//8张图片一张一张绘制
		if(! BitBlt(hdc,0,0,wnd_width,wnd_height,hMdc,0,0,SRCCOPY))
			MessageBox(NULL,_T("BitBlt Failed !"),_T("Error!"),MB_OK);//将内存图片拷贝到显示dc
		Sleep(40);//等待一段时间
	}//绘图循环

	DeleteObject(hMbitmap);//删除内存图片
	DeleteDC(hMdc);//删除内存dc
	ReleaseDC(hWnd, hdc);//释放dc
	
	return 0;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)//消息处理函数
{
	HMENU hSubMenu=NULL;
	switch(uMsg)
	{
	case WM_ICON_NOTIFY://托盘图标上的消息
		switch(LOWORD(lParam))
		{
		case WM_RBUTTONUP://右键
			POINT pos;
			hSubMenu=GetSubMenu(hMenu,0);
			GetCursorPos(&pos);
			SetForegroundWindow(hWnd);
			TrackPopupMenu(hSubMenu,0,pos.x,pos.y,0,hWnd,NULL);//在鼠标位置弹出菜单
			break;
		case WM_LBUTTONUP:
			SetForegroundWindow(hWnd);//激活该窗口
			break;
		case WM_LBUTTONDBLCLK://左键双击
			int	mode;
			if((mode=musicOperation(MCI_STATUS))==MCI_MODE_PLAY)
				musicOperation(MCI_PAUSE);
			else if(mode==MCI_MODE_PAUSE)
				musicOperation(MCI_PLAY);//如果音乐在播放，则暂停音乐，如果音乐暂停，则播放音乐
			break;
		}
		break;
	case WM_COMMAND://命令消息
		int wmId    = LOWORD(wParam);
		switch(wmId)
		{
		case ID_EXIT:
			PostQuitMessage(0);//退出
			break;
		}
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);//默认的消息处理函数
}

void CALLBACK TimerProc(HWND hWnd,UINT uMsg,UINT nTimerID,DWORD dwTime)//定时器消息处理函数
{
	BringWindowToTop(hWnd);//将窗口置顶
}

void init(HINSTANCE hInstance)
{
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WND_CLASSNAME;
	//设置窗体类属性

	hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON));//加载图标
	hMenu=LoadMenu(hInstance,MAKEINTRESOURCE(IDR_MENU1));//加载菜单
	if(hMenu==NULL||hIcon==NULL)
		MessageBox(NULL,_T("Icon or Menu load error!"),_T("Error!"),MB_OK);

	notify.cbSize=sizeof(notify);
	notify.dwInfoFlags=NIIF_USER | NIIF_LARGE_ICON ;
	notify.dwState=NIS_HIDDEN|NIS_SHAREDICON;
	notify.dwStateMask=NIS_HIDDEN|NIS_SHAREDICON;
	notify.uFlags=NIF_MESSAGE | NIF_ICON | NIF_TIP ;//| NIF_INFO;
	notify.hIcon=hIcon;
	notify.uCallbackMessage=WM_ICON_NOTIFY;
	notify.hBalloonIcon=hIcon;
	notify.uTimeout=30000;
	notify.uVersion=NOTIFYICON_VERSION;
	_tcsncpy(notify.szInfo,SZINFO,sizeof(SZINFO));
	_tcsncpy(notify.szInfoTitle,SZINFOTITLE,sizeof(SZINFOTITLE));
	_tcsncpy(notify.szTip,SZTIP,sizeof(SZTIP));
	//托盘显示的属性设置
    GdiplusStartup(&m_pGdiToken,&m_gdiplusStartupInput,NULL);//启动gdi+
}

void end (HINSTANCE hInstance)//结束函数
{
	TerminateThread(hThread,0);//结束绘图线程
	Shell_NotifyIcon(NIM_DELETE,&notify);//删除托盘图标
	musicOperation(MCI_STOP);//停止音乐播放
	musicOperation(MCI_CLOSE);//关闭音乐
	fileOperation(DEL);//删除临时文件
	GdiplusShutdown(m_pGdiToken);//关闭gdi+
	DestroyWindow(hWnd);//销毁窗口
	UnregisterClass(WND_CLASSNAME, hInstance);//清除注册窗体类
	
}


int	musicOperation(int uFlag)//音乐操作函数
{
	MCIDEVICEID static mciID;
	MCI_OPEN_PARMS static mciOpen={0};
	MCI_PLAY_PARMS static mciPlay={0};
	MCI_STATUS_PARMS static mciStatus={0};
	TCHAR buf[50];
	MCIERROR mciError;//音乐播放函数的变量
	switch(uFlag)
	{
	case MCI_OPEN://打开音乐
		mciOpen.lpstrDeviceType=_T("MPEG");
		mciOpen.lpstrElementName=filename;
		mciOpen.wDeviceID=NULL;
		mciOpen.dwCallback=(DWORD)hWnd;
		if(mciError=mciSendCommand(0, MCI_OPEN, MCI_NOTIFY | MCI_OPEN_ELEMENT, (DWORD)&mciOpen))
		{
			mciGetErrorString(mciError,buf,sizeof(buf));
			MessageBox(NULL,buf,_T("Open Error!"),MB_OK);
			exit(0);
		}
		mciID=mciOpen.wDeviceID;
		break;
	case MCI_PLAY://播放音乐
		if(mciError=mciSendCommand(mciID, MCI_PLAY, MCI_DGV_PLAY_REPEAT, (DWORD)&mciPlay))
		{
			mciGetErrorString(mciError,buf,sizeof(buf));
			MessageBox(NULL,buf,_T("Play Error!"),MB_OK);
			exit(0);
		}
		break;
	case MCI_PAUSE://暂停音乐
		if(mciError=mciSendCommand(mciID, MCI_PAUSE, 0, (DWORD)&mciPlay))
		{
			mciGetErrorString(mciError,buf,sizeof(buf));
			MessageBox(NULL,buf,_T("Pause Error!"),MB_OK);
			exit(0);
		}
		break;
	case MCI_STOP://停止音乐
		if(mciError=mciSendCommand(mciID, MCI_STOP, 0, 0))
		{
			mciGetErrorString(mciError,buf,sizeof(buf));
			MessageBox(NULL,buf,_T("Stop Error!"),MB_OK);
			exit(0);
		}
		break;
	case MCI_CLOSE://关闭音乐
		if(mciError=mciSendCommand(mciID, MCI_CLOSE, 0, 0))
		{
			mciGetErrorString(mciError,buf,sizeof(buf));
			MessageBox(NULL,buf,_T("Close Error!"),MB_OK);
			exit(0);
		}
		break;
	case MCI_STATUS://查询音乐状态
		mciStatus.dwItem=MCI_STATUS_MODE;
		if(mciError=mciSendCommand(mciID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)&mciStatus))
		{
			mciGetErrorString(mciError,buf,sizeof(buf));
			MessageBox(NULL,buf,_T("Status Error!"),MB_OK);
			exit(0);
		}
		return mciStatus.dwReturn;
		break;
	}
	return 0;
}

BOOL	fileOperation(int operate)//文件操作
{
	GetTempPath(200,filename);//文件路径
	wcscat(filename,FILENAME);//字符格式转换
	if(operate==CRA)//创建文件
	{
		HMODULE hModule=GetModuleHandle(NULL);
		HRSRC	hRes=FindResource(hModule,MAKEINTRESOURCE(IDR_MP31),_T("mp3"));
		HGLOBAL hGlobal=LoadResource(hModule,hRes);//加载音乐资源
		LPVOID	lpvoid=LockResource(hGlobal);//锁定
		HANDLE	fp=CreateFile(filename,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
		if( fp == INVALID_HANDLE_VALUE ) 
			return FALSE;//创建文件
		DWORD 	a;
		if(!WriteFile(fp,lpvoid,SizeofResource(NULL,hRes),&a,NULL))
		{
			MessageBox(NULL,_T("Write File Error!"),_T("Error!"),MB_OK);
			return false;
		}//写入文件内容
		FlushFileBuffers(fp);//清空缓冲区
		CloseHandle(fp);//关闭文件
		FreeResource(hGlobal);//释放资源
		return true;
	}
	else if(operate==DEL)//删除文件
	{
		return DeleteFile(filename);//删除文件
	}
	else
		return false;
}

BOOL ImageFromIDResource(UINT nID, LPCTSTR sTR,Image *&pImg)//从资源加载png图片
{
	HINSTANCE hInst = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
	HRSRC hRsrc = ::FindResource (hInst,MAKEINTRESOURCE(nID),sTR); // type
	if (!hRsrc)
		return FALSE;
	// load resource into memory
	DWORD len = SizeofResource(hInst, hRsrc);
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
	if (!lpRsrc)
		return FALSE;
	// Allocate global memory on which to create stream
	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
	memcpy(pmem,lpRsrc,len);//将资源拷贝到内存的另一个区域，以便操作
	GlobalUnlock(m_hMem);
	IStream* pstm;
	CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);
	// load from stream
	pImg=Gdiplus::Image::FromStream(pstm);//加载内存的png图片，并返回一个指针
	// free/release stuff
	pstm->Release();
	FreeResource(lpRsrc);
	GlobalFree(m_hMem);//释放流、资源、内存
	return TRUE;
}