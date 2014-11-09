// CPPSrcCounterDlg.h : header file
//

#pragma once

#include "ParallelTask.h"

class CBCGPListCtrlEx : public CBCGPListCtrl
{
public:

	virtual COLORREF OnGetCellBkColor(int nRow, int nColum)
	{
		if (nRow % 2)
		{
			return RGB(200, 255, 255);
		}
		else
		{
			return GetDefaultBkColor();
		}
	}
};

// CCPPSrcCounterDlg dialog
class CCPPSrcCounterDlg : public CBCGPDialog
{
public:

	CCPPSrcCounterDlg(CWnd* pParent = NULL);
	virtual ~CCPPSrcCounterDlg();

	enum { IDD = IDD_CPPSRCCOUNTER_DIALOG };

	void            NotifyResult(ParallelTask *Result);

	void            InsertRecord(LPCTSTR lpszFileName, int Code, int Comment, int Blank, int Total);
	void            InsertError(LPCTSTR lpszFileName);

protected:

	virtual void    DoDataExchange(CDataExchange* pDX);


// Implementation
private:

	HICON           m_hIcon;

	CBCGPButton     m_btnSelectFolder;
	CBCGPButton     m_btnCount;
	CBCGPButton     m_btnExit;
	CBCGPComboBox   m_cmbFolder;
	CBCGPProgressCtrl m_wndProgress;
	CBCGPToolBarImages m_Images;

	CBCGPListCtrlEx m_wndStatisitc;

	//task result
	Mutex           mLocker;
	Chain<ParallelTask> * mResultChain;

	int             m_nFileCount;
	int             m_nFinishedCount;

	int             m_nSumCode;
	int             m_nSumComment;
	int             m_nSumBlank;
	int             m_nSumTotal;

	int             m_nCountPass;

	Timer           m_timer;

	void            InitParallelSystem();
	void            ShutdownParallelSystem();

	void            UpdateStatistic(int Code, int Comment, int Blank, int Total);

	DECLARE_MESSAGE_MAP()

	// Generated message map functions
	virtual BOOL    OnInitDialog();
	afx_msg void    OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void    OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void    OnBnClickedOk();
	afx_msg void    OnBnClickedCancel();
	afx_msg void    OnClose();
	afx_msg void    OnBnClickedButtonSelect();
	afx_msg void    OnBnClickedButtonCount();
	afx_msg void    OnBnClickedButtonExit();
	afx_msg LRESULT OnParallelResult(WPARAM wParam, LPARAM lParam);
};
