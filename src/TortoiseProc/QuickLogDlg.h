#pragma once
#include "afxcmn.h"
#include "LcQuickLog.h"


// CQuickLogDlg dialog

class CQuickLogDlg : public CDialog
{
	DECLARE_DYNAMIC(CQuickLogDlg)

public:
	CQuickLogDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CQuickLogDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_QUICKLOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CLcQuickLog m_LcQuicklog;
};
