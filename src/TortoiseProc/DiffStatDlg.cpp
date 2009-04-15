// DiffStatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TortoiseProc.h"
#include "DiffStatDlg.h"


// CDiffStatDlg dialog

IMPLEMENT_DYNAMIC(CDiffStatDlg, CDialog)

CDiffStatDlg::CDiffStatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDiffStatDlg::IDD, pParent)
{

}

CDiffStatDlg::~CDiffStatDlg()
{
}

void CDiffStatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DIFFSTAT, m_LcDiffstat);
}


BEGIN_MESSAGE_MAP(CDiffStatDlg, CDialog)
END_MESSAGE_MAP()


// CDiffStatDlg message handlers

BOOL CDiffStatDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_LcDiffstat.InsertColumn(0,L"File");
	m_LcDiffstat.InsertColumn(0,L"Changes");

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
