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

protected:
	DECLARE_MESSAGE_MAP()
};


