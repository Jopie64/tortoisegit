#pragma once


// CLcQuickLog

class CLcQuickLog : public CListCtrl
{
	DECLARE_DYNAMIC(CLcQuickLog)

public:
	CLcQuickLog();
	virtual ~CLcQuickLog();

	void PreSubclassWindow();
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);

	void AsyncRetrieveGitLog();

	void AddLog();

protected:
	DECLARE_MESSAGE_MAP()

private:
	bool m_bAbort;
	DWORD m_IdWinThread;
	DWORD m_IdThread;
	void WaitForThread();
public:
	afx_msg void OnDestroy();
};


