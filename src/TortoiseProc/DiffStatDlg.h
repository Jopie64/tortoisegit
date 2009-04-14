#pragma once
#include "afxcmn.h"


// CDiffStatDlg dialog

class CDiffStatDlg : public CDialog
{
	DECLARE_DYNAMIC(CDiffStatDlg)

public:
	CDiffStatDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDiffStatDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_DIFFSTAT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_LcDiffstat;
};
