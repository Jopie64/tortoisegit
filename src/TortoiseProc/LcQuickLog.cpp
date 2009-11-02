// LcQuickLog.cpp : implementation file
//

#include "stdafx.h"
#include "TortoiseProc.h"
#include "LcQuickLog.h"
#include "Git.h"


// CLcQuickLog

IMPLEMENT_DYNAMIC(CLcQuickLog, CListCtrl)

CLcQuickLog::CLcQuickLog()
:	m_bAbort(false),
	m_IdGitThread(0),
	m_pWinThread(Threading::CThreads::I()->Get(GetCurrentThreadId()))
{

}

CLcQuickLog::~CLcQuickLog()
{
}


BEGIN_MESSAGE_MAP(CLcQuickLog, CListCtrl)
   ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDispInfo)
   ON_WM_DESTROY()
END_MESSAGE_MAP()



// CLcQuickLog message handlers

void CLcQuickLog::AddLog(CQlCommit* pCommit)
{
	if(!m_pWinThread->IsThisThread())
	{
		m_pWinThread->PostCallback(simplebind(&CLcQuickLog::AddLog, this, pCommit));
		return;
	}

	m_vCommit.push_back(pCommit);
	SetItemCountEx(m_vCommit.size());
	Invalidate(FALSE);
}

class CGitRetrLog : public CGitCall_Collector
{
public:
	CGitRetrLog(CLcQuickLog* pLC):CGitCall_Collector(L"git.exe log --pretty=oneline", "\n"),m_pLC(pLC){}

	virtual bool	Abort()const{return m_pLC->m_bAbort;}
	virtual void	OnCollected(BYTE_VECTOR& Data)
	{
		CQlCommit* pCommit = new CQlCommit;
		CStringA commitText = (const char*)&*Data.begin();
		pCommit->m_Rev = commitText;
		m_pLC->AddLog(pCommit);
	}

private:
	CLcQuickLog* m_pLC;


};

void CLcQuickLog::AsyncRetrieveGitLog()
{
	while(!m_bAbort)
	{
//		Sleep(100);
//		m_pWinThread->PostCallback(simplebind(
//			&CLcQuickLog::AddLog,this));
		CGitRetrLog W_Cmd(this);
		g_Git.Run(&W_Cmd);
		
	}
}

void CLcQuickLog::Clear()
{
	for(CvCommit::iterator it = m_vCommit.begin(); it != m_vCommit.end(); ++it)
		delete *it;

	m_vCommit.clear();
	SetItemCountEx(0);
}

void CLcQuickLog::PreSubclassWindow()
{
	SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	InsertColumn(0, L"Col0", 0, 80);
	InsertColumn(1, L"Col1", 0, 300);
	//SetItemCountEx(100);

	m_IdGitThread = Threading::ExecAsync(simplebind(&CLcQuickLog::AsyncRetrieveGitLog, this));
}

void CLcQuickLog::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* pItem= &(pDispInfo)->item;

	int ItemIx = pItem->iItem;

	if (pItem->mask & LVIF_TEXT) //valid text buffer?
	{
		switch(pItem->iSubItem)
		{
			case 0: 
				wsprintf(pItem->pszText, L"%d", ItemIx);
				break;
			case 1: 
				wsprintf(pItem->pszText, L"Hallo!");
				break;
		}
	}
}


void CLcQuickLog::OnDestroy()
{
	m_bAbort = true;
	if(m_IdGitThread != 0)
		Threading::CThread(m_IdGitThread).Join();

	CListCtrl::OnDestroy();
}
