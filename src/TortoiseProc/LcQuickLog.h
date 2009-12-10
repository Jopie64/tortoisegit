#pragma once

#include "Threading.h"
#include "GitStatus.h"

// CLcQuickLog
class CQlCommit
{
public:
	git_revnum_t m_Rev;
	git_revnum_t m_RevParents;
	CString		 m_Title;
	CString		 m_Date;
	CString		 m_Author;
};

typedef std::vector<CQlCommit*> CvCommit;

class CLcQuickLog : public CListCtrl
{
	DECLARE_DYNAMIC(CLcQuickLog)

public:
	CLcQuickLog();
	virtual ~CLcQuickLog();

	void PreSubclassWindow();
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);

	void AsyncRetrieveGitLog();

	void AddLog(CvCommit commits);

	void Clear();

	void RefreshCurrPage();

protected:
	DECLARE_MESSAGE_MAP()

private:
	Threading::CMsgThread*	m_pWinThread;
	DWORD					m_IdGitThread;

	CvCommit m_vCommit;
	void WaitForThread();
public:
	bool m_bAbort;

	afx_msg void OnDestroy();
};


