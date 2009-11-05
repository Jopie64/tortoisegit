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

void CLcQuickLog::AddLog(CvCommit commits)
{
	if(!m_pWinThread->IsThisThread())
	{
		m_pWinThread->PostCallback(simplebind(&CLcQuickLog::AddLog, this, commits));
		return;
	}

	m_vCommit.insert(m_vCommit.end(), commits.begin(), commits.end());
	SetItemCountEx(m_vCommit.size());
//	Invalidate(FALSE);
}

class CGitRetrLog : public CGitCall_Collector
{
public:
	CGitRetrLog(CLcQuickLog* pLC):CGitCall_Collector(
//		L"git.exe log --pretty=oneline",
		L"git log --format=format:\"%H%x00%P%x00%s%x00%ai%x00%an%x00\"",
		'\0'),m_pLC(pLC),m_iState(0){}

	virtual bool	Abort()const{return m_pLC->m_bAbort;}

	static CString ToString(BYTE_VECTOR& Data)
	{
		CString result;
		Data.push_back('\0');
		g_Git.StringAppend(&result, &*Data.begin());
		return result;
	}

	virtual void	OnCollected(BYTE_VECTOR& Data)
	{
		Data.push_back('\0');
		switch(m_iState)
		{
		case 0:	// New / Commit hash
			m_vCommits.push_back(new CQlCommit);
			m_vCommits.back()->m_Rev = ToString(Data);
			break;
		case 1: // Parents
			m_vCommits.back()->m_RevParents = ToString(Data);
			break;
		case 2: // Subject
			m_vCommits.back()->m_Title = ToString(Data);
			break;
		case 3: // Author Date
			m_vCommits.back()->m_Date = ToString(Data);
			break;
		case 4: // Author Name
			m_vCommits.back()->m_Author = ToString(Data);
			break;
		}
		++m_iState;
		if(m_iState > 4)
			m_iState = 0;

		if(m_iState == 0 && m_vCommits.size() > 100)
		{
			m_pLC->AddLog(m_vCommits);
			m_vCommits.clear();
		}
	}

	CvCommit	 m_vCommits;
private:
	CLcQuickLog*	m_pLC;
	int				m_iState;
	


};

void CLcQuickLog::AsyncRetrieveGitLog()
{
//	while(!m_bAbort)
	{
//		Sleep(100);
//		m_pWinThread->PostCallback(simplebind(
//			&CLcQuickLog::AddLog,this));
		CGitRetrLog W_Cmd(this);
		g_Git.Run(&W_Cmd);
		AddLog(W_Cmd.m_vCommits);
		
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
	InsertColumn(0, L"Commit", 0, 80);
	InsertColumn(1, L"Subject", 0, 300);
	InsertColumn(2, L"Date", 0, 150);
	InsertColumn(3, L"Author", 0, 150);
	//SetItemCountEx(100);

	m_IdGitThread = Threading::ExecAsync(simplebind(&CLcQuickLog::AsyncRetrieveGitLog, this));
}

void CLcQuickLog::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* pItem= &(pDispInfo)->item;

	int ItemIx = pItem->iItem;
	if(ItemIx >= m_vCommit.size())
	{
		ASSERT(FALSE);
		return;
	}
	CQlCommit* pCommit = m_vCommit[ItemIx];

	if (pItem->mask & LVIF_TEXT) //valid text buffer?
	{
		switch(pItem->iSubItem)
		{
			case 0: 
				_snwprintf(pItem->pszText, pItem->cchTextMax, L"%s", pCommit->m_Rev);
				break;
			case 1: 
				_snwprintf(pItem->pszText, pItem->cchTextMax, L"%s", pCommit->m_Title);
				break;
			case 2: 
				_snwprintf(pItem->pszText, pItem->cchTextMax, L"%s", pCommit->m_Date);
				break;
			case 3: 
				_snwprintf(pItem->pszText, pItem->cchTextMax, L"%s", pCommit->m_Author);
				break;
		}
	}
}


void CLcQuickLog::OnDestroy()
{
	m_bAbort = true;
	if(m_IdGitThread != 0)
		Threading::CThread(m_IdGitThread).Join();

	Clear();
	CListCtrl::OnDestroy();
}
