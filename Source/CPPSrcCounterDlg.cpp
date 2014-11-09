// CPPSrcCounterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CPPSrcCounter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CBCGPDialog
{
public:

	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CBCGPDialog(CAboutDlg::IDD)
{
	EnableVisualManagerStyle(TRUE, TRUE);
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CBCGPDialog)
END_MESSAGE_MAP()

// CCPPSrcCounterDlg dialog

CCPPSrcCounterDlg::CCPPSrcCounterDlg(CWnd* pParent /*=NULL*/)
: CBCGPDialog(CCPPSrcCounterDlg::IDD, pParent), mResultChain(nullptr), m_nCountPass(0)
{
	m_nFileCount = m_nSumCode = m_nSumComment = m_nSumBlank = m_nSumTotal = 0;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	EnableVisualManagerStyle(TRUE, TRUE);
}

CCPPSrcCounterDlg::~CCPPSrcCounterDlg()
{
	//
}

void CCPPSrcCounterDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_SELECT, m_btnSelectFolder);
	DDX_Control(pDX, IDC_BUTTON_COUNT, m_btnCount);
	DDX_Control(pDX, IDC_BUTTON_EXIT, m_btnExit);
	DDX_Control(pDX, IDC_COMBO_FOLDER, m_cmbFolder);
	DDX_Control(pDX, IDC_PROGRESS, m_wndProgress);
}

#define WM_PARALLEL_RESULT WM_USER + 1

BEGIN_MESSAGE_MAP(CCPPSrcCounterDlg, CBCGPDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CCPPSrcCounterDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CCPPSrcCounterDlg::OnBnClickedCancel)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_SELECT, &CCPPSrcCounterDlg::OnBnClickedButtonSelect)
	ON_BN_CLICKED(IDC_BUTTON_COUNT, &CCPPSrcCounterDlg::OnBnClickedButtonCount)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CCPPSrcCounterDlg::OnBnClickedButtonExit)
	ON_MESSAGE(WM_PARALLEL_RESULT, &CCPPSrcCounterDlg::OnParallelResult)
END_MESSAGE_MAP()

// CCPPSrcCounterDlg message handlers

#define IDC_LIST_STATISTIC 1

const int DLG_CX = 900;
const int DLG_CY = 540;

BOOL CCPPSrcCounterDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);

	int nScreenCX = GetSystemMetrics(SM_CXSCREEN);
	int nScreenCY = GetSystemMetrics(SM_CYSCREEN);

	SetWindowPos(NULL, (nScreenCX - DLG_CX) >> 1, (nScreenCY - DLG_CY) >> 1, DLG_CX, DLG_CY, 0);

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_Images.SetImageSize(CSize(16, 16));
	m_Images.Load(IDB_BITMAP_BTN);

	m_btnSelectFolder.SetImage(m_Images.ExtractIcon(0));
	m_btnCount.SetImage(m_Images.ExtractIcon(2));
	m_btnExit.SetImage(m_Images.ExtractIcon(4));

	CRect rectClient;
	GetClientRect(rectClient);

	CRect rectCtrl;
	m_wndProgress.GetWindowRect(rectCtrl);
	ScreenToClient(rectCtrl);
	rectCtrl.right = rectClient.right - 10;
	m_wndProgress.SetWindowPos(NULL, rectCtrl.left, rectCtrl.top, rectCtrl.Width(), rectCtrl.Height(), 0);
	m_wndProgress.SetRange(0, 100);

	//m_btnExit.SetImage()

	CWnd * wndStatic = GetDlgItem(IDC_STATIC);

	wndStatic->GetWindowRect(rectCtrl);
	ScreenToClient(rectCtrl);
	rectCtrl.right = rectClient.right - 10;
	wndStatic->SetWindowPos(NULL, rectCtrl.left, rectCtrl.top, rectCtrl.Width(), rectCtrl.Height(), 0);

	m_cmbFolder.GetWindowRect(rectCtrl);
	ScreenToClient(rectCtrl);
	rectCtrl.right = rectClient.right - 10;
	m_cmbFolder.SetWindowPos(NULL, rectCtrl.left, rectCtrl.top, rectCtrl.Width(), rectCtrl.Height(), 0);
	
    //init list control
	CRect rectStatistic;
	GetClientRect(rectStatistic);

	rectStatistic.InflateRect(-10, -64, -10, -10);

	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_VSCROLL | LVS_REPORT;

	m_wndStatisitc.m_bVisualManagerStyle = TRUE;

	m_wndStatisitc.Create(dwStyle, rectStatistic, this, IDC_LIST_STATISTIC);
	m_wndStatisitc.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0,
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_wndStatisitc.InsertColumn(0, _T("File"), LVCFMT_LEFT, 550);
	m_wndStatisitc.InsertColumn(1, _T("Code"), LVCFMT_LEFT, 75);
	m_wndStatisitc.InsertColumn(2, _T("Comment"), LVCFMT_LEFT, 75);
	m_wndStatisitc.InsertColumn(3, _T("Blank"), LVCFMT_LEFT, 75);
	m_wndStatisitc.InsertColumn(4, _T("Total"), LVCFMT_LEFT, 75);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCPPSrcCounterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CBCGPDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCPPSrcCounterDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CBCGPDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCPPSrcCounterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCPPSrcCounterDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
}

void CCPPSrcCounterDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
}

void CCPPSrcCounterDlg::OnClose()
{
	ShutdownParallelSystem();

	FreeMemory(); //force free

	CBCGPDialog::OnClose();
	CBCGPDialog::OnOK();
}

void CCPPSrcCounterDlg::OnBnClickedButtonSelect()
{
	CString strSelectedFolder;
	if (theApp.GetShellManager()->BrowseForFolder(
		strSelectedFolder, this, strSelectedFolder))
	{
		m_cmbFolder.SetWindowText(strSelectedFolder);
		m_wndStatisitc.DeleteAllItems();
		m_wndProgress.SetPos(0);
	}
}

void CCPPSrcCounterDlg::OnBnClickedButtonCount()
{
	m_wndStatisitc.DeleteAllItems();

	CString strSelectedFolder;
	m_cmbFolder.GetWindowText(strSelectedFolder);

	if (strSelectedFolder.IsEmpty())
	{
		return;
	}

	SrcFileList fileList;
	fileList.Init(strSelectedFolder);

	m_nFileCount = fileList.GetCount();
	m_nFinishedCount = 0;

	m_wndProgress.SetPos(0);

	if (m_nFileCount <= 0)
	{
		return;
	}

	//reset parallel system
	m_timer.Start();

	ShutdownParallelSystem();
	InitParallelSystem();

	m_nCountPass++;

	m_nSumCode = m_nSumComment = m_nSumBlank = m_nSumTotal = 0;

	const wchar_t * FileName = fileList.GetFirst();
	if (FileName)
	{
		FileReadTask * Task = FileReadTask::Create(FileName);
		gFileSystem.PushTask(Task);
	}

	while (FileName = fileList.GetNext())
	{
		FileReadTask * Task = FileReadTask::Create(FileName);
		gFileSystem.PushTask(Task);
	}

}

void CCPPSrcCounterDlg::OnBnClickedButtonExit()
{
	CBCGPDialog::OnOK();
}

LRESULT CCPPSrcCounterDlg::OnParallelResult(WPARAM wParam, LPARAM lParam)
{
	ParallelTask * Result = nullptr;

	mLocker.Lock();
	{
		if (mResultChain)
		{
			Result = mResultChain->mOwner;
			Result->mChain.RemoveFromChain(mResultChain);
		}
	}
	mLocker.Unlock();

	if (Result)
	{
#ifdef _DEBUG
		Result->AssertValid();
#endif
		if ((int)wParam == m_nCountPass)
		{
			Result->Execute();
		}
		
		Result->Free();
	}

	return 0;
}

void CCPPSrcCounterDlg::InitParallelSystem()
{
	gSrcCountSystem.Init();
	gFileSystem.Init();
}

void CCPPSrcCounterDlg::ShutdownParallelSystem()
{
	gFileSystem.Shutdown();
	gSrcCountSystem.Shutdown();

	//clearup result chain
	while (mResultChain)
	{
		ParallelTask * Task = mResultChain->mOwner;
		Task->mChain.RemoveFromChain(mResultChain);
		Task->Free();
	}
}

void CCPPSrcCounterDlg::UpdateStatistic(int Code, int Comment, int Blank, int Total)
{
	m_nFinishedCount++;

	m_wndProgress.SetPos(m_nFinishedCount * 100 / m_nFileCount);

	m_nSumCode += Code;
	m_nSumComment += Comment;
	m_nSumBlank += Blank;
	m_nSumTotal += Total;

	if (m_nFinishedCount == m_nFileCount)
	{
		//shutdown parallel system
		ShutdownParallelSystem();

		wchar_t buffer[1024];
		swprintf_s(buffer, L"Summary: Total File %d, Time Elapsed %f s", m_nFinishedCount, m_timer.Stop());

		int nIdx = m_wndStatisitc.GetItemCount();
		m_wndStatisitc.InsertItem(nIdx, buffer);

		swprintf_s(buffer, L"%d", m_nSumCode);
		m_wndStatisitc.SetItemText(nIdx, 1, buffer);

		swprintf_s(buffer, L"%d", m_nSumComment);
		m_wndStatisitc.SetItemText(nIdx, 2, buffer);

		swprintf_s(buffer, L"%d", m_nSumBlank);
		m_wndStatisitc.SetItemText(nIdx, 3, buffer);

		swprintf_s(buffer, L"%d", m_nSumTotal);
		m_wndStatisitc.SetItemText(nIdx, 4, buffer);
	}
}

void CCPPSrcCounterDlg::NotifyResult(ParallelTask *Result)
{
	mLocker.Lock();
	{
		Result->mChain.AddToChain(mResultChain);
	}
	mLocker.Unlock();

	PostMessage(WM_PARALLEL_RESULT, m_nCountPass, 0);
}

void CCPPSrcCounterDlg::InsertRecord(LPCTSTR lpszFileName, int Code, int Comment, int Blank, int Total)
{
	wchar_t buffer[64];
	int nIdx = m_wndStatisitc.GetItemCount();
	m_wndStatisitc.InsertItem(nIdx, lpszFileName);

	swprintf_s(buffer, L"%d", Code);
	m_wndStatisitc.SetItemText(nIdx, 1, buffer);

	swprintf_s(buffer, L"%d", Comment);
	m_wndStatisitc.SetItemText(nIdx, 2, buffer);

	swprintf_s(buffer, L"%d", Blank);
	m_wndStatisitc.SetItemText(nIdx, 3, buffer);

	swprintf_s(buffer, L"%d", Total);
	m_wndStatisitc.SetItemText(nIdx, 4, buffer);

	UpdateStatistic(Code, Comment, Blank, Total);
}

void CCPPSrcCounterDlg::InsertError(LPCTSTR lpszFileName)
{
	int nIdx = m_wndStatisitc.GetItemCount();

	wchar_t Buffer[1024];
	swprintf_s(Buffer, L"[ERROR] %s", lpszFileName);
	m_wndStatisitc.InsertItem(nIdx, Buffer);

	UpdateStatistic(0, 0, 0, 0);
}