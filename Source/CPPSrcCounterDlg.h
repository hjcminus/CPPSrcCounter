// CPPSrcCounterDlg.h : header file
//

#pragma once

#include "ParallelTask.h"

class CBCGPListCtrlEx : public CBCGPListCtrl
{
public:

	virtual void Sort(int iColumn, BOOL bAscending, BOOL bAdd) {
		m_bAscending = bAscending;

		__super::Sort(iColumn, bAscending, bAdd);

		// need reset item data
		for (int i = 0; i < GetItemCount(); ++i) {
			SetItemData(i, i);
		}
	}

	virtual int OnCompareItems(LPARAM lParam1, LPARAM lParam2, int iColumn) {
		int item1 = (int)lParam1;
		int item2 = (int)lParam2;

		if (!m_bAscending) {
			// keep statistic line always at bottom
			if (item2 == GetItemCount() - 1) {
				return 1;
			}
		}

		CString str1 = GetItemText(item1, iColumn);
		CString str2 = GetItemText(item2, iColumn);

		if (iColumn == 0) { // file name
			return str1.Compare(str2);
		}
		else { // lines
			int i = _wtoi(str1);
			int j = _wtoi(str2);

			if (i > j) {
				return 1;
			}
			else if (i < j) {
				return -1;
			}
			else {
				return 0;
			}
		}
	}

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

private:

	BOOL m_bAscending;
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
	int				m_FolderNameLen;

	//task result
	Mutex           mLocker;
	Chain<ParallelTask> * mResultChain;

	int             m_nFileCount;
	int             m_nFinishedCount;

	int             m_nSumCodeLines;
	int             m_nSumCodeCommentLines;
	int             m_nSumCommentLines;
	int             m_nSumBlankLines;

	int             m_nCountPass;

	Timer           m_timer;

	void            InitParallelSystem();
	void            ShutdownParallelSystem();

	void            UpdateStatistic(int CodeLines, int CodeCommentLines, int CommentLines, int BlankLines);
	void            TryAppendPathToCombo(LPCTSTR lpszPath);

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
