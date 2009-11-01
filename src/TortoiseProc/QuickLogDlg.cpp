// QuickLogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TortoiseProc.h"
#include "QuickLogDlg.h"


// CQuickLogDlg dialog

IMPLEMENT_DYNAMIC(CQuickLogDlg, CDialog)

CQuickLogDlg::CQuickLogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQuickLogDlg::IDD, pParent)
{

}

CQuickLogDlg::~CQuickLogDlg()
{
}

void CQuickLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_QUICKLOG, m_LcQuicklog);
}


BEGIN_MESSAGE_MAP(CQuickLogDlg, CDialog)
END_MESSAGE_MAP()


// CQuickLogDlg message handlers
