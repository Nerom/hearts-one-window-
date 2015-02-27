#include "heart.h"
//------------------------------------------------------
//	�������
//------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nCmdShow)
{
	if(FindWindow(WND_CLASSNAME,WND_TITLENAME))
	{
		MessageBox(NULL,_T("����,�����Ѿ������ˣ�"),_T("Warning��"),MB_OK);
		exit(0);
	}//���ڼ���Ƿ��Ѿ�������һ��ͬ�����ࡢͬ�������ĳ��򣬼��˳����Ƿ��Ѿ�����
	lpfn pSetLayeredWindowAttributes;//�����趨����͸�����Եĺ���
	HMODULE hUser32 = GetModuleHandle(_T("USER32.DLL"));
	pSetLayeredWindowAttributes = (lpfn)GetProcAddress(hUser32, "SetLayeredWindowAttributes");
	if (pSetLayeredWindowAttributes == NULL)
	{
		MessageBox(NULL,_T("�˳��������winXP������ϵͳ���У�"),_T("Error!"),MB_OK);
		::PostQuitMessage(0);
	}
	if (hUser32 != NULL)
		FreeLibrary(hUser32); 
	//��ȡSetLayeredWindowAttributesֵ�������ȡʧ������������У�����͸������xp���Ժ����������ԣ��ɰ汾��֧��
	init(hInstance);//��ʼ������

	if (!RegisterClass(&wc)) 
		return 0;
	//ע�ᴰ����

	RECT rcWnd = { 0, 0, wnd_width, wnd_height };
	AdjustWindowRect(&rcWnd, WS_POPUP, FALSE);//���������С
	//�ú�����������ͻ����εĴ�С��������Ҫ�Ĵ��ھ��εĴ�С��������Ĵ��ھ��������Դ��ݸ�CreateWindow������
	//���ڴ���һ���ͻ��������С�Ĵ��ڡ�

	hWnd = CreateWindowEx(WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_TRANSPARENT,
		WND_CLASSNAME,
		WND_TITLENAME,
		WS_POPUP,
		0,	// SM_CXSCREEN��������Ϊ��λ����Ļ�Ŀ��
		0,	// SM_CYSCREEN��������Ϊ��λ����Ļ�ĸ߶�
		(rcWnd.right-rcWnd.left), (rcWnd.bottom-rcWnd.top),
		NULL,
		NULL,
		hInstance,
		NULL);//��������
	notify.hWnd=hWnd;
	if(!Shell_NotifyIcon(NIM_ADD,&notify))
	{
		exit(0);
	}//��������ͼ��
//	::SetLayeredWindowAttributes(hWnd, 0, 100, LWA_ALPHA);			// LWA_ALPHAʹ�������ڰ�͸��
	pSetLayeredWindowAttributes(hWnd, RGB(255,255,255), 170, LWA_COLORKEY | LWA_ALPHA );	// LWA_COLORKEYʹָ���ؼ�ɫ��͸��

	DWORD threadID=0;
	hThread=CreateThread(NULL,NULL,Paint,0,NULL,&threadID);//������ͼ�߳�
	SetWindowPos(hWnd,HWND_TOPMOST,0,0,0,0,3);//���ô���λ��
	ShowWindow(hWnd, SW_SHOWNOACTIVATE);//��ʾ����
	UpdateWindow(hWnd);//�Զ����´���
	fileOperation(CRA);//������ʱ�����ļ�
	musicOperation(MCI_OPEN);//������
	musicOperation(MCI_PLAY);//��������

	SetTimer(hWnd,1,15,TimerProc);//���ö�ʱ�����ô���ÿ��һ��ʱ�����õ���ǰ����ʾ

	ZeroMemory(&msg, sizeof(msg));//��ʼ����Ϣ

	while (GetMessage(&msg, NULL, 0, 0))
	{

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//��Ϣѭ��
	end(hInstance);//��������
	return 0;
}

DWORD WINAPI Paint(LPVOID lpvoid)//��ͼ�̶߳���
{
	HDC hdc = GetDC(hWnd);//��ȡ���ڵĻ�ͼhdc
	HDC hMdc=CreateCompatibleDC(hdc);//�����ڴ�dc
	HBITMAP hMbitmap=CreateCompatibleBitmap(hdc,wnd_width,wnd_height);//�����ڴ��ͼͼƬ
	SelectObject(hMdc,hMbitmap);//����dc��ͼƬ
	PNG	 png[8]={0};//Ҫ��ʾ��8��ͼƬ
	HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);//��ȡ����ʵ�����
	RECT clientRect={0,0,wnd_width,wnd_height};//���崰�ڴ�С
	HBRUSH whitBru=(HBRUSH)GetStockObject(WHITE_BRUSH);//��ȡ��ɫ��ˢ��������ջ�ͼ���ݵ�
	Graphics graphics(hMdc);//����graphics��ͼ����
	int pw,ph;//�������ͼƬ�Ŀ�͸�
	pw=ph=70;//������Ϊ70
	srand(timeGetTime());//�������

	for(int i=0;i<8;i++)
	{
		if(! ImageFromIDResource(IDB_PNG1+i,_T("PNG"),png[i].pImage))
			MessageBox(NULL,_T("����ͼƬʧ�ܣ�"),_T("����"),MB_OK);//��ʼ��ͼƬָ��
		png[i].point.x=rand()%wnd_width;
		png[i].point.y=rand()%wnd_height;//��ʼ��ͼƬ���ֵ�λ�ã����
		png[i].xop=png[i].yop=1;//��ʼ��ͼƬ���˶����򣬾���������
		png[i].xs=rand()%MAXSPEED+1;
		png[i].ys=rand()%MAXSPEED+1;//��ʼ��ͼƬ���ٶ�
	}
	//��ʼ��png����Ҫ��ʾ�����ݵĳ�ʼ��
//	GetClientRect(hWnd,&clientRect);
//	SetBkMode(hMdc, TRANSPARENT);

	while(true)
	{
		FillRect(hMdc,&clientRect,whitBru);//�ڴ�ͼƬ����ɫ
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
			}//�ж�ͼƬ�Ƿ��˶�����Ļ�߽磬����ǣ����˶�����������÷����ϵ��˶��ٶ�
			graphics.DrawImage(png[i].pImage, png[i].point.x,png[i].point.y,pw,ph);//�ڸ�λ�û���ͼƬ
			png[i].point.x+=(png[i].xs*png[i].xop);
			png[i].point.y+=(png[i].ys*png[i].yop);//�ı�ͼƬ���꣬���ƶ�ͼƬ
		}//8��ͼƬһ��һ�Ż���
		if(! BitBlt(hdc,0,0,wnd_width,wnd_height,hMdc,0,0,SRCCOPY))
			MessageBox(NULL,_T("BitBlt Failed !"),_T("Error!"),MB_OK);//���ڴ�ͼƬ��������ʾdc
		Sleep(40);//�ȴ�һ��ʱ��
	}//��ͼѭ��

	DeleteObject(hMbitmap);//ɾ���ڴ�ͼƬ
	DeleteDC(hMdc);//ɾ���ڴ�dc
	ReleaseDC(hWnd, hdc);//�ͷ�dc
	
	return 0;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)//��Ϣ������
{
	HMENU hSubMenu=NULL;
	switch(uMsg)
	{
	case WM_ICON_NOTIFY://����ͼ���ϵ���Ϣ
		switch(LOWORD(lParam))
		{
		case WM_RBUTTONUP://�Ҽ�
			POINT pos;
			hSubMenu=GetSubMenu(hMenu,0);
			GetCursorPos(&pos);
			SetForegroundWindow(hWnd);
			TrackPopupMenu(hSubMenu,0,pos.x,pos.y,0,hWnd,NULL);//�����λ�õ����˵�
			break;
		case WM_LBUTTONUP:
			SetForegroundWindow(hWnd);//����ô���
			break;
		case WM_LBUTTONDBLCLK://���˫��
			int	mode;
			if((mode=musicOperation(MCI_STATUS))==MCI_MODE_PLAY)
				musicOperation(MCI_PAUSE);
			else if(mode==MCI_MODE_PAUSE)
				musicOperation(MCI_PLAY);//��������ڲ��ţ�����ͣ���֣����������ͣ���򲥷�����
			break;
		}
		break;
	case WM_COMMAND://������Ϣ
		int wmId    = LOWORD(wParam);
		switch(wmId)
		{
		case ID_EXIT:
			PostQuitMessage(0);//�˳�
			break;
		}
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);//Ĭ�ϵ���Ϣ������
}

void CALLBACK TimerProc(HWND hWnd,UINT uMsg,UINT nTimerID,DWORD dwTime)//��ʱ����Ϣ������
{
	BringWindowToTop(hWnd);//�������ö�
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
	//���ô���������

	hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON));//����ͼ��
	hMenu=LoadMenu(hInstance,MAKEINTRESOURCE(IDR_MENU1));//���ز˵�
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
	//������ʾ����������
    GdiplusStartup(&m_pGdiToken,&m_gdiplusStartupInput,NULL);//����gdi+
}

void end (HINSTANCE hInstance)//��������
{
	TerminateThread(hThread,0);//������ͼ�߳�
	Shell_NotifyIcon(NIM_DELETE,&notify);//ɾ������ͼ��
	musicOperation(MCI_STOP);//ֹͣ���ֲ���
	musicOperation(MCI_CLOSE);//�ر�����
	fileOperation(DEL);//ɾ����ʱ�ļ�
	GdiplusShutdown(m_pGdiToken);//�ر�gdi+
	DestroyWindow(hWnd);//���ٴ���
	UnregisterClass(WND_CLASSNAME, hInstance);//���ע�ᴰ����
	
}


int	musicOperation(int uFlag)//���ֲ�������
{
	MCIDEVICEID static mciID;
	MCI_OPEN_PARMS static mciOpen={0};
	MCI_PLAY_PARMS static mciPlay={0};
	MCI_STATUS_PARMS static mciStatus={0};
	TCHAR buf[50];
	MCIERROR mciError;//���ֲ��ź����ı���
	switch(uFlag)
	{
	case MCI_OPEN://������
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
	case MCI_PLAY://��������
		if(mciError=mciSendCommand(mciID, MCI_PLAY, MCI_DGV_PLAY_REPEAT, (DWORD)&mciPlay))
		{
			mciGetErrorString(mciError,buf,sizeof(buf));
			MessageBox(NULL,buf,_T("Play Error!"),MB_OK);
			exit(0);
		}
		break;
	case MCI_PAUSE://��ͣ����
		if(mciError=mciSendCommand(mciID, MCI_PAUSE, 0, (DWORD)&mciPlay))
		{
			mciGetErrorString(mciError,buf,sizeof(buf));
			MessageBox(NULL,buf,_T("Pause Error!"),MB_OK);
			exit(0);
		}
		break;
	case MCI_STOP://ֹͣ����
		if(mciError=mciSendCommand(mciID, MCI_STOP, 0, 0))
		{
			mciGetErrorString(mciError,buf,sizeof(buf));
			MessageBox(NULL,buf,_T("Stop Error!"),MB_OK);
			exit(0);
		}
		break;
	case MCI_CLOSE://�ر�����
		if(mciError=mciSendCommand(mciID, MCI_CLOSE, 0, 0))
		{
			mciGetErrorString(mciError,buf,sizeof(buf));
			MessageBox(NULL,buf,_T("Close Error!"),MB_OK);
			exit(0);
		}
		break;
	case MCI_STATUS://��ѯ����״̬
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

BOOL	fileOperation(int operate)//�ļ�����
{
	GetTempPath(200,filename);//�ļ�·��
	wcscat(filename,FILENAME);//�ַ���ʽת��
	if(operate==CRA)//�����ļ�
	{
		HMODULE hModule=GetModuleHandle(NULL);
		HRSRC	hRes=FindResource(hModule,MAKEINTRESOURCE(IDR_MP31),_T("mp3"));
		HGLOBAL hGlobal=LoadResource(hModule,hRes);//����������Դ
		LPVOID	lpvoid=LockResource(hGlobal);//����
		HANDLE	fp=CreateFile(filename,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
		if( fp == INVALID_HANDLE_VALUE ) 
			return FALSE;//�����ļ�
		DWORD 	a;
		if(!WriteFile(fp,lpvoid,SizeofResource(NULL,hRes),&a,NULL))
		{
			MessageBox(NULL,_T("Write File Error!"),_T("Error!"),MB_OK);
			return false;
		}//д���ļ�����
		FlushFileBuffers(fp);//��ջ�����
		CloseHandle(fp);//�ر��ļ�
		FreeResource(hGlobal);//�ͷ���Դ
		return true;
	}
	else if(operate==DEL)//ɾ���ļ�
	{
		return DeleteFile(filename);//ɾ���ļ�
	}
	else
		return false;
}

BOOL ImageFromIDResource(UINT nID, LPCTSTR sTR,Image *&pImg)//����Դ����pngͼƬ
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
	memcpy(pmem,lpRsrc,len);//����Դ�������ڴ����һ�������Ա����
	GlobalUnlock(m_hMem);
	IStream* pstm;
	CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);
	// load from stream
	pImg=Gdiplus::Image::FromStream(pstm);//�����ڴ��pngͼƬ��������һ��ָ��
	// free/release stuff
	pstm->Release();
	FreeResource(lpRsrc);
	GlobalFree(m_hMem);//�ͷ�������Դ���ڴ�
	return TRUE;
}