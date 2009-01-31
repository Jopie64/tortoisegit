// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2003-2008 - TortoiseGit

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
#pragma once

#include "resource.h"
#include "Git.h"
#include "ProjectProperties.h"
#include "StandAloneDlg.h"
#include "TGitPath.h"
#include "registry.h"
#include "SplitterControl.h"
#include "Colors.h"
#include "MenuButton.h"
#include "LogDlgHelper.h"
#include "FilterEdit.h"
#include "GitRev.h"
#include "Tooltip.h"
#include "HintListCtrl.h"
#include <regex>
#include "GitLogList.h"
#include "GitStatusListCtrl.h"

using namespace std;


#define MERGE_REVSELECTSTART	 1
#define MERGE_REVSELECTEND       2
#define MERGE_REVSELECTSTARTEND  3		///< both
#define MERGE_REVSELECTMINUSONE  4		///< first with N-1


#define LOGFILTER_TIMER		101

typedef int (__cdecl *GENERICCOMPAREFN)(const void * elem1, const void * elem2);

/**
 * \ingroup TortoiseProc
 * Shows log messages of a single file or folder in a listbox. 
 */
class CLogDlg : public CResizableStandAloneDialog, IFilterEditValidator
{
	DECLARE_DYNAMIC(CLogDlg)
	
	friend class CStoreSelection;

public:
	CLogDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLogDlg();
#if 0
	enum
	{
		LOGLIST_GRAPH,
		LOGLIST_ACTION,
		LOGLIST_MESSAGE,
		LOGLIST_AUTHOR,
		LOGLIST_DATE,
		LOGLIST_BUG,
		LOGLIST_MESSAGE_MAX=250
	};
#endif
	enum
	{
		FILELIST_ACTION,
		FILELIST_ADD,
		FILELIST_DEL,
		FILELIST_PATH
	};

	void SetParams(const CTGitPath& path, GitRev pegrev, GitRev startrev, GitRev endrev, int limit);
	void SetIncludeMerge(bool bInclude = true) {m_bIncludeMerges = bInclude;}
	void SetProjectPropertiesPath(const CTGitPath& path) {m_ProjectProperties.ReadProps(path);}
	bool IsThreadRunning() {return !!m_LogList.m_bThreadRunning;}
	void SetDialogTitle(const CString& sTitle) {m_sTitle = sTitle;}
	void SetSelect(bool bSelect) {m_bSelect = bSelect;}
	void ContinuousSelection(bool bCont = true) {m_bSelectionMustBeContinuous = bCont;}
	void SetMergePath(const CTGitPath& mergepath) {m_mergePath = mergepath;}

//	const GitRevRangeArray&	GetSelectedRevRanges() {return m_selectedRevs;}

// Dialog Data
	enum { IDD = IDD_LOGMESSAGE };

protected:
	//implement the virtual methods from Git base class
	virtual BOOL Log(git_revnum_t rev, const CString& author, const CString& date, const CString& message, LogChangedPathArray * cpaths,  int filechanges, BOOL copies, DWORD actions, BOOL haschildren);
	virtual BOOL Cancel();
	virtual bool Validate(LPCTSTR string);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg LRESULT OnFindDialogMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClickedInfoIcon(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClickedCancelFilter(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLogListLoading(WPARAM wParam, LPARAM lParam);

	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnBnClickedGetall();
	afx_msg void OnNMDblclkChangedFileList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkLoglist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedLoglist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedHelp();
	afx_msg void OnEnLinkMsgview(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedStatbutton();

	afx_msg void OnNMCustomdrawChangedFileList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnGetdispinfoChangedFileList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeSearchedit();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDtnDatetimechangeDateto(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeDatefrom(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickChangedFileList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickShowWholeProject();
	afx_msg void OnBnClickedHidepaths();
	afx_msg void OnBnClickedCheckStoponcopy();
	
	afx_msg void OnDtnDropdownDatefrom(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDropdownDateto(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedIncludemerge();
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnRefresh();
	afx_msg void OnFind();
	afx_msg void OnFocusFilter();
	afx_msg void OnEditCopy();

	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//virtual BOOL PreTranslateMessage(MSG* pMsg);

	void	FillLogMessageCtrl(bool bShow = true);
	void	DoDiffFromLog(INT_PTR selIndex, GitRev *rev1, GitRev *rev2, bool blame, bool unified);

	static  void LogCallBack(void *data, int cur){((CLogDlg*)data)->LogRunStatus(cur);}
	void	LogRunStatus(int cur);

	DECLARE_MESSAGE_MAP()

private:
	

	void Refresh (bool autoGoOnline = false);
	BOOL IsDiffPossible(LogChangedPath * changedpath, git_revnum_t rev);
	BOOL Open(bool bOpenWith, CString changedpath, git_revnum_t rev);
	void EditAuthor(const CLogDataVector& logs);
	void EditLogMessage(int index);
	void DoSizeV1(int delta);
	void DoSizeV2(int delta);
	void AdjustMinSize();
	void SetSplitterRange();
	void SetFilterCueText();
	
	void CopySelectionToClipBoard();
	void CopyChangedSelectionToClipBoard();
	CTGitPathList GetChangedPathsFromSelectedRevisions(bool bRelativePaths = false, bool bUseFilter = true);
    void SortShownListArray();
	
    void SetSortArrow(CListCtrl * control, int nColumn, bool bAscending);
	void SortByColumn(int nSortColumn, bool bAscending);
	
	void EnableOKButton();
	void GetAll(bool bIsShowProjectOrBranch = false);
	void UpdateLogInfoLabel();
	void SaveSplitterPos();
	bool ValidateRegexp(LPCTSTR regexp_str, tr1::wregex& pat, bool bMatchCase);
	void CheckRegexpTooltip();
	void GetChangedPaths(std::vector<CString>& changedpaths, std::vector<LogChangedPath*>& changedlogpaths);
	void DiffSelectedFile();
	void DiffSelectedRevWithPrevious();
	void SetDlgTitle(bool bOffline);
	CString GetAbsoluteUrlFromRelativeUrl(const CString& url);

	/**
	 * Extracts part of commit message suitable for displaying in revision list.
	 */
	CString MakeShortMessage(const CString& message);
//	inline int ShownCountWithStopped() const { return (int)m_arShownList.GetCount() + (m_bStrictStopped ? 1 : 0); }


	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	static int __cdecl	SortCompare(const void * pElem1, const void * pElem2);	///< sort callback function

	void ShowContextMenuForRevisions(CWnd* pWnd, CPoint point);
	void ShowContextMenuForChangedpaths(CWnd* pWnd, CPoint point);
public:
	CWnd *				m_pNotifyWindow;
	ProjectProperties	m_ProjectProperties;
	WORD				m_wParam;
private:
	//HFONT				m_boldFont;
	CString				m_sRelativeRoot;
	CString				m_sRepositoryRoot;
	CString				m_sSelfRelativeURL;
	CString				m_sURL;
	CString				m_sUUID;    ///< empty if the log cache is not used
	CGitLogList			m_LogList;
	CGitStatusListCtrl  m_ChangedFileListCtrl;
	CFilterEdit			m_cFilter;
	CProgressCtrl		m_LogProgress;
	CMenuButton			m_btnShow;
	CMenuButton			m_btnShowWholeProject;
	CTGitPath			m_path;
	CTGitPath			m_mergePath;
	GitRev				m_pegrev;
	GitRev				m_startrev;
	GitRev				m_LogRevision;
	GitRev				m_endrev;
	//GitRev				m_wcRev;
//	GitRevRangeArray	m_selectedRevs;
//	GitRevRangeArray	m_selectedRevsOneRange;
	bool				m_bSelectionMustBeContinuous;
	long				m_logcounter;
	bool				m_bCancelled;
	
	BOOL				m_bIncludeMerges;
	git_revnum_t		m_lowestRev;
	CTGitPathList	*   m_currentChangedArray;
	LogChangedPathArray m_CurrentFilteredChangedArray;
	CTGitPathList		m_currentChangedPathList;
	//CPtrArray			m_arShownList;
	bool				m_hasWC;
	int					m_nSearchIndex;
	bool				m_bFilterWithRegex;
	static const UINT	m_FindDialogMessage;
	CFindReplaceDialog *m_pFindDialog;
	CFont				m_logFont;
	CString				m_sMessageBuf;
	CSplitterControl	m_wndSplitter1;
	CSplitterControl	m_wndSplitter2;
	CRect				m_DlgOrigRect;
	CRect				m_MsgViewOrigRect;
	CRect				m_LogListOrigRect;
	CRect				m_ChgOrigRect;
//	CString				m_sFilterText;
	
	//volatile LONG		m_bNoDispUpdates;
	CDateTimeCtrl		m_DateFrom;
	CDateTimeCtrl		m_DateTo;
	int					m_limit;
	int					m_limitcounter;
	int                 m_nSortColumn;
	bool                m_bAscending;
	static int			m_nSortColumnPathList;
	static bool			m_bAscendingPathList;
	//CRegDWORD			m_regMaxBugIDColWidth;
	CButton				m_cHidePaths;
	bool				m_bShowedAll;
	CString				m_sTitle;
	bool				m_bSelect;
	bool				m_bShowBugtraqColumn;
	CString				m_sLogInfo;
	std::set<git_revnum_t> m_mergedRevs;

	CToolTips			m_tooltips;

	CColors				m_Colors;
	CImageList			m_imgList;
#if 0
	HICON				m_hModifiedIcon;
	HICON				m_hReplacedIcon;
	HICON				m_hAddedIcon;
	HICON				m_hDeletedIcon;
#endif

	DWORD				m_childCounter;
	DWORD				m_maxChild;
	HACCEL				m_hAccel;


    
	
	//CXPTheme			theme;
	bool				m_bVista;
};
static UINT WM_REVSELECTED = RegisterWindowMessage(_T("TORTOISEGit_REVSELECTED_MSG"));
static UINT WM_REVLIST = RegisterWindowMessage(_T("TORTOISEGit_REVLIST_MSG"));
static UINT WM_REVLISTONERANGE = RegisterWindowMessage(_T("TORTOISEGit_REVLISTONERANGE_MSG"));
