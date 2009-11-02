// LcQuickLog.cpp : implementation file
//

#include "stdafx.h"
#include "TortoiseProc.h"
#include "LcQuickLog.h"
#include "Threading.h"


// CLcQuickLog

IMPLEMENT_DYNAMIC(CLcQuickLog, CListCtrl)

CLcQuickLog::CLcQuickLog()
:	m_bAbort(false),
	m_IdThread(0),
	m_IdWinThread(GetCurrentThreadId())
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

void CLcQuickLog::AddLog()
{
	SetItemCountEx(GetItemCount() + 1);

}

void CLcQuickLog::AsyncRetrieveGitLog()
{
	while(!m_bAbort)
	{
		Sleep(100);
		Threading::CThreads::I()->Get(m_IdWinThread)->PostCallback(simplebind(
			&CLcQuickLog::AddLog,this));
		
	}
}

void CLcQuickLog::PreSubclassWindow()
{
	SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	InsertColumn(0, L"Col0", 0, 80);
	InsertColumn(1, L"Col1", 0, 300);
	//SetItemCountEx(100);

	m_IdThread = Threading::ExecAsync(simplebind(&CLcQuickLog::AsyncRetrieveGitLog, this));
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
	if(m_IdThread != 0)
		Threading::CThread(m_IdThread).Join();

	CListCtrl::OnDestroy();
}
