#pragma once

#include "Git.h"
#include <map>
#include "afxcmn.h"
#include "StandAloneDlg.h"

class CShadowTree
{
public:
	typedef std::map<CString,CShadowTree> TShadowTreeMap;

	CShadowTree():m_hTree(NULL),m_pParent(NULL){}
	
	CShadowTree*	GetNextSub(CString& nameLeft, bool bCreateIfNotExist);

	bool			IsLeaf()const {return m_ShadowTree.empty();}
	CString			GetRefName()const
	{
		if(m_pParent==NULL)
			return m_csRefName;
		return m_pParent->GetRefName()+"/"+m_csRefName;
	}
	bool			IsFrom(const wchar_t* from)const
	{
		return wcsncmp(GetRefName(),from,wcslen(from))==0;
	}

	CString			m_csRefName;
	CString			m_csRefHash;
	CString			m_csDate;
	CString			m_csDate_Iso8601;
	CString			m_csAuthor;
	CString			m_csSubject;

	HTREEITEM		m_hTree;

	TShadowTreeMap	m_ShadowTree;
	CShadowTree*	m_pParent;
};
typedef std::vector<CShadowTree*> VectorPShadowTree;

class CBrowseRefsDlg : public CResizableStandAloneDialog
{
	DECLARE_DYNAMIC(CBrowseRefsDlg)

public:
	CBrowseRefsDlg(CString cmdPath, CWnd* pParent = NULL);   // standard constructor
	virtual ~CBrowseRefsDlg();

	enum eCmd
	{
		eCmd_ViewLog = WM_APP,
		eCmd_AddRemote,
		eCmd_ManageRemotes,
		eCmd_CreateBranch,
		eCmd_CreateTag,
		eCmd_DeleteBranch,
		eCmd_DeleteTag
	};

	enum eCol
	{
		eCol_Name,
		eCol_Date,
		eCol_Msg,
		eCol_Hash
	};

// Dialog Data
	enum { IDD = IDD_DIALOG_BROWSE_REFS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

	void			Refresh(bool bSelectCurHead=false);

	CShadowTree&	GetTreeNode(CString refName, CShadowTree* pTreePos=NULL, bool bCreateIfNotExist=false);

	void			FillListCtrlForTreeNode(HTREEITEM treeNode);

	void			FillListCtrlForShadowTree(CShadowTree* pTree, CString refNamePrefix, bool isFirstLevel);

	bool			SelectRef(CString refName);

	bool			ConfirmDeleteRef(CString completeRefName);
	bool			DoDeleteRef(CString completeRefName, bool bForce);

private:
	CString			m_cmdPath;

	CShadowTree		m_TreeRoot;
	CTreeCtrl		m_RefTreeCtrl;
	CListCtrl		m_ListRefLeafs;

	int				m_currSortCol;
	bool			m_currSortDesc;
	afx_msg void OnTvnSelchangedTreeRef(NMHDR *pNMHDR, LRESULT *pResult);
public:

	afx_msg void OnContextMenu(CWnd* pWndFrom, CPoint point);

	void		OnContextMenu_ListRefLeafs(CPoint point);
	void		OnContextMenu_RefTreeCtrl(CPoint point);

	void		ShowContextMenu(CPoint point, HTREEITEM hTreePos, VectorPShadowTree& selectedLeafs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLvnColumnclickListRefLeafs(NMHDR *pNMHDR, LRESULT *pResult);
};
