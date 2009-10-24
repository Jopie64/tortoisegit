

// TortoiseSVN - a Windows shell extension for easy version control

// Copyright (C) 2003-2008 - TortoiseSVN

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

#include "StdAfx.h"
#include "GitStatusListCtrl.h"
#include "resource.h"
#include "..\\TortoiseShell\resource.h"
#include "MessageBox.h"
#include "MyMemDC.h"
#include "UnicodeUtils.h"
#include "AppUtils.h"
#include "PathUtils.h"
#include "TempFile.h"
#include "StringUtils.h"
#include "DirFileEnum.h"
#include "GitConfig.h"
//#include "SVNProperties.h"
#include "Git.h"
#include "GitDiff.h"
//#include "LogDlg.h"
//#include "SVNProgressDlg.h"
#include "SysImageList.h"
//#include "Svnstatuslistctrl.h"
#include "TGitPath.h"
#include "Registry.h"
#include "GitStatus.h"
//#include "SVNHelpers.h"
#include "InputDlg.h"
#include "ShellUpdater.h"
#include "GitAdminDir.h"
//#include "DropFiles.h"
#include "IconMenu.h"
//#include "AddDlg.h"
//#include "EditPropertiesDlg.h"
//#include "CreateChangelistDlg.h"
#include "XPTheme.h"
#include "CommonResource.h"

const UINT CGitStatusListCtrl::SVNSLNM_ITEMCOUNTCHANGED
					= ::RegisterWindowMessage(_T("GITSLNM_ITEMCOUNTCHANGED"));
const UINT CGitStatusListCtrl::SVNSLNM_NEEDSREFRESH
					= ::RegisterWindowMessage(_T("GITSLNM_NEEDSREFRESH"));
const UINT CGitStatusListCtrl::SVNSLNM_ADDFILE
					= ::RegisterWindowMessage(_T("GITSLNM_ADDFILE"));
const UINT CGitStatusListCtrl::SVNSLNM_CHECKCHANGED
					= ::RegisterWindowMessage(_T("GITSLNM_CHECKCHANGED"));
const UINT CGitStatusListCtrl::SVNSLNM_ITEMCHANGED
					= ::RegisterWindowMessage(_T("GITSLNM_ITEMCHANGED"));




BEGIN_MESSAGE_MAP(CGitStatusListCtrl, CListCtrl)
	ON_NOTIFY(HDN_ITEMCLICKA, 0, OnHdnItemclick)
	ON_NOTIFY(HDN_ITEMCLICKW, 0, OnHdnItemclick)
	ON_NOTIFY(HDN_ENDTRACK, 0, OnColumnResized)
	ON_NOTIFY(HDN_ENDDRAG, 0, OnColumnMoved)
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, OnLvnItemchanged)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDblclk)
	ON_NOTIFY_REFLECT(LVN_GETINFOTIP, OnLvnGetInfoTip)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	ON_WM_SETCURSOR()
	ON_WM_GETDLGCODE()
	ON_NOTIFY_REFLECT(NM_RETURN, OnNMReturn)
	ON_WM_KEYDOWN()
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
	ON_WM_PAINT()
	ON_NOTIFY(HDN_BEGINTRACKA, 0, &CGitStatusListCtrl::OnHdnBegintrack)
	ON_NOTIFY(HDN_BEGINTRACKW, 0, &CGitStatusListCtrl::OnHdnBegintrack)
	ON_NOTIFY(HDN_ITEMCHANGINGA, 0, &CGitStatusListCtrl::OnHdnItemchanging)
	ON_NOTIFY(HDN_ITEMCHANGINGW, 0, &CGitStatusListCtrl::OnHdnItemchanging)
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBeginDrag)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGING, &CGitStatusListCtrl::OnLvnItemchanging)
END_MESSAGE_MAP()



CGitStatusListCtrl::CGitStatusListCtrl() : CListCtrl()
	//, m_HeadRev(GitRev::REV_HEAD)
	, m_pbCanceled(NULL)
	, m_pStatLabel(NULL)
	, m_pSelectButton(NULL)
	, m_pConfirmButton(NULL)
	, m_bBusy(false)
	, m_bEmpty(false)
	, m_bUnversionedRecurse(true)
	, m_bShowIgnores(false)
	, m_pDropTarget(NULL)
	, m_bIgnoreRemoveOnly(false)
	, m_bCheckChildrenWithParent(false)
	, m_bUnversionedLast(true)
	, m_bHasChangeLists(false)
	, m_bHasLocks(false)
	, m_bBlock(false)
	, m_bBlockUI(false)
	, m_bHasCheckboxes(false)
	, m_bCheckIfGroupsExist(true)
	, m_bFileDropsEnabled(false)
	, m_bOwnDrag(false)
    , m_dwDefaultColumns(0)
    , m_ColumnManager(this)
    , m_bAscending(false)
    , m_nSortedColumn(-1)
	, m_sNoPropValueText(MAKEINTRESOURCE(IDS_STATUSLIST_NOPROPVALUE))
{
	m_FileLoaded=0;
	m_critSec.Init();
	m_bIsRevertTheirMy = false;
}

CGitStatusListCtrl::~CGitStatusListCtrl()
{
//	if (m_pDropTarget)
//		delete m_pDropTarget;
	ClearStatusArray();
}

void CGitStatusListCtrl::ClearStatusArray()
{
#if 0
	Locker lock(m_critSec);
	for (size_t i=0; i < m_arStatusArray.size(); i++)
	{
		delete m_arStatusArray[i];
	}
	m_arStatusArray.clear();
#endif
}
#if 0
CGitStatusListCtrl::FileEntry * CGitStatusListCtrl::GetListEntry(UINT_PTR index)
{

	if (index >= (UINT_PTR)m_arListArray.size())
		return NULL;
	if (m_arListArray[index] >= m_arStatusArray.size())
		return NULL;
	return m_arStatusArray[m_arListArray[index]];

	return NULL;
}
#endif
#if 0
CGitStatusListCtrl::FileEntry * CGitStatusListCtrl::GetVisibleListEntry(const CTGitPath& path)
{
	int itemCount = GetItemCount();
	for (int i=0; i < itemCount; i++)
	{
		FileEntry * entry = GetListEntry(i);
		if (entry->GetPath().IsEquivalentTo(path))
			return entry;
	}
	return NULL;
}
#endif
#if 0
CGitStatusListCtrl::FileEntry * CGitStatusListCtrl::GetListEntry(const CTGitPath& path)
{

	for (size_t i=0; i < m_arStatusArray.size(); i++)
	{
		FileEntry * entry = m_arStatusArray[i];
		if (entry->GetPath().IsEquivalentTo(path))
			return entry;
	}

	return NULL;
}
#endif

#if 0
int CGitStatusListCtrl::GetIndex(const CTGitPath& path)
{
	int itemCount = GetItemCount();
	for (int i=0; i < itemCount; i++)
	{
		FileEntry * entry = GetListEntry(i);
		if (entry->GetPath().IsEquivalentTo(path))
			return i;
	}
	return -1;
}
#endif 

void CGitStatusListCtrl::Init(DWORD dwColumns, const CString& sColumnInfoContainer, unsigned __int64 dwContextMenus /* = GitSLC_POPALL */, bool bHasCheckboxes /* = true */)
{
	Locker lock(m_critSec);

    m_dwDefaultColumns = dwColumns | 1;
    m_dwContextMenus = dwContextMenus;
	m_bHasCheckboxes = bHasCheckboxes;

    // set the extended style of the listcontrol
	// the style LVS_EX_FULLROWSELECT interferes with the background watermark image but it's more important to be able to select in the whole row.
	CRegDWORD regFullRowSelect(_T("Software\\TortoiseGit\\FullRowSelect"), TRUE);
	DWORD exStyle = LVS_EX_HEADERDRAGDROP | LVS_EX_DOUBLEBUFFER | LVS_EX_INFOTIP | LVS_EX_SUBITEMIMAGES;
	if (DWORD(regFullRowSelect))
		exStyle |= LVS_EX_FULLROWSELECT;
	exStyle |= (bHasCheckboxes ? LVS_EX_CHECKBOXES : 0);
	SetRedraw(false);
	SetExtendedStyle(exStyle);

	CXPTheme theme;
	theme.SetWindowTheme(m_hWnd, L"Explorer", NULL);

	m_nIconFolder = SYS_IMAGE_LIST().GetDirIconIndex();
	SetImageList(&SYS_IMAGE_LIST(), LVSIL_SMALL);

    m_ColumnManager.ReadSettings (m_dwDefaultColumns, sColumnInfoContainer);

	// enable file drops
#if 0
	if (m_pDropTarget == NULL)
	{
		m_pDropTarget = new CGitStatusListCtrlDropTarget(this);
		RegisterDragDrop(m_hWnd,m_pDropTarget);
		// create the supported formats:
		FORMATETC ftetc={0};
		ftetc.dwAspect = DVASPECT_CONTENT;
		ftetc.lindex = -1;
		ftetc.tymed = TYMED_HGLOBAL;
		ftetc.cfFormat=CF_HDROP;
		m_pDropTarget->AddSuportedFormat(ftetc);
	}
#endif

	SetRedraw(true);

	m_bUnversionedRecurse = !!((DWORD)CRegDWORD(_T("Software\\TortoiseGit\\UnversionedRecurse"), TRUE));
}

bool CGitStatusListCtrl::SetBackgroundImage(UINT nID)
{
	return CAppUtils::SetListCtrlBackgroundImage(GetSafeHwnd(), nID);
}

BOOL CGitStatusListCtrl::GetStatus ( const CTGitPathList* pathList
                                   , bool bUpdate /* = FALSE */
                                   , bool bShowIgnores /* = false */
								   , bool bShowUnRev
                                   , bool bShowUserProps /* = false */)
{
	Locker lock(m_critSec);
	int mask= CGitStatusListCtrl::FILELIST_MODIFY;
	if(bShowIgnores)
		mask|= CGitStatusListCtrl::FILELIST_IGNORE;
	if(bShowUnRev)
		mask|= CGitStatusListCtrl::FILELIST_UNVER;
	this->UpdateFileList(mask,bUpdate,(CTGitPathList*)pathList);


#if 0
	
	int refetchcounter = 0;
	BOOL bRet = TRUE;
	Invalidate();
	// force the cursor to change
	POINT pt;
	GetCursorPos(&pt);
	SetCursorPos(pt.x, pt.y);

	m_mapFilenameToChecked.clear();
	//m_StatusUrlList.Clear();
	bool bHasChangelists = (m_changelists.size()>1 || (m_changelists.size()>0 && !m_bHasIgnoreGroup));
	m_changelists.clear();
	for (size_t i=0; i < m_arStatusArray.size(); i++)
	{
		FileEntry * entry = m_arStatusArray[i];
		if ( bHasChangelists && entry->checked)
		{
			// If change lists are present, remember all checked entries
			CString path = entry->GetPath().GetGitPathString();
			m_mapFilenameToChecked[path] = true;
		}
		if ( (entry->status==git_wc_status_unversioned || entry->status==git_wc_status_missing ) && entry->checked )
		{
			// The user manually selected an unversioned or missing file. We remember
			// this so that the selection can be restored when refreshing.
			CString path = entry->GetPath().GetGitPathString();
			m_mapFilenameToChecked[path] = true;
		}
		else if ( entry->status > git_wc_status_normal && !entry->checked )
		{
			// The user manually deselected a versioned file. We remember
			// this so that the deselection can be restored when refreshing.
			CString path = entry->GetPath().GetGitPathString();
			m_mapFilenameToChecked[path] = false;
		}
	}

	// use a sorted path list to make sure we fetch the status of
	// parent items first, *then* the child items (if any)
	CTGitPathList sortedPathList = pathList;
	sortedPathList.SortByPathname();
	do
	{
		bRet = TRUE;
		m_nTargetCount = 0;
		m_bHasExternalsFromDifferentRepos = FALSE;
		m_bHasExternals = FALSE;
		m_bHasUnversionedItems = FALSE;
		m_bHasLocks = false;
		m_bHasChangeLists = false;
		m_bShowIgnores = bShowIgnores;
		m_nSortedColumn = 0;
		m_bBlock = TRUE;
		m_bBusy = true;
		m_bEmpty = false;
		Invalidate();

		// first clear possible status data left from
		// previous GetStatus() calls
		ClearStatusArray();

		m_StatusFileList = sortedPathList;

		// Since Git_client_status() returns all files, even those in
		// folders included with Git:externals we need to check if all
		// files we get here belongs to the same repository.
		// It is possible to commit changes in an external folder, as long
		// as the folder belongs to the same repository (but another path),
		// but it is not possible to commit all files if the externals are
		// from a different repository.
		//
		// To check if all files belong to the same repository, we compare the
		// UUID's - if they're identical then the files belong to the same
		// repository and can be committed. But if they're different, then
		// tell the user to commit all changes in the external folders
		// first and exit.
		CStringA sUUID;					// holds the repo UUID
		CTGitPathList arExtPaths;		// list of Git:external paths

		GitConfig config;

		m_sURL.Empty();

		m_nTargetCount = sortedPathList.GetCount();

		GitStatus status(m_pbCanceled);
		if(m_nTargetCount > 1 && sortedPathList.AreAllPathsFilesInOneDirectory())
		{
			// This is a special case, where we've been given a list of files
			// all from one folder
			// We handle them by setting a status filter, then requesting the Git status of
			// all the files in the directory, filtering for the ones we're interested in
			status.SetFilter(sortedPathList);

			// if all selected entries are files, we don't do a recursive status
			// fetching. But if only one is a directory, we have to recurse.
			git_depth_t depth = git_depth_files;
			// We have set a filter. If all selected items were files or we fetch
			// the status not recursively, then we have to include
			// ignored items too - the user has selected them
			bool bShowIgnoresRight = true;
			for (int fcindex=0; fcindex<sortedPathList.GetCount(); ++fcindex)
			{
				if (sortedPathList[fcindex].IsDirectory())
				{
					depth = git_depth_infinity;
					bShowIgnoresRight = false;
					break;
				}
			}
			if(!FetchStatusForSingleTarget(config, status, sortedPathList.GetCommonDirectory(), bUpdate, sUUID, arExtPaths, true, depth, bShowIgnoresRight))
			{
				bRet = FALSE;
			}
		}
		else
		{
			for(int nTarget = 0; nTarget < m_nTargetCount; nTarget++)
			{
				// check whether the path we want the status for is already fetched due to status-fetching
				// of a parent path.
				// this check is only done for file paths, because folder paths could be included already
				// but not recursively
				if (sortedPathList[nTarget].IsDirectory() || GetListEntry(sortedPathList[nTarget]) == NULL)
				{
					if(!FetchStatusForSingleTarget(config, status, sortedPathList[nTarget], bUpdate, sUUID, 
						arExtPaths, false, git_depth_infinity, bShowIgnores))
					{
						bRet = FALSE;
					}
				}
			}
		}

		// remove the 'helper' files of conflicted items from the list.
		// otherwise they would appear as unversioned files.
		for (INT_PTR cind = 0; cind < m_ConflictFileList.GetCount(); ++cind)
		{
			for (size_t i=0; i < m_arStatusArray.size(); i++)
			{
				if (m_arStatusArray[i]->GetPath().IsEquivalentTo(m_ConflictFileList[cind]))
				{
					delete m_arStatusArray[i];
					m_arStatusArray.erase(m_arStatusArray.begin()+i);
					break;
				}
			}
		}
		refetchcounter++;
	} while(!BuildStatistics() && (refetchcounter < 2) && (*m_pbCanceled==false));

    if (bShowUserProps)
        FetchUserProperties();

    m_ColumnManager.UpdateUserPropList (m_arStatusArray);

	m_bBlock = FALSE;
	m_bBusy = false;
	GetCursorPos(&pt);
	SetCursorPos(pt.x, pt.y);
	return bRet;
#endif 
	BuildStatistics();
	return TRUE;
}

//
// Fetch all local properties for all elements in the status array
//
void CGitStatusListCtrl::FetchUserProperties()
{
#if 0
	GitPool globalPool;

    for (size_t i = 0, count = m_arStatusArray.size(); i < count; ++i)
    {
        // local / temp pool to hold parameters and props for a single item

    	GitPool localPool ((apr_pool_t*)globalPool);

        // open working copy for this path

        const char* path = m_arStatusArray[i]->path.GetGitApiPath (localPool);
         
	    Git_wc_adm_access_t *adm_access = NULL;          
        Git_error_t * error = Git_wc_adm_probe_open3 ( &adm_access
                                                     , NULL
                                                     , path
                                                     , FALSE	// no write lock
													 , 0		// lock just the directory/file itself
                                                     , NULL
													 , NULL
                                                     , localPool);
        if (error == NULL)
        {
            // get the props and add them to the status info

            apr_hash_t* props = NULL;
            Git_error_t * error = Git_wc_prop_list ( &props
                                                   , path
                                                   , adm_access
                                                   , localPool);
            if (error == NULL)
            {
                for ( apr_hash_index_t *index 
                        = apr_hash_first (localPool, props)
                    ; index != NULL
                    ; index = apr_hash_next (index))
                {
                    // extract next entry from hash

                    const char* key = NULL;
                    ptrdiff_t keyLen;
                    const char** val = NULL;

                    apr_hash_this ( index
                                  , reinterpret_cast<const void**>(&key)
                                  , &keyLen
                                  , reinterpret_cast<void**>(&val));

                    // decode / dispatch it

        	        CString name = CUnicodeUtils::GetUnicode (key);
	                CString value = CUnicodeUtils::GetUnicode (*val);

                    // store in property container (truncate it after ~100 chars)

                    m_arStatusArray[i]->present_props[name] 
                        = value.Left (GitSLC_MAXUSERPROPLENGTH);
                }
            }
            error = Git_wc_adm_close2 (adm_access, localPool);
        }
        Git_error_clear (error);
    }
#endif
}


//
// Work on a single item from the list of paths which is provided to us
//
bool CGitStatusListCtrl::FetchStatusForSingleTarget(
							GitConfig& config,
							GitStatus& status,
							const CTGitPath& target,
							bool bFetchStatusFromRepository,
							CStringA& strCurrentRepositoryUUID,
							CTGitPathList& arExtPaths,
							bool bAllDirect,
							git_depth_t depth,
							bool bShowIgnores
							)
{
#if 0
	config.GetDefaultIgnores();

	CTGitPath workingTarget(target);

	git_wc_status2_t * s;
	CTGitPath GitPath;
	s = status.GetFirstFileStatus(workingTarget, GitPath, bFetchStatusFromRepository, depth, bShowIgnores);

	m_HeadRev = SVNRev(status.headrev);
	if (s!=0)
	{
		Git_wc_status_kind wcFileStatus = GitStatus::GetMoreImportant(s->text_status, s->prop_status);

		// This one fixes a problem with externals:
		// If a strLine is a file, Git:externals and its parent directory
		// will also be returned by GetXXXFileStatus. Hence, we skip all
		// status info until we find the one matching workingTarget.
		if (!workingTarget.IsDirectory())
		{
			if (!workingTarget.IsEquivalentTo(GitPath))
			{
				while (s != 0)
				{
					s = status.GetNextFileStatus(GitPath);
					if(workingTarget.IsEquivalentTo(GitPath))
					{
						break;
					}
				}
				if (s == 0)
				{
					m_sLastError = status.GetLastErrorMsg();
					return false;
				}
				// Now, set working target to be the base folder of this item
				workingTarget = workingTarget.GetDirectory();
			}
		}
		bool bEntryFromDifferentRepo = false;
		// Is this a versioned item with an associated repos UUID?
		if ((s->entry)&&(s->entry->uuid))
		{
			// Have we seen a repos UUID yet?
			if (strCurrentRepositoryUUID.IsEmpty())
			{
				// This is the first repos UUID we've seen - record it
				strCurrentRepositoryUUID = s->entry->uuid;
				m_sUUID = strCurrentRepositoryUUID;
			}
			else
			{
				if (strCurrentRepositoryUUID.Compare(s->entry->uuid)!=0)
				{
					// This item comes from a different repository than our main one
					m_bHasExternalsFromDifferentRepos = TRUE;
					bEntryFromDifferentRepo = true;
					if (s->entry->kind == Git_node_dir)
						arExtPaths.AddPath(workingTarget);
				}
			}
		}
		else if (strCurrentRepositoryUUID.IsEmpty() && (s->text_status == Git_wc_status_added))
		{
			// An added entry doesn't have an UUID assigned to it yet.
			// So we fetch the status of the parent directory instead and
			// check if that one has an UUID assigned to it.
			Git_wc_status2_t * sparent;
			CTGitPath path = workingTarget;
			do
			{
				CTGitPath GitParentPath;
				GitStatus tempstatus;
				sparent = tempstatus.GetFirstFileStatus(path.GetContainingDirectory(), GitParentPath, false, Git_depth_empty, false);
				path = GitParentPath;
			} while ( (sparent) && (sparent->entry) && (!sparent->entry->uuid) && (sparent->text_status==Git_wc_status_added) );
			if (sparent && sparent->entry && sparent->entry->uuid)
			{
				strCurrentRepositoryUUID = sparent->entry->uuid;
				m_sUUID = strCurrentRepositoryUUID;
			}
		}

		if ((wcFileStatus == Git_wc_status_unversioned)&& GitPath.IsDirectory())
		{
			// check if the unversioned folder is maybe versioned. This
			// could happen with nested layouts
			Git_wc_status_kind st = GitStatus::GetAllStatus(workingTarget);
			if ((st != Git_wc_status_unversioned)&&(st != Git_wc_status_none))
			{
				return true;	// ignore nested layouts
			}
		}
		if (status.IsExternal(GitPath))
		{
			m_bHasExternals = TRUE;
		}

		AddNewFileEntry(s, GitPath, workingTarget, true, m_bHasExternals, bEntryFromDifferentRepo);

		if (((wcFileStatus == Git_wc_status_unversioned)||(wcFileStatus == Git_wc_status_none)||((wcFileStatus == Git_wc_status_ignored)&&(m_bShowIgnores))) && GitPath.IsDirectory())
		{
			// we have an unversioned folder -> get all files in it recursively!
			AddUnversionedFolder(GitPath, workingTarget.GetContainingDirectory(), &config);
		}

		// for folders, get all statuses inside it too
		if(workingTarget.IsDirectory())
		{
			ReadRemainingItemsStatus(status, workingTarget, strCurrentRepositoryUUID, arExtPaths, &config, bAllDirect);
		}

	} // if (s!=0)
	else
	{
		m_sLastError = status.GetLastErrorMsg();
		return false;
	}
#endif
	return true;
}
#if 0
const CGitStatusListCtrl::FileEntry*
CGitStatusListCtrl::AddNewFileEntry(
			const git_wc_status2_t* pGitStatus,  // The return from the Git GetStatus functions
			const CTGitPath& path,				// The path of the item we're adding
			const CTGitPath& basePath,			// The base directory for this status build
			bool bDirectItem,					// Was this item the first found by GetFirstFileStatus or by a subsequent GetNextFileStatus call
			bool bInExternal,					// Are we in an 'external' folder
			bool bEntryfromDifferentRepo		// if the entry is from a different repository
			)
{
	FileEntry * entry = new FileEntry();

	
	entry->path = path;
	entry->basepath = basePath;
	entry->status = GitStatus::GetMoreImportant(pGitStatus->text_status, pGitStatus->prop_status);
	entry->textstatus = pGitStatus->text_status;
	entry->propstatus = pGitStatus->prop_status;
//	entry->remotestatus = GitStatus::GetMoreImportant(pGitStatus->repos_text_status, pGitStatus->repos_prop_status);
//	entry->remotetextstatus = pGitStatus->repos_text_status;
//	entry->remotepropstatus = pGitStatus->repos_prop_status;
	entry->inexternal = bInExternal;
	entry->differentrepo = bEntryfromDifferentRepo;
	entry->direct = bDirectItem;
//	entry->copied = !!pGitStatus->copied;
//	entry->switched = !!pGitStatus->switched;

	entry->last_commit_date = pGitStatus->ood_last_cmt_date;
	if ((entry->last_commit_date == NULL)&&(pGitStatus->entry))
		entry->last_commit_date = pGitStatus->entry->cmt_date;
	entry->remoterev = pGitStatus->ood_last_cmt_rev;
	if (pGitStatus->entry)
		entry->last_commit_rev = pGitStatus->entry->cmt_rev;
	if (pGitStatus->ood_last_cmt_author)
		entry->last_commit_author = CUnicodeUtils::GetUnicode(pGitStatus->ood_last_cmt_author);
	if ((entry->last_commit_author.IsEmpty())&&(pGitStatus->entry)&&(pGitStatus->entry->cmt_author))
		entry->last_commit_author = CUnicodeUtils::GetUnicode(pGitStatus->entry->cmt_author);

	if (pGitStatus->entry)
		entry->isConflicted = (pGitStatus->entry->conflict_wrk && PathFileExists(CUnicodeUtils::GetUnicode(pGitStatus->entry->conflict_wrk))) ? true : false;

	if ((entry->status == Git_wc_status_conflicted)||(entry->isConflicted))
	{
		entry->isConflicted = true;
		if (pGitStatus->entry)
		{
			CTGitPath cpath;
			if (pGitStatus->entry->conflict_wrk)
			{
				cpath = path.GetDirectory();
				cpath.AppendPathString(CUnicodeUtils::GetUnicode(pGitStatus->entry->conflict_wrk));
				m_ConflictFileList.AddPath(cpath);
			}
			if (pGitStatus->entry->conflict_old)
			{
				cpath = path.GetDirectory();
				cpath.AppendPathString(CUnicodeUtils::GetUnicode(pGitStatus->entry->conflict_old));
				m_ConflictFileList.AddPath(cpath);
			}
			if (pGitStatus->entry->conflict_new)
			{
				cpath = path.GetDirectory();
				cpath.AppendPathString(CUnicodeUtils::GetUnicode(pGitStatus->entry->conflict_new));
				m_ConflictFileList.AddPath(cpath);
			}
			if (pGitStatus->entry->prejfile)
			{
				cpath = path.GetDirectory();
				cpath.AppendPathString(CUnicodeUtils::GetUnicode(pGitStatus->entry->prejfile));
				m_ConflictFileList.AddPath(cpath);
			}
		}
	}

	if (pGitStatus->entry)
	{
		entry->isfolder = (pGitStatus->entry->kind == Git_node_dir);
		entry->Revision = pGitStatus->entry->revision;
		entry->keeplocal = !!pGitStatus->entry->keep_local;
		entry->working_size = pGitStatus->entry->working_size;
		entry->depth = pGitStatus->entry->depth;

		if (pGitStatus->entry->url)
		{
			entry->url = CUnicodeUtils::GetUnicode(CPathUtils::PathUnescape(pGitStatus->entry->url));
		}
		if (pGitStatus->entry->copyfrom_url)
		{
			entry->copyfrom_url = CUnicodeUtils::GetUnicode(CPathUtils::PathUnescape(pGitStatus->entry->copyfrom_url));
			entry->copyfrom_rev = pGitStatus->entry->copyfrom_rev;
		}
		else
			entry->copyfrom_rev = 0;

		if(bDirectItem)
		{
			if (m_sURL.IsEmpty())
				m_sURL = entry->url;
			else
				m_sURL.LoadString(IDS_STATUSLIST_MULTIPLETARGETS);
			m_StatusUrlList.AddPath(CTGitPath(entry->url));
		}
		if (pGitStatus->entry->lock_owner)
			entry->lock_owner = CUnicodeUtils::GetUnicode(pGitStatus->entry->lock_owner);
		if (pGitStatus->entry->lock_token)
		{
			entry->lock_token = CUnicodeUtils::GetUnicode(pGitStatus->entry->lock_token);
			m_bHasLocks = true;
		}
		if (pGitStatus->entry->lock_comment)
			entry->lock_comment = CUnicodeUtils::GetUnicode(pGitStatus->entry->lock_comment);

		if (pGitStatus->entry->present_props)
		{
			entry->present_props = pGitStatus->entry->present_props;
		}

		if (pGitStatus->entry->changelist)
		{
			entry->changelist = CUnicodeUtils::GetUnicode(pGitStatus->entry->changelist);
			m_changelists[entry->changelist] = -1;
			m_bHasChangeLists = true;
		}
		entry->needslock = (pGitStatus->entry->present_props && (strstr(pGitStatus->entry->present_props, "Git:needs-lock")!=NULL) );
	}
	else
	{
		if (pGitStatus->ood_kind == Git_node_none)
			entry->isfolder = path.IsDirectory();
		else
			entry->isfolder = (pGitStatus->ood_kind == Git_node_dir);
	}
	if (pGitStatus->repos_lock)
	{
		if (pGitStatus->repos_lock->owner)
			entry->lock_remoteowner = CUnicodeUtils::GetUnicode(pGitStatus->repos_lock->owner);
		if (pGitStatus->repos_lock->token)
			entry->lock_remotetoken = CUnicodeUtils::GetUnicode(pGitStatus->repos_lock->token);
		if (pGitStatus->repos_lock->comment)
			entry->lock_comment = CUnicodeUtils::GetUnicode(pGitStatus->repos_lock->comment);
	}

	// Pass ownership of the entry to the array
	m_arStatusArray.push_back(entry);

	return entry;
}
#endif

void CGitStatusListCtrl::AddUnversionedFolder(const CTGitPath& folderName,
												const CTGitPath& basePath,
												GitConfig * config)
{
#if 0
	if (!m_bUnversionedRecurse)
		return;
	CSimpleFileFind filefinder(folderName.GetWinPathString());

	CTGitPath filename;
	m_bHasUnversionedItems = TRUE;
	while (filefinder.FindNextFileNoDots())
	{
		filename.SetFromWin(filefinder.GetFilePath(), filefinder.IsDirectory());

		bool bMatchIgnore = !!config->MatchIgnorePattern(filename.GetFileOrDirectoryName());
		bMatchIgnore = bMatchIgnore || config->MatchIgnorePattern(filename.GetGitPathString());
		if (((bMatchIgnore)&&(m_bShowIgnores))||(!bMatchIgnore))
		{
			FileEntry * entry = new FileEntry();
			entry->path = filename;
			entry->basepath = basePath;
			entry->inunversionedfolder = true;
			entry->isfolder = filefinder.IsDirectory();

			m_arStatusArray.push_back(entry);
			if (entry->isfolder)
			{
				if (!g_GitAdminDir.HasAdminDir(entry->path.GetWinPathString(), true))
					AddUnversionedFolder(entry->path, basePath, config);
			}
		}
	}
#endif
}


void CGitStatusListCtrl::ReadRemainingItemsStatus(GitStatus& status, const CTGitPath& basePath,
										  CStringA& strCurrentRepositoryUUID,
										  CTGitPathList& arExtPaths, GitConfig * config, bool bAllDirect)
{
#if 0
	git_wc_status2_t * s;

	CTGitPath lastexternalpath;
	CTGitPath GitPath;
	while ((s = status.GetNextFileStatus(GitPath)) != NULL)
	{
		Git_wc_status_kind wcFileStatus = GitStatus::GetMoreImportant(s->text_status, s->prop_status);
		if ((wcFileStatus == Git_wc_status_unversioned) && (GitPath.IsDirectory()))
		{
			// check if the unversioned folder is maybe versioned. This
			// could happen with nested layouts
			Git_wc_status_kind st = GitStatus::GetAllStatus(GitPath);
			if ((st != Git_wc_status_unversioned)&&(st != Git_wc_status_none))
			{
				FileEntry * entry = new FileEntry();
				entry->path = GitPath;
				entry->basepath = basePath;
				entry->inunversionedfolder = true;
				entry->isfolder = true;
				entry->isNested = true;
				m_arStatusArray.push_back(entry);
				continue;
			}
		}
		bool bDirectoryIsExternal = false;
		bool bEntryfromDifferentRepo = false;
		if (s->entry)
		{
			if (s->entry->uuid)
			{
				if (strCurrentRepositoryUUID.IsEmpty())
					strCurrentRepositoryUUID = s->entry->uuid;
				else
				{
					if (strCurrentRepositoryUUID.Compare(s->entry->uuid)!=0)
					{
						bEntryfromDifferentRepo = true;
						if (GitStatus::IsImportant(wcFileStatus))
							m_bHasExternalsFromDifferentRepos = TRUE;
						if (s->entry->kind == Git_node_dir)
						{
							if ((lastexternalpath.IsEmpty())||(!lastexternalpath.IsAncestorOf(GitPath)))
							{
								arExtPaths.AddPath(GitPath);
								lastexternalpath = GitPath;
							}
						}
					}
				}
			}
			else
			{
				// we don't have an UUID - maybe an added file/folder
				if (!strCurrentRepositoryUUID.IsEmpty())
				{
					if ((!lastexternalpath.IsEmpty())&&
						(lastexternalpath.IsAncestorOf(GitPath)))
					{
						bEntryfromDifferentRepo = true;
						m_bHasExternalsFromDifferentRepos = TRUE;
					}
				}
			}
		}
		else
		{
			// if unversioned items lie around in external
			// directories from different repos, we have to mark them
			// as such too.
			if (!strCurrentRepositoryUUID.IsEmpty())
			{
				if ((!lastexternalpath.IsEmpty())&&
					(lastexternalpath.IsAncestorOf(GitPath)))
				{
					bEntryfromDifferentRepo = true;
				}
			}
		}
		if (status.IsExternal(GitPath))
		{
			arExtPaths.AddPath(GitPath);
			m_bHasExternals = TRUE;
		}
		if ((!bEntryfromDifferentRepo)&&(status.IsInExternal(GitPath)))
		{
			// if the externals are inside an unversioned folder (this happens if
			// the externals are specified with e.g. "ext\folder url" instead of just
			// "folder url"), then a commit won't succeed.
			// therefore, we treat those as if the externals come from a different
			// repository
			CTGitPath extpath = GitPath;
			while (basePath.IsAncestorOf(extpath))
			{
				if (!extpath.HasAdminDir())
				{
					bEntryfromDifferentRepo = true;
					break;
				}
				extpath = extpath.GetContainingDirectory();
			}
		}
		// Do we have any external paths?
		if(arExtPaths.GetCount() > 0)
		{
			// If do have external paths, we need to check if the current item belongs
			// to one of them
			for (int ix=0; ix<arExtPaths.GetCount(); ix++)
			{
				if (arExtPaths[ix].IsAncestorOf(GitPath))
				{
					bDirectoryIsExternal = true;
					break;
				}
			}
		}

		if ((wcFileStatus == Git_wc_status_unversioned)&&(!bDirectoryIsExternal))
			m_bHasUnversionedItems = TRUE;

		const FileEntry* entry = AddNewFileEntry(s, GitPath, basePath, bAllDirect, bDirectoryIsExternal, bEntryfromDifferentRepo);

		bool bMatchIgnore = !!config->MatchIgnorePattern(entry->path.GetFileOrDirectoryName());
		bMatchIgnore = bMatchIgnore || config->MatchIgnorePattern(entry->path.GetGitPathString());
		if ((((wcFileStatus == Git_wc_status_unversioned)||(wcFileStatus == Git_wc_status_none))&&(!bMatchIgnore))||
			((wcFileStatus == Git_wc_status_ignored)&&(m_bShowIgnores))||
			(((wcFileStatus == Git_wc_status_unversioned)||(wcFileStatus == Git_wc_status_none))&&(bMatchIgnore)&&(m_bShowIgnores)))
		{
			if (entry->isfolder)
			{
				// we have an unversioned folder -> get all files in it recursively!
				AddUnversionedFolder(GitPath, basePath, config);
			}
		}
	} // while ((s = status.GetNextFileStatus(GitPath)) != NULL) 
#endif
}

// Get the show-flags bitmap value which corresponds to a particular Git status
DWORD CGitStatusListCtrl::GetShowFlagsFromGitStatus(git_wc_status_kind status)
{
	switch (status)
	{
	case git_wc_status_none:
	case git_wc_status_unversioned:
		return SVNSLC_SHOWUNVERSIONED;
	case git_wc_status_ignored:
		if (!m_bShowIgnores)
			return SVNSLC_SHOWDIRECTS;
		return SVNSLC_SHOWDIRECTS|SVNSLC_SHOWIGNORED;
	case git_wc_status_incomplete:
		return SVNSLC_SHOWINCOMPLETE;
	case git_wc_status_normal:
		return SVNSLC_SHOWNORMAL;
	case git_wc_status_external:
		return SVNSLC_SHOWEXTERNAL;
	case git_wc_status_added:
		return SVNSLC_SHOWADDED;
	case git_wc_status_missing:
		return SVNSLC_SHOWMISSING;
	case git_wc_status_deleted:
		return SVNSLC_SHOWREMOVED;
	case git_wc_status_replaced:
		return SVNSLC_SHOWREPLACED;
	case git_wc_status_modified:
		return SVNSLC_SHOWMODIFIED;
	case git_wc_status_merged:
		return SVNSLC_SHOWMERGED;
	case git_wc_status_conflicted:
		return SVNSLC_SHOWCONFLICTED;
	case git_wc_status_obstructed:
		return SVNSLC_SHOWOBSTRUCTED;
	default:
		// we should NEVER get here!
		ASSERT(FALSE);
		break;
	}
	return 0;
}

void CGitStatusListCtrl::Show(DWORD dwShow, DWORD dwCheck /*=0*/, bool bShowFolders /* = true */,BOOL UpdateStatusList)
{
	CWinApp * pApp = AfxGetApp();
	if (pApp)
		pApp->DoWaitCursor(1);

	Locker lock(m_critSec);
	WORD langID = (WORD)CRegStdWORD(_T("Software\\TortoiseGit\\LanguageID"), GetUserDefaultLangID());
	
	//SetItemCount(listIndex);
	SetRedraw(FALSE);
	DeleteAllItems();
	PrepareGroups();
	m_nSelected = 0;

	if(UpdateStatusList)
	{
		m_arStatusArray.clear();
		for(int i=0;i<this->m_StatusFileList.GetCount();i++)
		{
			m_arStatusArray.push_back((CTGitPath*)&m_StatusFileList[i]);
		}

		for(int i=0;i<this->m_UnRevFileList.GetCount();i++)
		{
			m_arStatusArray.push_back((CTGitPath*)&m_UnRevFileList[i]);
		}

		for(int i=0;i<this->m_IgnoreFileList.GetCount();i++)
		{
			m_arStatusArray.push_back((CTGitPath*)&m_IgnoreFileList[i]);
		}
	}

	if( m_nSortedColumn )
	{
		CSorter predicate (&m_ColumnManager, m_nSortedColumn, m_bAscending);
		std::sort(m_arStatusArray.begin(), m_arStatusArray.end(), predicate);
	}

	int index =0;
	for(int i=0;i<this->m_arStatusArray.size();i++)
	{
		//set default checkbox status
		if(((CTGitPath*)m_arStatusArray[i])->m_Action & dwCheck)
			((CTGitPath*)m_arStatusArray[i])->m_Checked=true;
		else
			((CTGitPath*)m_arStatusArray[i])->m_Checked=false;

		if(((CTGitPath*)m_arStatusArray[i])->m_Action & dwShow)
		{
			AddEntry((CTGitPath*)m_arStatusArray[i],langID,index);
			index++;
		}
	}
	
	int maxcol = ((CHeaderCtrl*)(GetDlgItem(0)))->GetItemCount()-1;
	for (int col = 0; col <= maxcol; col++)
        SetColumnWidth (col, m_ColumnManager.GetWidth (col, true));

    SetRedraw(TRUE);
	GetStatisticsString();

	CHeaderCtrl * pHeader = GetHeaderCtrl();
	HDITEM HeaderItem = {0};
	HeaderItem.mask = HDI_FORMAT;
	for (int i=0; i<pHeader->GetItemCount(); ++i)
	{
		pHeader->GetItem(i, &HeaderItem);
		HeaderItem.fmt &= ~(HDF_SORTDOWN | HDF_SORTUP);
		pHeader->SetItem(i, &HeaderItem);
	}
	if (m_nSortedColumn)
	{
		pHeader->GetItem(m_nSortedColumn, &HeaderItem);
		HeaderItem.fmt |= (m_bAscending ? HDF_SORTUP : HDF_SORTDOWN);
		pHeader->SetItem(m_nSortedColumn, &HeaderItem);
	}

#if 0
	if (nSelectedEntry)
	{
		SetItemState(nSelectedEntry, LVIS_SELECTED, LVIS_SELECTED);
		EnsureVisible(nSelectedEntry, false);
	}
	else
	{
		// Restore the item at the top of the list.
		for (int i=0;GetTopIndex() != nTopIndex;i++)
		{
			if ( !EnsureVisible(nTopIndex+i,false) )
			{
				break;
			}
		}
	}
#endif
	if (pApp)
		pApp->DoWaitCursor(-1);

	Invalidate();
	
	m_dwShow = dwShow;

#if 0

	CWinApp * pApp = AfxGetApp();
	if (pApp)
		pApp->DoWaitCursor(1);

	m_bShowFolders = bShowFolders;
	
	int nTopIndex = GetTopIndex();
	POSITION posSelectedEntry = GetFirstSelectedItemPosition();
	int nSelectedEntry = 0;
	if (posSelectedEntry)
		nSelectedEntry = GetNextSelectedItem(posSelectedEntry);
	SetRedraw(FALSE);
	DeleteAllItems();

	PrepareGroups();

	m_arListArray.clear();

	m_arListArray.reserve(m_arStatusArray.size());
	SetItemCount (static_cast<int>(m_arStatusArray.size()));

	int listIndex = 0;
	for (size_t i=0; i < m_arStatusArray.size(); ++i)
	{
		FileEntry * entry = m_arStatusArray[i];
		if ((entry->inexternal) && (!(dwShow & SVNSLC_SHOWINEXTERNALS)))
			continue;
		if ((entry->differentrepo || entry->isNested) && (! (dwShow & SVNSLC_SHOWEXTERNALFROMDIFFERENTREPO)))
			continue;
		if (entry->IsFolder() && (!bShowFolders))
			continue;	// don't show folders if they're not wanted.

#if 0
		git_wc_status_kind status = GitStatus::GetMoreImportant(entry->status, entry->remotestatus);
		DWORD showFlags = GetShowFlagsFromGitStatus(status);
		if (entry->IsLocked())
			showFlags |= SVNSLC_SHOWLOCKS;
		if (entry->switched)
			showFlags |= SVNSLC_SHOWSWITCHED;
		if (!entry->changelist.IsEmpty())
			showFlags |= SVNSLC_SHOWINCHANGELIST;
#endif
		bool bAllowCheck = ((entry->changelist.Compare(SVNSLC_IGNORECHANGELIST) != 0) 
			&& (m_bCheckIfGroupsExist || (m_changelists.size()==0 || (m_changelists.size()==1 && m_bHasIgnoreGroup))));

		// status_ignored is a special case - we must have the 'direct' flag set to add a status_ignored item
#if 0
		if (status != Git_wc_status_ignored || (entry->direct) || (dwShow & GitSLC_SHOWIGNORED))
		{
			if ((!entry->IsFolder()) && (status == Git_wc_status_deleted) && (dwShow & SVNSLC_SHOWREMOVEDANDPRESENT))
			{
				if (PathFileExists(entry->GetPath().GetWinPath()))
				{
					m_arListArray.push_back(i);
					if ((dwCheck & SVNSLC_SHOWREMOVEDANDPRESENT)||((dwCheck & SVNSLC_SHOWDIRECTS)&&(entry->direct)))
					{
						if (bAllowCheck)
							entry->checked = true;
					}
					AddEntry(entry, langID, listIndex++);
				}
			}
			else if ((dwShow & showFlags)||((dwShow & SVNSLC_SHOWDIRECTFILES)&&(entry->direct)&&(!entry->IsFolder())))
			{
				m_arListArray.push_back(i);
				if ((dwCheck & showFlags)||((dwCheck & SVNSLC_SHOWDIRECTS)&&(entry->direct)))
				{
					if (bAllowCheck)
						entry->checked = true;
				}
				AddEntry(entry, langID, listIndex++);
			}
			else if ((dwShow & showFlags)||((dwShow & SVNSLC_SHOWDIRECTFOLDER)&&(entry->direct)&&entry->IsFolder()))
			{
				m_arListArray.push_back(i);
				if ((dwCheck & showFlags)||((dwCheck & SVNSLC_SHOWDIRECTS)&&(entry->direct)))
				{
					if (bAllowCheck)
						entry->checked = true;
				}
				AddEntry(entry, langID, listIndex++);
			}
		}
#endif
	}

	SetItemCount(listIndex);

    m_ColumnManager.UpdateRelevance (m_arStatusArray, m_arListArray);

	int maxcol = ((CHeaderCtrl*)(GetDlgItem(0)))->GetItemCount()-1;
	for (int col = 0; col <= maxcol; col++)
        SetColumnWidth (col, m_ColumnManager.GetWidth (col, true));

    SetRedraw(TRUE);
	GetStatisticsString();

	CHeaderCtrl * pHeader = GetHeaderCtrl();
	HDITEM HeaderItem = {0};
	HeaderItem.mask = HDI_FORMAT;
	for (int i=0; i<pHeader->GetItemCount(); ++i)
	{
		pHeader->GetItem(i, &HeaderItem);
		HeaderItem.fmt &= ~(HDF_SORTDOWN | HDF_SORTUP);
		pHeader->SetItem(i, &HeaderItem);
	}
	if (m_nSortedColumn)
	{
		pHeader->GetItem(m_nSortedColumn, &HeaderItem);
		HeaderItem.fmt |= (m_bAscending ? HDF_SORTUP : HDF_SORTDOWN);
		pHeader->SetItem(m_nSortedColumn, &HeaderItem);
	}

	if (nSelectedEntry)
	{
		SetItemState(nSelectedEntry, LVIS_SELECTED, LVIS_SELECTED);
		EnsureVisible(nSelectedEntry, false);
	}
	else
	{
		// Restore the item at the top of the list.
		for (int i=0;GetTopIndex() != nTopIndex;i++)
		{
			if ( !EnsureVisible(nTopIndex+i,false) )
			{
				break;
			}
		}
	}

	if (pApp)
		pApp->DoWaitCursor(-1);

	m_bEmpty = (GetItemCount() == 0);
	Invalidate();
#endif

}

void CGitStatusListCtrl::Show(DWORD dwShow, const CTGitPathList& checkedList, bool bShowFolders /* = true */)
{
	DeleteAllItems();
	for(int i=0;i<checkedList.GetCount();i++)
		this->AddEntry((CTGitPath *)&checkedList[i],0,i);
	return ;
#if 0

	Locker lock(m_critSec);
	WORD langID = (WORD)CRegStdWORD(_T("Software\\TortoiseGit\\LanguageID"), GetUserDefaultLangID());

	CWinApp * pApp = AfxGetApp();
	if (pApp)
		pApp->DoWaitCursor(1);
	m_dwShow = dwShow;
	m_bShowFolders = bShowFolders;
	m_nSelected = 0;
	int nTopIndex = GetTopIndex();
	POSITION posSelectedEntry = GetFirstSelectedItemPosition();
	int nSelectedEntry = 0;
	if (posSelectedEntry)
		nSelectedEntry = GetNextSelectedItem(posSelectedEntry);
	SetRedraw(FALSE);
	DeleteAllItems();

	PrepareGroups();

	m_arListArray.clear();

	m_arListArray.reserve(m_arStatusArray.size());
	SetItemCount (static_cast<int>(m_arStatusArray.size()));

	int listIndex = 0;
	for (size_t i=0; i < m_arStatusArray.size(); ++i)
	{
		FileEntry * entry = m_arStatusArray[i];
		if ((entry->inexternal) && (!(dwShow & SVNSLC_SHOWINEXTERNALS)))
			continue;
		if ((entry->differentrepo || entry->isNested) && (! (dwShow & SVNSLC_SHOWEXTERNALFROMDIFFERENTREPO)))
			continue;
		if (entry->IsFolder() && (!bShowFolders))
			continue;	// don't show folders if they're not wanted.
#if 0
		git_wc_status_kind status = SVNStatus::GetMoreImportant(entry->status, entry->remotestatus);
		DWORD showFlags = GetShowFlagsFromSVNStatus(status);
		if (entry->IsLocked())
			showFlags |= SVNSLC_SHOWLOCKS;
		if (!entry->changelist.IsEmpty())
			showFlags |= SVNSLC_SHOWINCHANGELIST;

		// status_ignored is a special case - we must have the 'direct' flag set to add a status_ignored item
		if (status != git_wc_status_ignored || (entry->direct) || (dwShow & SVNSLC_SHOWIGNORED))
		{
			for (int npath = 0; npath < checkedList.GetCount(); ++npath)
			{
				if (entry->GetPath().IsEquivalentTo(checkedList[npath]))
				{
					entry->checked = true;
					break;
				}
			}
			if ((!entry->IsFolder()) && (status == git_wc_status_deleted) && (dwShow & SVNSLC_SHOWREMOVEDANDPRESENT))
			{
				if (PathFileExists(entry->GetPath().GetWinPath()))
				{
					m_arListArray.push_back(i);
					AddEntry(entry, langID, listIndex++);
				}
			}
			else if ((dwShow & showFlags)||((dwShow & SVNSLC_SHOWDIRECTFILES)&&(entry->direct)&&(!entry->IsFolder())))
			{
				m_arListArray.push_back(i);
				AddEntry(entry, langID, listIndex++);
			}
			else if ((dwShow & showFlags)||((dwShow & SVNSLC_SHOWDIRECTFOLDER)&&(entry->direct)&&entry->IsFolder()))
			{
				m_arListArray.push_back(i);
				AddEntry(entry, langID, listIndex++);
			}
			else if (entry->switched)
			{
				m_arListArray.push_back(i);
				AddEntry(entry, langID, listIndex++);
			}
		}
#endif
	}

	SetItemCount(listIndex);

	int maxcol = ((CHeaderCtrl*)(GetDlgItem(0)))->GetItemCount()-1;
	for (int col = 0; col <= maxcol; col++)
        SetColumnWidth (col, m_ColumnManager.GetWidth (col, true));

    SetRedraw(TRUE);
	GetStatisticsString();

	CHeaderCtrl * pHeader = GetHeaderCtrl();
	HDITEM HeaderItem = {0};
	HeaderItem.mask = HDI_FORMAT;
	for (int i=0; i<pHeader->GetItemCount(); ++i)
	{
		pHeader->GetItem(i, &HeaderItem);
		HeaderItem.fmt &= ~(HDF_SORTDOWN | HDF_SORTUP);
		pHeader->SetItem(i, &HeaderItem);
	}
	if (m_nSortedColumn)
	{
		pHeader->GetItem(m_nSortedColumn, &HeaderItem);
		HeaderItem.fmt |= (m_bAscending ? HDF_SORTUP : HDF_SORTDOWN);
		pHeader->SetItem(m_nSortedColumn, &HeaderItem);
	}

	if (nSelectedEntry)
	{
		SetItemState(nSelectedEntry, LVIS_SELECTED, LVIS_SELECTED);
		EnsureVisible(nSelectedEntry, false);
	}
	else
	{
		// Restore the item at the top of the list.
		for (int i=0;GetTopIndex() != nTopIndex;i++)
		{
			if ( !EnsureVisible(nTopIndex+i,false) )
			{
				break;
			}
		}
	}

	if (pApp)
		pApp->DoWaitCursor(-1);

	m_bEmpty = (GetItemCount() == 0);
	Invalidate();
#endif

}
int CGitStatusListCtrl::GetColumnIndex(int mask)
{
	int i=0;
	for(i=0;i<32;i++)
		if(mask&0x1)
			return i;
		else
			mask=mask>>1;
	return -1;
}
void CGitStatusListCtrl::AddEntry(CTGitPath * GitPath, WORD langID, int listIndex)
{
	static CString ponly(MAKEINTRESOURCE(IDS_STATUSLIST_PROPONLY));
	static HINSTANCE hResourceHandle(AfxGetResourceHandle());

	CString path = GitPath->GetGitPathString();

	m_bBlock = TRUE;
	TCHAR buf[100];
	int index = listIndex;
	int nCol = 1;
	CString entryname = GitPath->GetGitPathString();
	int icon_idx = 0;
	if (GitPath->IsDirectory())
		icon_idx = m_nIconFolder;
	else
	{
		icon_idx = SYS_IMAGE_LIST().GetPathIconIndex(*GitPath);
	}
	// relative path
	CString rename;
	rename.Format(_T("(from %s)"),GitPath->GetGitOldPathString());
	if(GitPath->m_Action & (CTGitPath::LOGACTIONS_REPLACED|CTGitPath::LOGACTIONS_COPY))
		entryname+=rename;
	
	InsertItem(index, entryname, icon_idx);

	this->SetItemData(index, (DWORD_PTR)GitPath);
	// SVNSLC_COLFILENAME
	SetItemText(index, nCol++, GitPath->GetFileOrDirectoryName());
	// SVNSLC_COLEXT
	SetItemText(index, nCol++, GitPath->GetFileExtension());
	// SVNSLC_COLSTATUS
	SetItemText(index, nCol++, GitPath->GetActionName());

	SetItemText(index, GetColumnIndex(SVNSLC_COLADD),GitPath->m_StatAdd);
	SetItemText(index, GetColumnIndex(SVNSLC_COLDEL),GitPath->m_StatDel);


	SetCheck(index, GitPath->m_Checked);
	if (GitPath->m_Checked)
		m_nSelected++;


	if( GitPath->m_Action & CTGitPath::LOGACTIONS_IGNORE)
		SetItemGroup(index, 2);
	else if( GitPath->m_Action & CTGitPath::LOGACTIONS_UNVER)
		SetItemGroup(index,1);
	else
		SetItemGroup(index,0);
	m_bBlock = FALSE;


}
#if 0
void CGitStatusListCtrl::AddEntry(FileEntry * entry, WORD langID, int listIndex)
{
	static CString ponly(MAKEINTRESOURCE(IDS_STATUSLIST_PROPONLY));
	static HINSTANCE hResourceHandle(AfxGetResourceHandle());

	CString path = entry->GetPath().GetGitPathString();
	if ( m_mapFilenameToChecked.size()!=0 && m_mapFilenameToChecked.find(path) != m_mapFilenameToChecked.end() )
	{
		// The user manually de-/selected an item. We now restore this status
		// when refreshing.
		entry->checked = m_mapFilenameToChecked[path];
	}

	m_bBlock = TRUE;
	TCHAR buf[100];
	int index = listIndex;
	int nCol = 1;
	CString entryname = entry->GetDisplayName();
	int icon_idx = 0;
	if (entry->isfolder)
		icon_idx = m_nIconFolder;
	else
	{
		icon_idx = SYS_IMAGE_LIST().GetPathIconIndex(entry->path);
	}
	// relative path
	InsertItem(index, entryname, icon_idx);
	// SVNSLC_COLFILENAME
	SetItemText(index, nCol++, entry->path.GetFileOrDirectoryName());
	// SVNSLC_COLEXT
	SetItemText(index, nCol++, entry->path.GetFileExtension());
	// SVNSLC_COLSTATUS
	if (entry->isNested)
	{
		CString sTemp(MAKEINTRESOURCE(IDS_STATUSLIST_NESTED));
		SetItemText(index, nCol++, sTemp);
	}
	else
	{
		GitStatus::GetStatusString(hResourceHandle, entry->status, buf, sizeof(buf)/sizeof(TCHAR), (WORD)langID);
		if ((entry->copied)&&(_tcslen(buf)>1))
			_tcscat_s(buf, 100, _T(" (+)"));
		if ((entry->switched)&&(_tcslen(buf)>1))
			_tcscat_s(buf, 100, _T(" (s)"));
#if 0
		if ((entry->status == entry->propstatus)&&
			(entry->status != git_wc_status_normal)&&
			(entry->status != git_wc_status_unversioned)&&
			(!GitStatus::IsImportant(entry->textstatus)))
			_tcscat_s(buf, 100, ponly);
#endif
		SetItemText(index, nCol++, buf);
	}
	// SVNSLC_COLREMOTESTATUS
	if (entry->isNested)
	{
		CString sTemp(MAKEINTRESOURCE(IDS_STATUSLIST_NESTED));
		SetItemText(index, nCol++, sTemp);
	}
	else
	{
#if 0
		SVNStatus::GetStatusString(hResourceHandle, entry->remotestatus, buf, sizeof(buf)/sizeof(TCHAR), (WORD)langID);
		if ((entry->copied)&&(_tcslen(buf)>1))
			_tcscat_s(buf, 100, _T(" (+)"));
		if ((entry->switched)&&(_tcslen(buf)>1))
			_tcscat_s(buf, 100, _T(" (s)"));
		if ((entry->remotestatus == entry->remotepropstatus)&&
			(entry->remotestatus != git_wc_status_none)&&
			(entry->remotestatus != git_wc_status_normal)&&
			(entry->remotestatus != git_wc_status_unversioned)&&
			(!SVNStatus::IsImportant(entry->remotetextstatus)))
			_tcscat_s(buf, 100, ponly);
#endif
		SetItemText(index, nCol++, buf);
	}
	// SVNSLC_COLTEXTSTATUS
	if (entry->isNested)
	{
		CString sTemp(MAKEINTRESOURCE(IDS_STATUSLIST_NESTED));
		SetItemText(index, nCol++, sTemp);
	}
	else
	{
#if 0
		SVNStatus::GetStatusString(hResourceHandle, entry->textstatus, buf, sizeof(buf)/sizeof(TCHAR), (WORD)langID);
		if ((entry->copied)&&(_tcslen(buf)>1))
			_tcscat_s(buf, 100, _T(" (+)"));
		if ((entry->switched)&&(_tcslen(buf)>1))
			_tcscat_s(buf, 100, _T(" (s)"));
#endif
		SetItemText(index, nCol++, buf);
	}
	// SVNSLC_COLPROPSTATUS
	if (entry->isNested)
	{
		SetItemText(index, nCol++, _T(""));
	}
	else
	{
#if 0
		SVNStatus::GetStatusString(hResourceHandle, entry->propstatus, buf, sizeof(buf)/sizeof(TCHAR), (WORD)langID);
		if ((entry->copied)&&(_tcslen(buf)>1))
			_tcscat_s(buf, 100, _T(" (+)"));
		if ((entry->switched)&&(_tcslen(buf)>1))
			_tcscat_s(buf, 100, _T(" (s)"));
#endif
		SetItemText(index, nCol++, buf);
	}
	// SVNSLC_COLREMOTETEXT
	if (entry->isNested)
	{
		SetItemText(index, nCol++, _T(""));
	}
	else
	{
#if 0
		SVNStatus::GetStatusString(hResourceHandle, entry->remotetextstatus, buf, sizeof(buf)/sizeof(TCHAR), (WORD)langID);
		SetItemText(index, nCol++, buf);
#endif
	}
	// SVNSLC_COLREMOTEPROP
	if (entry->isNested)
	{
		SetItemText(index, nCol++, _T(""));
	}
	else
	{
//		SVNStatus::GetStatusString(hResourceHandle, entry->remotepropstatus, buf, sizeof(buf)/sizeof(TCHAR), (WORD)langID);
		SetItemText(index, nCol++, buf);
	}
	// SVNSLC_COLURL
//	SetItemText(index, nCol++, entry->url);
	// SVNSLC_COLLOCK
#if 0
	if (!m_HeadRev.IsHead())
	{
		// we have contacted the repository

		// decision-matrix
		// wc		repository		text
		// ""		""				""
		// ""		UID1			owner
		// UID1		UID1			owner
		// UID1		""				lock has been broken
		// UID1		UID2			lock has been stolen
		if (entry->lock_token.IsEmpty() || (entry->lock_token.Compare(entry->lock_remotetoken)==0))
		{
			if (entry->lock_owner.IsEmpty())
				SetItemText(index, nCol++, entry->lock_remoteowner);
			else
				SetItemText(index, nCol++, entry->lock_owner);
		}
		else if (entry->lock_remotetoken.IsEmpty())
		{
			// broken lock
			CString temp(MAKEINTRESOURCE(IDS_STATUSLIST_LOCKBROKEN));
			SetItemText(index, nCol++, temp);
		}
		else
		{
			// stolen lock
			CString temp;
			temp.Format(IDS_STATUSLIST_LOCKSTOLEN, (LPCTSTR)entry->lock_remoteowner);
			SetItemText(index, nCol++, temp);
		}
	}
	else
		SetItemText(index, nCol++, entry->lock_owner);
	// SVNSLC_COLLOCKCOMMENT
	SetItemText(index, nCol++, entry->lock_comment);
	// SVNSLC_COLAUTHOR
	SetItemText(index, nCol++, entry->last_commit_author);
	// SVNSLC_COLREVISION
	CString temp;
	temp.Format(_T("%ld"), entry->last_commit_rev);
	if (entry->last_commit_rev > 0)
		SetItemText(index, nCol++, temp);
	else
		SetItemText(index, nCol++, _T(""));
	// SVNSLC_COLREMOTEREVISION
	temp.Format(_T("%ld"), entry->remoterev);
	if (entry->remoterev > 0)
		SetItemText(index, nCol++, temp);
	else
		SetItemText(index, nCol++, _T(""));
	// SVNSLC_COLDATE
	TCHAR datebuf[SVN_DATE_BUFFER];
	apr_time_t date = entry->last_commit_date;
	SVN::formatDate(datebuf, date, true);
	if (date)
		SetItemText(index, nCol++, datebuf);
	else
		SetItemText(index, nCol++, _T(""));
	// SVNSLC_COLSVNNEEDSLOCK
    BOOL bFoundSVNNeedsLock = entry->present_props.IsNeedsLockSet();
	CString strSVNNeedsLock = (bFoundSVNNeedsLock) ? _T("*") : _T("");
	SetItemText(index, nCol++, strSVNNeedsLock);
	// SVNSLC_COLCOPYFROM
	if (m_sURL.Compare(entry->copyfrom_url.Left(m_sURL.GetLength()))==0)
		temp = entry->copyfrom_url.Mid(m_sURL.GetLength());
	else
		temp = entry->copyfrom_url;
	SetItemText(index, nCol++, temp);
	// SVNSLC_COLMODIFICATIONDATE
	__int64 filetime = entry->GetPath().GetLastWriteTime();
	if ( (filetime) && (entry->status!=git_wc_status_deleted) )
	{
		FILETIME* f = (FILETIME*)(__int64*)&filetime;
		TCHAR datebuf[SVN_DATE_BUFFER];
		SVN::formatDate(datebuf,*f,true);
		SetItemText(index, nCol++, datebuf);
	}
	else
	{
		SetItemText(index, nCol++, _T(""));
	}

    // user-defined properties
    for ( int i = SVNSLC_NUMCOLUMNS, count = m_ColumnManager.GetColumnCount()
        ; i < count
        ; ++i)
    {
        assert (i == nCol++);
        assert (m_ColumnManager.IsUserProp (i));

        CString name = m_ColumnManager.GetName(i);
        if (entry->present_props.HasProperty (name))
		{
			const CString& propVal = entry->present_props [name];
			if (propVal.IsEmpty())
				SetItemText(index, i, m_sNoPropValueText);
			else
				SetItemText(index, i, propVal);
		}
		else
            SetItemText(index, i, _T(""));
    }

	SetCheck(index, entry->checked);
	if (entry->checked)
		m_nSelected++;
	if (m_changelists.find(entry->changelist) != m_changelists.end())
		SetItemGroup(index, m_changelists[entry->changelist]);
	else
		SetItemGroup(index, 0);
	m_bBlock = FALSE;
#endif
}
#endif
bool CGitStatusListCtrl::SetItemGroup(int item, int groupindex)
{
//	if ((m_dwContextMenus & SVNSLC_POPCHANGELISTS) == NULL)
//		return false;
	if (groupindex < 0)
		return false;
	LVITEM i = {0};
	i.mask = LVIF_GROUPID;
	i.iItem = item;
	i.iSubItem = 0;
	i.iGroupId = groupindex;

	return !!SetItem(&i);
}

void CGitStatusListCtrl::Sort()
{
	Locker lock(m_critSec);

    CSorter predicate (&m_ColumnManager, m_nSortedColumn, m_bAscending);

	std::sort(m_arStatusArray.begin(), m_arStatusArray.end(), predicate);
	SaveColumnWidths();
	Show(m_dwShow, 0, m_bShowFolders);

}

void CGitStatusListCtrl::OnHdnItemclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	*pResult = 0;
	if (m_bBlock)
		return;
	m_bBlock = TRUE;
	if (m_nSortedColumn == phdr->iItem)
		m_bAscending = !m_bAscending;
	else
		m_bAscending = TRUE;
	m_nSortedColumn = phdr->iItem;
	m_mapFilenameToChecked.clear();
	Sort();

	CHeaderCtrl * pHeader = GetHeaderCtrl();
	HDITEM HeaderItem = {0};
	HeaderItem.mask = HDI_FORMAT;
	for (int i=0; i<pHeader->GetItemCount(); ++i)
	{
		pHeader->GetItem(i, &HeaderItem);
		HeaderItem.fmt &= ~(HDF_SORTDOWN | HDF_SORTUP);
		pHeader->SetItem(i, &HeaderItem);
	}
	pHeader->GetItem(m_nSortedColumn, &HeaderItem);
	HeaderItem.fmt |= (m_bAscending ? HDF_SORTUP : HDF_SORTDOWN);
	pHeader->SetItem(m_nSortedColumn, &HeaderItem);

	// the checked state of the list control items must be restored

	for (int i=0; i<GetItemCount(); ++i)
	{
		CTGitPath * entry = (CTGitPath*)GetItemData(i);
		ASSERT(entry);
		if(entry)
			SetCheck(i, entry->m_Checked);
	}

	m_bBlock = FALSE;
}

void CGitStatusListCtrl::OnLvnItemchanging(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

#define ISCHECKED(x) ((x) ? ((((x)&LVIS_STATEIMAGEMASK)>>12)-1) : FALSE)
	if ((m_bBlock)&&(m_bBlockUI))
	{
		// if we're blocked, prevent changing of the check state
		if ((!ISCHECKED(pNMLV->uOldState) && ISCHECKED(pNMLV->uNewState))||
			(ISCHECKED(pNMLV->uOldState) && !ISCHECKED(pNMLV->uNewState)))
			*pResult = TRUE;
	}
}

BOOL CGitStatusListCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
	CWnd* pParent = GetParent();
	if (NULL != pParent && NULL != pParent->GetSafeHwnd())
	{
		pParent->SendMessage(SVNSLNM_ITEMCHANGED, pNMLV->iItem);
	}

	if ((pNMLV->uNewState==0)||(pNMLV->uNewState & LVIS_SELECTED)||(pNMLV->uNewState & LVIS_FOCUSED))
		return FALSE;

	if (m_bBlock)
		return FALSE;

	bool bSelected = !!(ListView_GetItemState(m_hWnd, pNMLV->iItem, LVIS_SELECTED) & LVIS_SELECTED);
	int nListItems = GetItemCount();

	m_bBlock = TRUE;
	// was the item checked?
	
	//CTGitPath *gitpath=(CTGitPath*)GetItemData(pNMLV->iItem);
	//gitpath->m_Checked=GetCheck(pNMLV->iItem);

	if (GetCheck(pNMLV->iItem))
	{
		CheckEntry(pNMLV->iItem, nListItems);
		if (bSelected)
		{
			POSITION pos = GetFirstSelectedItemPosition();
			int index;
			while ((index = GetNextSelectedItem(pos)) >= 0)
			{
				if (index != pNMLV->iItem)
					CheckEntry(index, nListItems);
			}
		}
	}
	else
	{
		UncheckEntry(pNMLV->iItem, nListItems);
		if (bSelected)
		{
			POSITION pos = GetFirstSelectedItemPosition();
			int index;
			while ((index = GetNextSelectedItem(pos)) >= 0)
			{
				if (index != pNMLV->iItem)
					UncheckEntry(index, nListItems);
			}
		}
	}

	GetStatisticsString();
	m_bBlock = FALSE;
	NotifyCheck();

	return FALSE;
}

void CGitStatusListCtrl::OnColumnResized(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER header = reinterpret_cast<LPNMHEADER>(pNMHDR);
    if (   (header != NULL) 
        && (header->iItem >= 0) 
        && (header->iItem < m_ColumnManager.GetColumnCount()))
    {
        m_ColumnManager.ColumnResized (header->iItem);
    }

    *pResult = FALSE;
}

void CGitStatusListCtrl::OnColumnMoved(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER header = reinterpret_cast<LPNMHEADER>(pNMHDR);
	*pResult = TRUE;
    if (   (header != NULL) 
        && (header->iItem >= 0) 
        && (header->iItem < m_ColumnManager.GetColumnCount())
		// only allow the reordering if the column was not moved left of the first
		// visible item - otherwise the 'invisible' columns are not at the far left
		// anymore and we get all kinds of redrawing problems.
		&& (header->pitem)
		&& (header->pitem->iOrder > m_ColumnManager.GetInvisibleCount()))
    {
        m_ColumnManager.ColumnMoved (header->iItem, header->pitem->iOrder);
		*pResult = FALSE;
    }

    Invalidate(FALSE);
}

void CGitStatusListCtrl::CheckEntry(int index, int nListItems)
{
	Locker lock(m_critSec);
	//FileEntry * entry = GetListEntry(index);
	CTGitPath *path=(CTGitPath*)GetItemData(index);
	ASSERT(path != NULL);
	if (path == NULL)
		return;
	SetCheck(index, TRUE);
	//entry = GetListEntry(index);
	// if an unversioned item was checked, then we need to check if
	// the parent folders are unversioned too. If the parent folders actually
	// are unversioned, then check those too.
#if 0
	if (entry->status == git_wc_status_unversioned)
	{
		// we need to check the parent folder too
		const CTGitPath& folderpath = entry->path;
		for (int i=0; i< nListItems; ++i)
		{
			FileEntry * testEntry = GetListEntry(i);
			ASSERT(testEntry != NULL);
			if (testEntry == NULL)
				continue;
			if (!testEntry->checked)
			{
				if (testEntry->path.IsAncestorOf(folderpath) && (!testEntry->path.IsEquivalentTo(folderpath)))
				{
					SetEntryCheck(testEntry,i,true);
					m_nSelected++;
				}
			}
		}
	}
	bool bShift = !!(GetAsyncKeyState(VK_SHIFT) & 0x8000);
	if ( (entry->status == git_wc_status_deleted) || (m_bCheckChildrenWithParent) || (bShift) )
	{
		// if a deleted folder gets checked, we have to check all
		// children of that folder too.
		if (entry->path.IsDirectory())
		{
			SetCheckOnAllDescendentsOf(entry, true);
		}

		// if a deleted file or folder gets checked, we have to
		// check all parents of this item too.
		for (int i=0; i<nListItems; ++i)
		{
			FileEntry * testEntry = GetListEntry(i);
			ASSERT(testEntry != NULL);
			if (testEntry == NULL)
				continue;
			if (!testEntry->checked)
			{
				if (testEntry->path.IsAncestorOf(entry->path) && (!testEntry->path.IsEquivalentTo(entry->path)))
				{
					if ((testEntry->status == git_wc_status_deleted)||(m_bCheckChildrenWithParent))
					{
						SetEntryCheck(testEntry,i,true);
						m_nSelected++;
						// now we need to check all children of this parent folder
						SetCheckOnAllDescendentsOf(testEntry, true);
					}
				}
			}
		}
	}
#endif
	if ( !path->m_Checked )
	{
		path->m_Checked = TRUE;
		m_nSelected++;
	}
}

void CGitStatusListCtrl::UncheckEntry(int index, int nListItems)
{
	Locker lock(m_critSec);
	CTGitPath *path=(CTGitPath*)GetItemData(index);
	ASSERT(path != NULL);
	if (path == NULL)
		return;
	SetCheck(index, FALSE);
	//entry = GetListEntry(index);
	// item was unchecked
#if 0
	if (entry->path.IsDirectory())
	{
		// disable all files within an unselected folder, except when unchecking a folder with property changes
		bool bShift = !!(GetAsyncKeyState(VK_SHIFT) & 0x8000);
		if ( (entry->status != git_wc_status_modified) || (bShift) )
		{
			SetCheckOnAllDescendentsOf(entry, false);
		}
	}
	else if (entry->status == git_wc_status_deleted)
	{
		// a "deleted" file was unchecked, so uncheck all parent folders
		// and all children of those parents
		for (int i=0; i<nListItems; i++)
		{
			FileEntry * testEntry = GetListEntry(i);
			ASSERT(testEntry != NULL);
			if (testEntry == NULL)
				continue;
			if (testEntry->checked)
			{
				if (testEntry->path.IsAncestorOf(entry->path))
				{
					if (testEntry->status == git_wc_status_deleted)
					{
						SetEntryCheck(testEntry,i,false);
						m_nSelected--;

						SetCheckOnAllDescendentsOf(testEntry, false);
					}
				}
			}
		}
	}
#endif
	if ( path->m_Checked )
	{
		path->m_Checked  = FALSE;
		m_nSelected--;
	}
}
#if 0
bool CGitStatusListCtrl::EntryPathCompareNoCase(const FileEntry* pEntry1, const FileEntry* pEntry2)
{
	return pEntry1->path < pEntry2->path;
}

bool CGitStatusListCtrl::IsEntryVersioned(const FileEntry* pEntry1)
{
	return pEntry1->status != git_wc_status_unversioned;
}
#endif
bool CGitStatusListCtrl::BuildStatistics()
{

	bool bRefetchStatus = false;

	// now gather some statistics
	m_nUnversioned = 0;
	m_nNormal = 0;
	m_nModified = 0;
	m_nAdded = 0;
	m_nDeleted = 0;
	m_nConflicted = 0;
	m_nTotal = 0;
	m_nSelected = 0;
	
	for (int i=0; i < (int)m_arStatusArray.size(); ++i)
	{
		int status=((CTGitPath*)m_arStatusArray[i])->m_Action;

		if(status&(CTGitPath::LOGACTIONS_ADDED|CTGitPath::LOGACTIONS_COPY))
			m_nAdded++;
		
		if(status&CTGitPath::LOGACTIONS_DELETED)
			m_nDeleted++;
		
		if(status&(CTGitPath::LOGACTIONS_REPLACED|CTGitPath::LOGACTIONS_MODIFIED))
			m_nModified++;
		
		if(status&CTGitPath::LOGACTIONS_UNMERGED)
			m_nConflicted++;
		
		if(status&(CTGitPath::LOGACTIONS_IGNORE|CTGitPath::LOGACTIONS_UNVER))
			m_nUnversioned++;
	
	

//			} // switch (entry->status)
//		} // if (entry)
	} // for (int i=0; i < (int)m_arStatusArray.size(); ++i)
	return !bRefetchStatus;

	return FALSE;
}

void CGitStatusListCtrl::GetMinMaxRevisions(git_revnum_t& rMin, git_revnum_t& rMax, bool bShownOnly, bool bCheckedOnly)
{
#if 0
	Locker lock(m_critSec);
	rMin = LONG_MAX;
	rMax = 0;

	if ((bShownOnly)||(bCheckedOnly))
	{
		for (int i=0; i<GetItemCount(); ++i)
		{
			const FileEntry * entry = GetListEntry(i);

			if ((entry)&&(entry->last_commit_rev))
			{
				if ((!bCheckedOnly)||(entry->IsChecked()))
				{
					if (entry->last_commit_rev >= 0)
					{
						rMin = min(rMin, entry->last_commit_rev);
						rMax = max(rMax, entry->last_commit_rev);
					}
				}
			}
		}
	}
	else
	{
		for (int i=0; i < (int)m_arStatusArray.size(); ++i)
		{
			const FileEntry * entry = m_arStatusArray[i];
			if ((entry)&&(entry->last_commit_rev))
			{
				if (entry->last_commit_rev >= 0)
				{
					rMin = min(rMin, entry->last_commit_rev);
					rMax = max(rMax, entry->last_commit_rev);
				}
			}
		}
	}
	if (rMin == LONG_MAX)
		rMin = 0;
#endif
}

int CGitStatusListCtrl::GetGroupFromPoint(POINT * ppt)
{
	// the point must be relative to the upper left corner of the control

	if (ppt == NULL)
		return -1;
	if (!IsGroupViewEnabled())
		return -1;

	POINT pt = *ppt;
	pt.x = 10;
	UINT flags = 0;
	int nItem = -1;
	RECT rc;
	GetWindowRect(&rc);
	while (((flags & LVHT_BELOW) == 0)&&(pt.y < rc.bottom))
	{
		nItem = HitTest(pt, &flags);
		if ((flags & LVHT_ONITEM)||(flags & LVHT_EX_GROUP_HEADER))
		{
			// the first item below the point

			// check if the point is too much right (i.e. if the point
			// is farther to the right than the width of the item)
			RECT r;
			GetItemRect(nItem, &r, LVIR_LABEL);
			if (ppt->x > r.right)
				return -1;

			LVITEM lv = {0};
			lv.mask = LVIF_GROUPID;
			lv.iItem = nItem;
			GetItem(&lv);
			int groupID = lv.iGroupId;
			// now we search upwards and check if the item above this one
			// belongs to another group. If it belongs to the same group,
			// we're not over a group header
			while (pt.y >= 0)
			{
				pt.y -= 2;
				nItem = HitTest(pt, &flags);
				if ((flags & LVHT_ONITEM)&&(nItem >= 0))
				{
					// the first item below the point
					LVITEM lv = {0};
					lv.mask = LVIF_GROUPID;
					lv.iItem = nItem;
					GetItem(&lv);
					if (lv.iGroupId != groupID)
						return groupID;
					else
						return -1;
				}
			}
			if (pt.y < 0)
				return groupID;
			return -1;
		}
		pt.y += 2;
	};
	return -1;
}

void CGitStatusListCtrl::OnContextMenuGroup(CWnd * /*pWnd*/, CPoint point)
{
	POINT clientpoint = point;
	ScreenToClient(&clientpoint);
	if ((IsGroupViewEnabled())&&(GetGroupFromPoint(&clientpoint) >= 0))
	{
		CMenu popup;
		if (popup.CreatePopupMenu())
		{
			CString temp;
			temp.LoadString(IDS_STATUSLIST_CHECKGROUP);
			popup.AppendMenu(MF_STRING | MF_ENABLED, IDSVNLC_CHECKGROUP, temp);
			temp.LoadString(IDS_STATUSLIST_UNCHECKGROUP);
			popup.AppendMenu(MF_STRING | MF_ENABLED, IDSVNLC_UNCHECKGROUP, temp);
			int cmd = popup.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN | TPM_NONOTIFY, point.x, point.y, this, 0);
			bool bCheck = false;
			switch (cmd)
			{
			case IDSVNLC_CHECKGROUP:
				bCheck = true;
				// fall through here
			case IDSVNLC_UNCHECKGROUP:
				{
					int group = GetGroupFromPoint(&clientpoint);
					// go through all items and check/uncheck those assigned to the group
					// but block the OnLvnItemChanged handler
					m_bBlock = true;
					LVITEM lv;
					for (int i=0; i<GetItemCount(); ++i)
					{
						SecureZeroMemory(&lv, sizeof(LVITEM));
						lv.mask = LVIF_GROUPID;
						lv.iItem = i;
						GetItem(&lv);

						if (lv.iGroupId == group)
						{
							CTGitPath * entry = (CTGitPath*)GetItemData(i);
							if (entry)
							{
								bool bOldCheck = entry->m_Checked;
								SetEntryCheck(entry, i, bCheck);
								if (bCheck != bOldCheck)
								{
									if (bCheck)
										m_nSelected++;
									else
										m_nSelected--;
								}
							}
						}

					}
					GetStatisticsString();
					NotifyCheck();
					m_bBlock = false;
				}
				break;
			}
		}
	}
}

void CGitStatusListCtrl::OnContextMenuList(CWnd * pWnd, CPoint point)
{

	WORD langID = (WORD)CRegStdWORD(_T("Software\\TortoiseGit\\LanguageID"), GetUserDefaultLangID());

	bool XPorLater = false;
	OSVERSIONINFOEX inf;
	SecureZeroMemory(&inf, sizeof(OSVERSIONINFOEX));
	inf.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((OSVERSIONINFO *)&inf);
	WORD fullver = MAKEWORD(inf.dwMinorVersion, inf.dwMajorVersion);
	if (fullver >= 0x0501)
		XPorLater = true;
	bool bShift = !!(GetAsyncKeyState(VK_SHIFT) & 0x8000);
	CTGitPath * filepath;

	int selIndex = GetSelectionMark();
	if ((point.x == -1) && (point.y == -1))
	{
		CRect rect;
		GetItemRect(selIndex, &rect, LVIR_LABEL);
		ClientToScreen(&rect);
		point = rect.CenterPoint();
	}
	if ((GetSelectedCount() == 0)&&(XPorLater)&&(m_bHasCheckboxes))
	{
		// nothing selected could mean the context menu is requested for
		// a group header
		OnContextMenuGroup(pWnd, point);
	}
	else if (selIndex >= 0)
	{
		//FileEntry * entry = GetListEntry(selIndex);

		filepath = (CTGitPath * )GetItemData(selIndex);

		ASSERT(filepath != NULL);
		if (filepath == NULL)
			return;

		//const CTGitPath& filepath = entry->path;
		int wcStatus = filepath->m_Action;
		// entry is selected, now show the popup menu
		Locker lock(m_critSec);
		CIconMenu popup;
		CMenu changelistSubMenu;
		CMenu ignoreSubMenu;
		if (popup.CreatePopupMenu())
		{
			//Add Menu for verion controled file
		
			if (wcStatus & CTGitPath::LOGACTIONS_UNMERGED)
			{
				if ((m_dwContextMenus & SVNSLC_POPCONFLICT)/*&&(entry->textstatus == git_wc_status_conflicted)*/)
				{
					popup.AppendMenuIcon(IDSVNLC_EDITCONFLICT, IDS_MENUCONFLICT, IDI_CONFLICT);
				}
				if (m_dwContextMenus & SVNSLC_POPRESOLVE)
				{
					popup.AppendMenuIcon(IDSVNLC_RESOLVECONFLICT, IDS_STATUSLIST_CONTEXT_RESOLVED, IDI_RESOLVE);
				}
				if ((m_dwContextMenus & SVNSLC_POPRESOLVE)/*&&(entry->textstatus == git_wc_status_conflicted)*/)
				{
					popup.AppendMenuIcon(IDSVNLC_RESOLVETHEIRS, IDS_SVNPROGRESS_MENUUSETHEIRS, IDI_RESOLVE);
					popup.AppendMenuIcon(IDSVNLC_RESOLVEMINE, IDS_SVNPROGRESS_MENUUSEMINE, IDI_RESOLVE);
				}
				if ((m_dwContextMenus & SVNSLC_POPCONFLICT)||(m_dwContextMenus & SVNSLC_POPRESOLVE))
					popup.AppendMenu(MF_SEPARATOR);
			}

			if (!(wcStatus &CTGitPath::LOGACTIONS_UNVER))
			{
				if (m_dwContextMenus & SVNSLC_POPCOMPAREWITHBASE)
				{
					popup.AppendMenuIcon(IDSVNLC_COMPARE, IDS_LOG_COMPAREWITHBASE, IDI_DIFF);
					popup.SetDefaultItem(IDSVNLC_COMPARE, FALSE);
				}

				if (m_dwContextMenus & this->GetContextMenuBit(IDSVNLC_COMPAREWC))
				{
					if( (!m_CurrentVersion.IsEmpty()) && m_CurrentVersion != GIT_REV_ZERO)
						popup.AppendMenuIcon(IDSVNLC_COMPAREWC, IDS_LOG_POPUP_COMPARE, IDI_DIFF);
				}
				//Select one items
				if (GetSelectedCount() == 1)
				{
					bool bEntryAdded = false;
					//if (entry->remotestatus <= git_wc_status_normal)
					//{
					//	if (wcStatus > git_wc_status_normal)
					//	{
					//		if ((m_dwContextMenus & SVNSLC_POPGNUDIFF)&&(wcStatus != git_wc_status_deleted)&&(wcStatus != git_wc_status_missing))
					//		{
					if(!g_Git.IsInitRepos() && (m_dwContextMenus&SVNSLC_POPGNUDIFF))
					{
						popup.AppendMenuIcon(IDSVNLC_GNUDIFF1, IDS_LOG_POPUP_GNUDIFF, IDI_DIFF);

						bEntryAdded = true;
					}
					//		}
					//	}
					//
					//}
					//else if (wcStatus != git_wc_status_deleted)
					//{
					//	if (m_dwContextMenus & SVNSLC_POPCOMPARE)
					//	{
					//		popup.AppendMenuIcon(IDSVNLC_COMPAREWC, IDS_LOG_POPUP_COMPARE, IDI_DIFF);
					//		popup.SetDefaultItem(IDSVNLC_COMPARE, FALSE);
					//		bEntryAdded = true;
					//	}
					//	if (m_dwContextMenus & SVNSLC_POPGNUDIFF)
					//	{
					//		popup.AppendMenuIcon(IDSVNLC_GNUDIFF1, IDS_LOG_POPUP_GNUDIFF, IDI_DIFF);
					//		bEntryAdded = true;
					//	}
					//}
					if (bEntryAdded)
						popup.AppendMenu(MF_SEPARATOR);
				}
				//else if (GetSelectedCount() > 1)  
				//{
				//	if (m_dwContextMenus & SVNSLC_POPCOMMIT)
				//	{
				//		popup.AppendMenuIcon(IDSVNLC_COMMIT, IDS_STATUSLIST_CONTEXT_COMMIT, IDI_COMMIT);
				//		popup.SetDefaultItem(IDSVNLC_COMPARE, FALSE);
				//	}
				//}
			}
			
			if( (!this->m_Rev1.IsEmpty()) || (!this->m_Rev1.IsEmpty()) )
			{
				if(GetSelectedCount() == 1)
				{
					if (m_dwContextMenus & this->GetContextMenuBit(IDSVNLC_COMPARETWO))
					{	
						popup.AppendMenuIcon(IDSVNLC_COMPARETWO, IDS_LOG_POPUP_COMPARETWO, IDI_DIFF);
						popup.SetDefaultItem(IDSVNLC_COMPARETWO, FALSE);
					}
					if (m_dwContextMenus & this->GetContextMenuBit(IDSVNLC_GNUDIFF2))
					{	
						popup.AppendMenuIcon(IDSVNLC_GNUDIFF2, IDS_LOG_POPUP_GNUDIFF, IDI_DIFF);
					}
				}
			}
	
			///Select Multi item
			//if (GetSelectedCount() > 0)
			//{
			//	if ((GetSelectedCount() == 2)&&(m_dwContextMenus & SVNSLC_POPREPAIRMOVE))
			//	{
			//		POSITION pos = GetFirstSelectedItemPosition();
			//		int index = GetNextSelectedItem(pos);
			//		if (index >= 0)
			//		{
			//			FileEntry * entry = GetListEntry(index);
			//			git_wc_status_kind status1 = git_wc_status_none;
			//			git_wc_status_kind status2 = git_wc_status_none;
			//			if (entry)
			//				status1 = entry->status;
			//			index = GetNextSelectedItem(pos);
			//			if (index >= 0)
			//			{
			//				entry = GetListEntry(index);
			//				if (entry)
			//					status2 = entry->status;
			//				if ((status1 == git_wc_status_missing && status2 == git_wc_status_unversioned) ||
			//					(status2 == git_wc_status_missing && status1 == git_wc_status_unversioned))
			//				{
			//					popup.AppendMenuIcon(IDSVNLC_REPAIRMOVE, IDS_STATUSLIST_CONTEXT_REPAIRMOVE);
			//				}
			//			}
			//		}
			//	}
			//	if (wcStatus > git_wc_status_normal)
			//	{
			//		if (m_dwContextMenus & SVNSLC_POPREVERT)
			//		{
			//			// reverting missing folders is not possible
			//			if (!entry->IsFolder() || (wcStatus != git_wc_status_missing))
			//			{
			//				popup.AppendMenuIcon(IDSVNLC_REVERT, IDS_MENUREVERT, IDI_REVERT);
			//			}
			//		}
			//	}
			//	if (entry->remotestatus > git_wc_status_normal)
			//	{
			//		if (m_dwContextMenus & SVNSLC_POPUPDATE)
			//		{
			//			popup.AppendMenuIcon(IDSVNLC_UPDATE, IDS_MENUUPDATE, IDI_UPDATE);
			//		}
			//	}
			//}

			if ( (GetSelectedCount() >0 ) && (!(wcStatus & CTGitPath::LOGACTIONS_UNVER)))
			{
				if (m_dwContextMenus & SVNSLC_POPREVERT)
				{
					popup.AppendMenuIcon(IDSVNLC_REVERT, IDS_MENUREVERT, IDI_REVERT);
				}

				if ((m_dwContextMenus & GetContextMenuBit(IDSVNLC_REVERTTOREV)) && ( !this->m_CurrentVersion.IsEmpty() )
					&& this->m_CurrentVersion != GIT_REV_ZERO)
				{
					popup.AppendMenuIcon(IDSVNLC_REVERTTOREV, IDS_LOG_POPUP_REVERTTOREV, IDI_REVERT);
				}
			}

			if ((GetSelectedCount() == 1)&&(!(wcStatus & CTGitPath::LOGACTIONS_UNVER))
				&&(!(wcStatus & CTGitPath::LOGACTIONS_IGNORE)))
			{
				if (m_dwContextMenus & SVNSLC_POPSHOWLOG)
				{
					popup.AppendMenuIcon(IDSVNLC_LOG, IDS_REPOBROWSE_SHOWLOG, IDI_LOG);
				}
				if (m_dwContextMenus & SVNSLC_POPBLAME)
				{
					popup.AppendMenuIcon(IDSVNLC_BLAME, IDS_MENUBLAME, IDI_BLAME);
				}
			}
//			if ((wcStatus != git_wc_status_deleted)&&(wcStatus != git_wc_status_missing) && (GetSelectedCount() == 1))
			if ( (GetSelectedCount() == 1) )
			{
				if (m_dwContextMenus & this->GetContextMenuBit(IDSVNLC_SAVEAS) ) 
				{
					popup.AppendMenuIcon(IDSVNLC_SAVEAS, IDS_LOG_POPUP_SAVE, IDI_SAVEAS);
				}

				if (m_dwContextMenus & SVNSLC_POPOPEN)
				{
					popup.AppendMenuIcon(IDSVNLC_VIEWREV, IDS_LOG_POPUP_VIEWREV);
					popup.AppendMenuIcon(IDSVNLC_OPEN, IDS_REPOBROWSE_OPEN, IDI_OPEN);
					popup.AppendMenuIcon(IDSVNLC_OPENWITH, IDS_LOG_POPUP_OPENWITH, IDI_OPEN);
				}
				
				if (m_dwContextMenus & SVNSLC_POPEXPLORE)
				{
					popup.AppendMenuIcon(IDSVNLC_EXPLORE, IDS_STATUSLIST_CONTEXT_EXPLORE, IDI_EXPLORER);
				}
	
			}
			if (GetSelectedCount() > 0)
			{
//				if (((wcStatus == git_wc_status_unversioned)||(wcStatus == git_wc_status_ignored))&&(m_dwContextMenus & SVNSLC_POPDELETE))
//				{
//					popup.AppendMenuIcon(IDSVNLC_DELETE, IDS_MENUREMOVE, IDI_DELETE);
//				}
//				if ((wcStatus != Git_wc_status_unversioned)&&(wcStatus != git_wc_status_ignored)&&(wcStatus != Git_wc_status_deleted)&&(wcStatus != Git_wc_status_added)&&(m_dwContextMenus & GitSLC_POPDELETE))
//				{
//					if (bShift)
//						popup.AppendMenuIcon(IDGitLC_REMOVE, IDS_MENUREMOVEKEEP, IDI_DELETE);
//					else
//						popup.AppendMenuIcon(IDGitLC_REMOVE, IDS_MENUREMOVE, IDI_DELETE);
//				}
				if ((wcStatus & CTGitPath::LOGACTIONS_UNVER)/*||(wcStatus == git_wc_status_deleted)*/)
				{
					if (m_dwContextMenus & SVNSLC_POPADD)
					{
						//if ( entry->IsFolder() )
						//{
						//	popup.AppendMenuIcon(IDSVNLC_ADD_RECURSIVE, IDS_STATUSLIST_CONTEXT_ADD_RECURSIVE, IDI_ADD);
						//}
						//else
						{
							popup.AppendMenuIcon(IDSVNLC_ADD, IDS_STATUSLIST_CONTEXT_ADD, IDI_ADD);
						}
					}

					if (m_dwContextMenus & SVNSLC_POPDELETE)
					{
						popup.AppendMenuIcon(IDSVNLC_DELETE, IDS_MENUREMOVE, IDI_DELETE);
					}
				//}
				//if ( (wcStatus == git_wc_status_unversioned) || (wcStatus == git_wc_status_deleted) )
				//{
					if (m_dwContextMenus & SVNSLC_POPIGNORE)
					{

						CTGitPathList ignorelist;
						FillListOfSelectedItemPaths(ignorelist);
						//check if all selected entries have the same extension
						bool bSameExt = true;
						CString sExt;
						for (int i=0; i<ignorelist.GetCount(); ++i)
						{
							if (sExt.IsEmpty() && (i==0))
								sExt = ignorelist[i].GetFileExtension();
							else if (sExt.CompareNoCase(ignorelist[i].GetFileExtension())!=0)
								bSameExt = false;
						}
						if (bSameExt)
						{
							if (ignoreSubMenu.CreateMenu())
							{
								CString ignorepath;
								if (ignorelist.GetCount()==1)
									ignorepath = ignorelist[0].GetFileOrDirectoryName();
								else
									ignorepath.Format(IDS_MENUIGNOREMULTIPLE, ignorelist.GetCount());
								ignoreSubMenu.AppendMenu(MF_STRING | MF_ENABLED, IDSVNLC_IGNORE, ignorepath);
								ignorepath = _T("*")+sExt;
								ignoreSubMenu.AppendMenu(MF_STRING | MF_ENABLED, IDSVNLC_IGNOREMASK, ignorepath);
								CString temp;
								temp.LoadString(IDS_MENUIGNORE);
								popup.InsertMenu((UINT)-1, MF_BYPOSITION | MF_POPUP, (UINT_PTR)ignoreSubMenu.m_hMenu, temp);
							}
						}
						else
						{
							CString temp;
							if (ignorelist.GetCount()==1)
							{
								temp.LoadString(IDS_MENUIGNORE);
							}
							else
							{
								temp.Format(IDS_MENUIGNOREMULTIPLE, ignorelist.GetCount());
							}
							popup.AppendMenuIcon(IDSVNLC_IGNORE, temp, IDI_IGNORE);
						}
					}
				}
			}


		
			if (GetSelectedCount() > 0)
			{
#if 0	
				if ((!entry->IsFolder())&&(wcStatus >= git_wc_status_normal)
					&&(wcStatus!=git_wc_status_missing)&&(wcStatus!=git_wc_status_deleted)
					&&(wcStatus!=git_wc_status_added))
				{
					popup.AppendMenu(MF_SEPARATOR);
					if ((entry->lock_token.IsEmpty())&&(!entry->IsFolder()))
					{
						if (m_dwContextMenus & SVNSLC_POPLOCK)
						{
							popup.AppendMenuIcon(IDSVNLC_LOCK, IDS_MENU_LOCK, IDI_LOCK);
						}
					}
					if ((!entry->lock_token.IsEmpty())&&(!entry->IsFolder()))
					{
						if (m_dwContextMenus & SVNSLC_POPUNLOCK)
						{
							popup.AppendMenuIcon(IDSVNLC_UNLOCK, IDS_MENU_UNLOCK, IDI_UNLOCK);
						}
					}
				}

				if ((!entry->IsFolder())&&((!entry->lock_token.IsEmpty())||(!entry->lock_remotetoken.IsEmpty())))
				{
					if (m_dwContextMenus & SVNSLC_POPUNLOCKFORCE)
					{
						popup.AppendMenuIcon(IDSVNLC_UNLOCKFORCE, IDS_MENU_UNLOCKFORCE, IDI_UNLOCK);
					}
				}

				if (wcStatus != git_wc_status_missing && wcStatus != git_wc_status_deleted &&wcStatus!=git_wc_status_unversioned)
				{
					popup.AppendMenu(MF_SEPARATOR);
					popup.AppendMenuIcon(IDSVNLC_PROPERTIES, IDS_STATUSLIST_CONTEXT_PROPERTIES, IDI_PROPERTIES);
				}
#endif 
				popup.AppendMenu(MF_SEPARATOR);
				popup.AppendMenuIcon(IDSVNLC_COPY, IDS_STATUSLIST_CONTEXT_COPY, IDI_COPYCLIP);
				popup.AppendMenuIcon(IDSVNLC_COPYEXT, IDS_STATUSLIST_CONTEXT_COPYEXT, IDI_COPYCLIP);
#if 0
				if ((m_dwContextMenus & SVNSLC_POPCHANGELISTS)&&(XPorLater)
					&&(wcStatus != git_wc_status_unversioned)&&(wcStatus != git_wc_status_none))
				{
					popup.AppendMenu(MF_SEPARATOR);
					// changelist commands
					size_t numChangelists = GetNumberOfChangelistsInSelection();
					if (numChangelists > 0)
					{
						popup.AppendMenuIcon(IDSVNLC_REMOVEFROMCS, IDS_STATUSLIST_CONTEXT_REMOVEFROMCS);
					}
					if ((!entry->IsFolder())&&(changelistSubMenu.CreateMenu()))
					{
						CString temp;
						temp.LoadString(IDS_STATUSLIST_CONTEXT_CREATECS);
						changelistSubMenu.AppendMenu(MF_STRING | MF_ENABLED, IDSVNLC_CREATECS, temp);

						if (entry->changelist.Compare(SVNSLC_IGNORECHANGELIST))
						{
							changelistSubMenu.AppendMenu(MF_SEPARATOR);
							changelistSubMenu.AppendMenu(MF_STRING | MF_ENABLED, IDSVNLC_CREATEIGNORECS, SVNSLC_IGNORECHANGELIST);
						}

						if (m_changelists.size() > 0)
						{
							// find the changelist names
							bool bNeedSeparator = true;
							int cmdID = IDSVNLC_MOVETOCS;
							for (std::map<CString, int>::const_iterator it = m_changelists.begin(); it != m_changelists.end(); ++it)
							{
								if ((entry->changelist.Compare(it->first))&&(it->first.Compare(SVNSLC_IGNORECHANGELIST)))
								{
									if (bNeedSeparator)
									{
										changelistSubMenu.AppendMenu(MF_SEPARATOR);
										bNeedSeparator = false;
									}
									changelistSubMenu.AppendMenu(MF_STRING | MF_ENABLED, cmdID, it->first);
									cmdID++;
								}
							}
						}
						temp.LoadString(IDS_STATUSLIST_CONTEXT_MOVETOCS);
						popup.AppendMenu(MF_POPUP|MF_STRING, (UINT_PTR)changelistSubMenu.GetSafeHmenu(), temp);
					}
				}
#endif
			}

			int cmd = popup.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN | TPM_NONOTIFY, point.x, point.y, this, 0);

			m_bBlock = TRUE;
			AfxGetApp()->DoWaitCursor(1);
			int iItemCountBeforeMenuCmd = GetItemCount();
			bool bForce = false;
			switch (cmd)
			{
			case IDSVNLC_VIEWREV:
				OpenFile(filepath,NOTEPAD2);
				break;
			case IDSVNLC_OPEN:
				OpenFile(filepath,OPEN);
				break;
			case IDSVNLC_OPENWITH:
				OpenFile(filepath,OPEN_WITH);
				break;
			case IDSVNLC_EXPLORE:
				{
					ShellExecute(this->m_hWnd, _T("explore"), filepath->GetDirectory().GetWinPath(), NULL, NULL, SW_SHOW);
				}
				break;

			// Compare current version and work copy. 
			case IDSVNLC_COMPAREWC:
				{
					POSITION pos = GetFirstSelectedItemPosition();
					while ( pos )
					{
						int index = GetNextSelectedItem(pos);
						StartDiffWC(index);
					}
				}
				break;
			// Compare with base version. when current version is zero, compare workcopy and HEAD. 
			case IDSVNLC_COMPARE:
				{
					POSITION pos = GetFirstSelectedItemPosition();
					while ( pos )
					{
						int index = GetNextSelectedItem(pos);
						StartDiff(index);
					}
				}
				break;
			case IDSVNLC_COMPARETWO:
				{
					POSITION pos = GetFirstSelectedItemPosition();
					while ( pos )
					{
						int index = GetNextSelectedItem(pos);
						StartDiffTwo(index);
					}
				}
				break;
			case IDSVNLC_GNUDIFF1:
				{
				//	SVNDiff diff(NULL, this->m_hWnd, true);
				//
				//	if (entry->remotestatus <= git_wc_status_normal)
				//		CAppUtils::StartShowUnifiedDiff(m_hWnd, entry->path, SVNRev::REV_BASE, entry->path, SVNRev::REV_WC);
				//	else
				//		CAppUtils::StartShowUnifiedDiff(m_hWnd, entry->path, SVNRev::REV_WC, entry->path, SVNRev::REV_HEAD);
					if(m_CurrentVersion.IsEmpty() || m_CurrentVersion == GIT_REV_ZERO)
						CAppUtils::StartShowUnifiedDiff(m_hWnd,*filepath,GitRev::GetWorkingCopy(),
															*filepath,GitRev::GetHead());
					else
						CAppUtils::StartShowUnifiedDiff(m_hWnd,*filepath,m_CurrentVersion,
															*filepath,m_CurrentVersion+_T("~1"));
				}
				break;
			case IDSVNLC_GNUDIFF2:
				{
				//	SVNDiff diff(NULL, this->m_hWnd, true);
				//
				//	if (entry->remotestatus <= git_wc_status_normal)
				//		CAppUtils::StartShowUnifiedDiff(m_hWnd, entry->path, SVNRev::REV_BASE, entry->path, SVNRev::REV_WC);
				//	else
				//		CAppUtils::StartShowUnifiedDiff(m_hWnd, entry->path, SVNRev::REV_WC, entry->path, SVNRev::REV_HEAD);
					
					CAppUtils::StartShowUnifiedDiff(m_hWnd,*filepath,m_Rev1,
															*filepath,m_Rev2);
				}
				break;
			
			case IDSVNLC_ADD:
				{	// The add went ok, but we now need to run through the selected items again
					// and update their status
					std::vector<int> selectIndex;

					POSITION pos = GetFirstSelectedItemPosition();
					int index;
					while ((index = GetNextSelectedItem(pos)) >= 0)
					{
						selectIndex.push_back(index);
					}
					for(int i=0;i<selectIndex.size();i++)
					{
						index=selectIndex[i];

						CTGitPath * path=(CTGitPath*)GetItemData(index);
						ASSERT(path);
						if(path == NULL)
							continue;
						CString cmd;
						cmd.Format(_T("git.exe add -- \"%s\""),path->GetGitPathString());
						CString output;
						if(!g_Git.Run(cmd,&output,CP_ACP))
						{
							path->m_Action = CTGitPath::LOGACTIONS_ADDED;
							SetEntryCheck(path,index,true);
							SetItemGroup(index,0);
							this->m_StatusFileList.AddPath(*path);
							this->m_UnRevFileList.RemoveItem(*path);
							this->m_IgnoreFileList.RemoveItem(*path);
							Show(this->m_dwShow,0,true,true);
						}
					}
					
				}
				break;

			case IDSVNLC_DELETE:
				{
					//Collect paths
					std::vector<int> selectIndex;

					POSITION pos = GetFirstSelectedItemPosition();
					int index;
					while ((index = GetNextSelectedItem(pos)) >= 0)
					{
						selectIndex.push_back(index);
					}

					//Create file-list ('\0' separated) for SHFileOperation
					CString filelist;
					for(int i=0;i<selectIndex.size();i++)
					{
						index=selectIndex[i];

						CTGitPath * path=(CTGitPath*)GetItemData(index);
						ASSERT(path);
						if(path == NULL)
							continue;

						filelist += path->GetWinPathString();
						filelist += _T("|");
					}
					filelist += _T("|");
					int len = filelist.GetLength();
					TCHAR * buf = new TCHAR[len+2];
					_tcscpy_s(buf, len+2, filelist);
					for (int i=0; i<len; ++i)
						if (buf[i] == '|')
							buf[i] = 0;
					SHFILEOPSTRUCT fileop;
					fileop.hwnd = this->m_hWnd;
					fileop.wFunc = FO_DELETE;
					fileop.pFrom = buf;
					fileop.pTo = NULL;
					fileop.fFlags = FOF_NO_CONNECTED_ELEMENTS | ((GetAsyncKeyState(VK_SHIFT) & 0x8000) ? 0 : FOF_ALLOWUNDO);
					fileop.lpszProgressTitle = _T("deleting file");
					int result = SHFileOperation(&fileop);
					delete [] buf;

					if ( (result==0) && (!fileop.fAnyOperationsAborted) )
					{
						SetRedraw(FALSE);
						POSITION pos = NULL;
						while ((pos = GetFirstSelectedItemPosition()) != 0)
						{
							int index = GetNextSelectedItem(pos);
							if (GetCheck(index))
								m_nSelected--;
							m_nTotal--;

							RemoveListEntry(index);
						}
						SetRedraw(TRUE);
					}
				}
				break;

			case IDSVNLC_BLAME:
				{
					CString sCmd;
					sCmd.Format(_T("\"%s\" /command:blame /path:\"%s\""),
						(LPCTSTR)(CPathUtils::GetAppDirectory()+_T("TortoiseProc.exe")), g_Git.m_CurrentDir+_T("\\")+filepath->GetWinPath());

					CAppUtils::LaunchApplication(sCmd, NULL, false);
				}
				break;

			case IDSVNLC_LOG:
				{
					CString sCmd;
					sCmd.Format(_T("\"%s\" /command:log /path:\"%s\""),
						(LPCTSTR)(CPathUtils::GetAppDirectory()+_T("TortoiseProc.exe")), g_Git.m_CurrentDir+_T("\\")+filepath->GetWinPath());

					CAppUtils::LaunchApplication(sCmd, NULL, false);
				}
				break;

			case IDSVNLC_EDITCONFLICT:
			{
				CAppUtils::ConflictEdit(*filepath,false,this->m_bIsRevertTheirMy);
				break;
			}
			case IDSVNLC_RESOLVETHEIRS: //follow up 
			case IDSVNLC_RESOLVEMINE:   //follow up
			case IDSVNLC_RESOLVECONFLICT:
			{
				if (CMessageBox::Show(m_hWnd, IDS_PROC_RESOLVE, IDS_APPNAME, MB_ICONQUESTION | MB_YESNO)==IDYES)
				{
					POSITION pos = GetFirstSelectedItemPosition();
					while (pos != 0)
					{
						int index;
						index = GetNextSelectedItem(pos);
						CTGitPath * fentry =(CTGitPath*) this->GetItemData(index);
						if(fentry == NULL)
							continue;
						CString gitcmd,output;
						output.Empty();
						int stage=0;
						
						if ( ((!this->m_bIsRevertTheirMy)&&cmd == IDSVNLC_RESOLVETHEIRS) ||
							 ((this->m_bIsRevertTheirMy)&&cmd == IDSVNLC_RESOLVEMINE) )
						{
							gitcmd.Format(_T("git.exe cat-file blob \":3:%s\""),fentry->GetGitPathString());
							if(g_Git.RunLogFile(gitcmd,(CString&)fentry->GetWinPathString()))
							{
								CMessageBox::Show(m_hWnd, output, _T("TortoiseGit"), MB_ICONERROR);
								continue;
							}
						}
						output.Empty();
						if ( ((!this->m_bIsRevertTheirMy)&&cmd == IDSVNLC_RESOLVEMINE) ||
							 ((this->m_bIsRevertTheirMy)&&cmd == IDSVNLC_RESOLVETHEIRS) )
						{
							gitcmd.Format(_T("git.exe cat-file blob \":2:%s\""),fentry->GetGitPathString());
							if(g_Git.RunLogFile(gitcmd,(CString&)fentry->GetWinPathString()))
							{
								CMessageBox::Show(m_hWnd, output, _T("TortoiseGit"), MB_ICONERROR);
								continue;
							}

						}
						output.Empty();
						if ( fentry->m_Action & CTGitPath::LOGACTIONS_UNMERGED)
						{
							gitcmd.Format(_T("git.exe add -- \"%s\""),fentry->GetGitPathString());
							if(g_Git.Run(gitcmd,&output,CP_ACP))
							{
								CMessageBox::Show(m_hWnd, output, _T("TortoiseGit"), MB_ICONERROR);
							}else
							{
								fentry->m_Action |= CTGitPath::LOGACTIONS_MODIFIED;
								fentry->m_Action &=~CTGitPath::LOGACTIONS_UNMERGED;
							}
						}
						
					}
					Show(m_dwShow, 0, m_bShowFolders);
				}
			}
			break;

			case IDSVNLC_IGNORE:
			{
				CTGitPathList ignorelist;
				//std::vector<CString> toremove;
				FillListOfSelectedItemPaths(ignorelist, true);
				SetRedraw(FALSE);

				if(!CAppUtils::IgnoreFile(ignorelist,false))
					break;

				for(int i=0;i<ignorelist.GetCount();i++)
				{
					int nListboxEntries = GetItemCount();
					for (int nItem=0; nItem<nListboxEntries; ++nItem)
					{
						CTGitPath *path=(CTGitPath*)GetItemData(nItem);
						if (path->GetGitPathString()==ignorelist[i].GetGitPathString())
						{
							RemoveListEntry(nItem);
							break;
						}
					}
				}
				SetRedraw(TRUE);
			}
#if 0
					CTSVNPathList ignorelist;
					std::vector<CString> toremove;
					FillListOfSelectedItemPaths(ignorelist, true);
					SetRedraw(FALSE);
					for (int j=0; j<ignorelist.GetCount(); ++j)
					{
						int nListboxEntries = GetItemCount();
						for (int i=0; i<nListboxEntries; ++i)
						{
							if (GetListEntry(i)->GetPath().IsEquivalentTo(ignorelist[j]))
							{
								selIndex = i;
								break;
							}
						}
						CString name = CPathUtils::PathPatternEscape(ignorelist[j].GetFileOrDirectoryName());
						CTSVNPath parentfolder = ignorelist[j].GetContainingDirectory();
						SVNProperties props(parentfolder, SVNRev::REV_WC, false);
						CStringA value;
						for (int i=0; i<props.GetCount(); i++)
						{
							CString propname(props.GetItemName(i).c_str());
							if (propname.CompareNoCase(_T("git:ignore"))==0)
							{
								stdstring stemp;
								// treat values as normal text even if they're not
								value = (char *)props.GetItemValue(i).c_str();
							}
						}
						if (value.IsEmpty())
							value = name;
						else
						{
							value = value.Trim("\n\r");
							value += "\n";
							value += name;
							value.Remove('\r');
						}
						if (!props.Add(_T("git:ignore"), (LPCSTR)value))
						{
							CString temp;
							temp.Format(IDS_ERR_FAILEDIGNOREPROPERTY, (LPCTSTR)name);
							CMessageBox::Show(this->m_hWnd, temp, _T("TortoiseGit"), MB_ICONERROR);
							break;
						}
						if (GetCheck(selIndex))
							m_nSelected--;
						m_nTotal--;

						// now, if we ignored a folder, remove all its children
						if (ignorelist[j].IsDirectory())
						{
							for (int i=0; i<(int)m_arListArray.size(); ++i)
							{
								FileEntry * entry = GetListEntry(i);
								if (entry->status == git_wc_status_unversioned)
								{
									if (!ignorelist[j].IsEquivalentTo(entry->GetPath())&&(ignorelist[j].IsAncestorOf(entry->GetPath())))
									{
										entry->status = git_wc_status_ignored;
										entry->textstatus = git_wc_status_ignored;
										if (GetCheck(i))
											m_nSelected--;
										toremove.push_back(entry->GetPath().GetSVNPathString());
									}
								}
							}
						}

						CTSVNPath basepath = m_arStatusArray[m_arListArray[selIndex]]->basepath;

						FileEntry * entry = m_arStatusArray[m_arListArray[selIndex]];
						if ( entry->status == git_wc_status_unversioned ) // keep "deleted" items
							toremove.push_back(entry->GetPath().GetSVNPathString());

						if (!m_bIgnoreRemoveOnly)
						{
							SVNStatus status;
							git_wc_status2_t * s;
							CTSVNPath gitPath;
							s = status.GetFirstFileStatus(parentfolder, gitPath, false, git_depth_empty);
							// first check if the folder isn't already present in the list
							bool bFound = false;
							nListboxEntries = GetItemCount();
							for (int i=0; i<nListboxEntries; ++i)
							{
								FileEntry * entry = GetListEntry(i);
								if (entry->path.IsEquivalentTo(gitPath))
								{
									bFound = true;
									break;
								}
							}
							if (!bFound)
							{
								if (s!=0)
								{
									FileEntry * entry = new FileEntry();
									entry->path = gitPath;
									entry->basepath = basepath;
									entry->status = SVNStatus::GetMoreImportant(s->text_status, s->prop_status);
									entry->textstatus = s->text_status;
									entry->propstatus = s->prop_status;
									entry->remotestatus = SVNStatus::GetMoreImportant(s->repos_text_status, s->repos_prop_status);
									entry->remotetextstatus = s->repos_text_status;
									entry->remotepropstatus = s->repos_prop_status;
									entry->inunversionedfolder = FALSE;
									entry->checked = true;
									entry->inexternal = false;
									entry->direct = false;
									entry->isfolder = true;
									entry->last_commit_date = 0;
									entry->last_commit_rev = 0;
									entry->remoterev = 0;
									if (s->entry)
									{
										if (s->entry->url)
										{
											entry->url = CUnicodeUtils::GetUnicode(CPathUtils::PathUnescape(s->entry->url));
										}
									}
									if (s->entry && s->entry->present_props)
									{
										entry->present_props = s->entry->present_props;
									}
									m_arStatusArray.push_back(entry);
									m_arListArray.push_back(m_arStatusArray.size()-1);
									AddEntry(entry, langID, GetItemCount());
								}
							}
						}
					}
					for (std::vector<CString>::iterator it = toremove.begin(); it != toremove.end(); ++it)
					{
						int nListboxEntries = GetItemCount();
						for (int i=0; i<nListboxEntries; ++i)
						{
							if (GetListEntry(i)->path.GetSVNPathString().Compare(*it)==0)
							{
								RemoveListEntry(i);
								break;
							}
						}
					}
					SetRedraw(TRUE);
				}
#endif
				break;
			case IDSVNLC_IGNOREMASK:
				{
					CString common;
					CString ext=filepath->GetFileExtension();
					CTGitPathList ignorelist;
					FillListOfSelectedItemPaths(ignorelist, true);
					SetRedraw(FALSE);

					CAppUtils::IgnoreFile(ignorelist,true);
					common=ignorelist.GetCommonRoot().GetGitPathString();

					for (int i=0; i< GetItemCount(); ++i)
					{
						CTGitPath *path=(CTGitPath*)GetItemData(i);
						if(!( path->m_Action & CTGitPath::LOGACTIONS_UNVER))
							continue;
						if( path->GetGitPathString().Left(common.GetLength()) == common )
						{
							if (path->GetFileExtension()==ext)
							{
								RemoveListEntry(i);
								i--; // remove index i at item, new one will replace. 
							}
						}
					}
					
					SetRedraw(TRUE);
				}
#if 0
					std::set<CTSVNPath> parentlist;
					for (int i=0; i<ignorelist.GetCount(); ++i)
					{
						parentlist.insert(ignorelist[i].GetContainingDirectory());
					}
					std::set<CTSVNPath>::iterator it;
					std::vector<CString> toremove;
					
					for (it = parentlist.begin(); it != parentlist.end(); ++it)
					{
						CTSVNPath parentFolder = (*it).GetDirectory();
						SVNProperties props(parentFolder, SVNRev::REV_WC, false);
						CStringA value;
						for (int i=0; i<props.GetCount(); i++)
						{
							CString propname(props.GetItemName(i).c_str());
							if (propname.CompareNoCase(_T("git:ignore"))==0)
							{
								stdstring stemp;
								// treat values as normal text even if they're not
								value = (char *)props.GetItemValue(i).c_str();
							}
						}
						if (value.IsEmpty())
							value = name;
						else
						{
							value = value.Trim("\n\r");
							value += "\n";
							value += name;
							value.Remove('\r');
						}
						if (!props.Add(_T("git:ignore"), (LPCSTR)value))
						{
							CString temp;
							temp.Format(IDS_ERR_FAILEDIGNOREPROPERTY, (LPCTSTR)name);
							CMessageBox::Show(this->m_hWnd, temp, _T("TortoiseGit"), MB_ICONERROR);
						}
						else
						{
							CTSVNPath basepath;
							int nListboxEntries = GetItemCount();
							for (int i=0; i<nListboxEntries; ++i)
							{
								FileEntry * entry = GetListEntry(i);
								ASSERT(entry != NULL);
								if (entry == NULL)
									continue;
								if (basepath.IsEmpty())
									basepath = entry->basepath;
								// since we ignored files with a mask (e.g. *.exe)
								// we have to find find all files in the same
								// folder (IsAncestorOf() returns TRUE for _all_ children,
								// not just the immediate ones) which match the
								// mask and remove them from the list too.
								if ((entry->status == git_wc_status_unversioned)&&(parentFolder.IsAncestorOf(entry->path)))
								{
									CString f = entry->path.GetSVNPathString();
									if (f.Mid(parentFolder.GetSVNPathString().GetLength()).Find('/')<=0)
									{
										if (CStringUtils::WildCardMatch(name, f))
										{
											if (GetCheck(i))
												m_nSelected--;
											m_nTotal--;
											toremove.push_back(f);
										}
									}
								}
							}
							if (!m_bIgnoreRemoveOnly)
							{
								SVNStatus status;
								git_wc_status2_t * s;
								CTSVNPath gitPath;
								s = status.GetFirstFileStatus(parentFolder, gitPath, false, git_depth_empty);
								if (s!=0)
								{
									// first check if the folder isn't already present in the list
									bool bFound = false;
									for (int i=0; i<nListboxEntries; ++i)
									{
										FileEntry * entry = GetListEntry(i);
										if (entry->path.IsEquivalentTo(gitPath))
										{
											bFound = true;
											break;
										}
									}
									if (!bFound)
									{
										FileEntry * entry = new FileEntry();
										entry->path = gitPath;
										entry->basepath = basepath;
										entry->status = SVNStatus::GetMoreImportant(s->text_status, s->prop_status);
										entry->textstatus = s->text_status;
										entry->propstatus = s->prop_status;
										entry->remotestatus = SVNStatus::GetMoreImportant(s->repos_text_status, s->repos_prop_status);
										entry->remotetextstatus = s->repos_text_status;
										entry->remotepropstatus = s->repos_prop_status;
										entry->inunversionedfolder = false;
										entry->checked = true;
										entry->inexternal = false;
										entry->direct = false;
										entry->isfolder = true;
										entry->last_commit_date = 0;
										entry->last_commit_rev = 0;
										entry->remoterev = 0;
										if (s->entry)
										{
											if (s->entry->url)
											{
												entry->url = CUnicodeUtils::GetUnicode(CPathUtils::PathUnescape(s->entry->url));
											}
										}
										if (s->entry && s->entry->present_props)
										{
											entry->present_props = s->entry->present_props;
										}
										m_arStatusArray.push_back(entry);
										m_arListArray.push_back(m_arStatusArray.size()-1);
										AddEntry(entry, langID, GetItemCount());
									}
								}
							}
						}
					}
					for (std::vector<CString>::iterator it = toremove.begin(); it != toremove.end(); ++it)
					{
						int nListboxEntries = GetItemCount();
						for (int i=0; i<nListboxEntries; ++i)
						{
							if (GetListEntry(i)->path.GetSVNPathString().Compare(*it)==0)
							{
								RemoveListEntry(i);
								break;
							}
						}
					}
					SetRedraw(TRUE);
				}
#endif
				break;
			
				case IDSVNLC_REVERT:
				{
					// If at least one item is not in the status "added"
					// we ask for a confirmation
					BOOL bConfirm = FALSE;
					POSITION pos = GetFirstSelectedItemPosition();
					int index;
					while ((index = GetNextSelectedItem(pos)) >= 0)
					{
						//FileEntry * fentry = GetListEntry(index);
						CTGitPath *fentry=(CTGitPath*)GetItemData(index);
						if(fentry && fentry->m_Action &CTGitPath::LOGACTIONS_MODIFIED )
						{
							bConfirm = TRUE;
							break;
						}
					}	

					CString str;
					str.Format(IDS_PROC_WARNREVERT,GetSelectedCount());

					if (!bConfirm || CMessageBox::Show(this->m_hWnd, str, _T("TortoiseGit"), MB_YESNO | MB_ICONQUESTION)==IDYES)
					{
						CTGitPathList targetList;
						FillListOfSelectedItemPaths(targetList);

						// make sure that the list is reverse sorted, so that
						// children are removed before any parents
						targetList.SortByPathname(true);

						// put all reverted files in the trashbin, except the ones with 'added'
						// status because they are not restored by the revert.
						CTGitPathList delList;
						POSITION pos = GetFirstSelectedItemPosition();
						int index;
						while ((index = GetNextSelectedItem(pos)) >= 0)
						{
							CTGitPath *entry=(CTGitPath *)GetItemData(index);
							if (entry&&(!(entry->m_Action& CTGitPath::LOGACTIONS_ADDED)))
								delList.AddPath(*entry);
						}
						if (DWORD(CRegDWORD(_T("Software\\TortoiseGit\\RevertWithRecycleBin"), TRUE)))
							delList.DeleteAllFiles(true);

						if (g_Git.Revert(targetList))
						{
							CMessageBox::Show(this->m_hWnd, _T("Revert Fail"), _T("TortoiseGit"), MB_ICONERROR);
						}
						else
						{
							for(int i=0;i<targetList.GetCount();i++)
							{	
								int nListboxEntries = GetItemCount();
								for (int nItem=0; nItem<nListboxEntries; ++nItem)
								{
									CTGitPath *path=(CTGitPath*)GetItemData(nItem);
									if (path->GetGitPathString()==targetList[i].GetGitPathString())
									{
										if(path->m_Action & CTGitPath::LOGACTIONS_ADDED)
										{
											path->m_Action = CTGitPath::LOGACTIONS_UNVER;
											SetEntryCheck(path,nItem,false);
											PrepareGroups(true);
											SetItemGroup(nItem,1);
											this->m_StatusFileList.RemoveItem(*path);
											this->m_UnRevFileList.AddPath(*path);
											//this->m_IgnoreFileList.RemoveItem(*path);

										}else
										{
											RemoveListEntry(nItem);
										}
										break;
									}
								}
							}
							SetRedraw(TRUE);
							SaveColumnWidths();
							Show(m_dwShow, 0, m_bShowFolders);
							NotifyCheck();
						}
					}
				}
				break;

			case IDSVNLC_COPY:
				CopySelectedEntriesToClipboard(0);
				break;
			case IDSVNLC_COPYEXT:
				CopySelectedEntriesToClipboard((DWORD)-1);
				break;

			case IDSVNLC_SAVEAS:
				FileSaveAs(filepath);
				break;

			case IDSVNLC_REVERTTOREV:
				RevertSelectedItemToVersion();
				break;
#if 0
			case IDSVNLC_PROPERTIES:
				{
					CTSVNPathList targetList;
					FillListOfSelectedItemPaths(targetList);
					CEditPropertiesDlg dlg;
					dlg.SetPathList(targetList);
					dlg.DoModal();
					if (dlg.HasChanged())
					{
						// since the user might have changed/removed/added
						// properties recursively, we don't really know
						// which items have changed their status.
						// So tell the parent to do a refresh.
						CWnd* pParent = GetParent();
						if (NULL != pParent && NULL != pParent->GetSafeHwnd())
						{
							pParent->SendMessage(SVNSLNM_NEEDSREFRESH);
						}
					}
				}
				break;
			case IDSVNLC_COMMIT:
				{
					CTSVNPathList targetList;
					FillListOfSelectedItemPaths(targetList);
					CTSVNPath tempFile = CTempFiles::Instance().GetTempFilePath(false);
					VERIFY(targetList.WriteToFile(tempFile.GetWinPathString()));
					CString commandline = CPathUtils::GetAppDirectory();
					commandline += _T("TortoiseProc.exe /command:commit /pathfile:\"");
					commandline += tempFile.GetWinPathString();
					commandline += _T("\"");
					commandline += _T(" /deletepathfile");
					CAppUtils::LaunchApplication(commandline, NULL, false);
				}
				break;
		
		case IDSVNLC_COMPAREWC:
				{
					POSITION pos = GetFirstSelectedItemPosition();
					while ( pos )
					{
						int index = GetNextSelectedItem(pos);
						FileEntry * entry = GetListEntry(index);
						ASSERT(entry != NULL);
						if (entry == NULL)
							continue;
						SVNDiff diff(NULL, m_hWnd, true);
						diff.SetAlternativeTool(!!(GetAsyncKeyState(VK_SHIFT) & 0x8000));
						git_revnum_t baseRev = entry->Revision;
						diff.DiffFileAgainstBase(
							entry->path, baseRev, entry->textstatus, entry->propstatus);
					}
				}
				break;
			case IDSVNLC_GNUDIFF1:
				{
					SVNDiff diff(NULL, this->m_hWnd, true);

					if (entry->remotestatus <= git_wc_status_normal)
						CAppUtils::StartShowUnifiedDiff(m_hWnd, entry->path, SVNRev::REV_BASE, entry->path, SVNRev::REV_WC);
					else
						CAppUtils::StartShowUnifiedDiff(m_hWnd, entry->path, SVNRev::REV_WC, entry->path, SVNRev::REV_HEAD);
				}
				break;
			case IDSVNLC_UPDATE:
				{
					CTSVNPathList targetList;
					FillListOfSelectedItemPaths(targetList);
					bool bAllExist = true;
					for (int i=0; i<targetList.GetCount(); ++i)
					{
						if (!targetList[i].Exists())
						{
							bAllExist = false;
							break;
						}
					}
					if (bAllExist)
					{
						CSVNProgressDlg dlg;
						dlg.SetCommand(CSVNProgressDlg::SVNProgress_Update);
						dlg.SetPathList(targetList);
						dlg.SetRevision(SVNRev::REV_HEAD);
						dlg.DoModal();
					}
					else
					{
						CString sTempFile = CTempFiles::Instance().GetTempFilePath(false).GetWinPathString();
						targetList.WriteToFile(sTempFile, false);
						CString sCmd;
						sCmd.Format(_T("\"%s\" /command:update /rev /pathfile:\"%s\" /deletepathfile"),
							(LPCTSTR)(CPathUtils::GetAppDirectory()+_T("TortoiseProc.exe")), (LPCTSTR)sTempFile);

						CAppUtils::LaunchApplication(sCmd, NULL, false);
					}
				}
				break;
	
			case IDSVNLC_REMOVE:
				{
					SVN git;
					CTSVNPathList itemsToRemove;
					FillListOfSelectedItemPaths(itemsToRemove);

					// We must sort items before removing, so that files are always removed
					// *before* their parents
					itemsToRemove.SortByPathname(true);

					bool bSuccess = false;
					if (git.Remove(itemsToRemove, FALSE, !!(GetAsyncKeyState(VK_SHIFT) & 0x8000)))
					{
						bSuccess = true;
					}
					else
					{
						if ((git.Err->apr_err == SVN_ERR_UNVERSIONED_RESOURCE) ||
							(git.Err->apr_err == SVN_ERR_CLIENT_MODIFIED))
						{
							CString msg, yes, no, yestoall;
							msg.Format(IDS_PROC_REMOVEFORCE, (LPCTSTR)git.GetLastErrorMessage());
							yes.LoadString(IDS_MSGBOX_YES);
							no.LoadString(IDS_MSGBOX_NO);
							yestoall.LoadString(IDS_PROC_YESTOALL);
							UINT ret = CMessageBox::Show(m_hWnd, msg, _T("TortoiseGit"), 2, IDI_ERROR, yes, no, yestoall);
							if ((ret == 1)||(ret==3))
							{
								if (!git.Remove(itemsToRemove, TRUE, !!(GetAsyncKeyState(VK_SHIFT) & 0x8000)))
								{
									CMessageBox::Show(m_hWnd, git.GetLastErrorMessage(), _T("TortoiseGit"), MB_ICONERROR);
								}
								else
									bSuccess = true;
							}
						}
						else
							CMessageBox::Show(m_hWnd, git.GetLastErrorMessage(), _T("TortoiseGit"), MB_ICONERROR);
					}
					if (bSuccess)
					{
						// The remove went ok, but we now need to run through the selected items again
						// and update their status
						POSITION pos = GetFirstSelectedItemPosition();
						int index;
						std::vector<int> entriesToRemove;
						while ((index = GetNextSelectedItem(pos)) >= 0)
						{
							FileEntry * e = GetListEntry(index);
							if (!bShift &&
								((e->textstatus == git_wc_status_unversioned)||
								(e->textstatus == git_wc_status_none)||
								(e->textstatus == git_wc_status_ignored)))
							{
								if (GetCheck(index))
									m_nSelected--;
								m_nTotal--;
								entriesToRemove.push_back(index);
							}
							else
							{
								e->textstatus = git_wc_status_deleted;
								e->status = git_wc_status_deleted;
								SetEntryCheck(e,index,true);
							}
						}
						for (std::vector<int>::reverse_iterator it = entriesToRemove.rbegin(); it != entriesToRemove.rend(); ++it)
						{
							RemoveListEntry(*it);
						}
					}
					SaveColumnWidths();
					Show(m_dwShow, 0, m_bShowFolders);
					NotifyCheck();
				}
				break;
			case IDSVNLC_DELETE:
				{
					CTSVNPathList pathlist;
					FillListOfSelectedItemPaths(pathlist);
					pathlist.RemoveChildren();
					CString filelist;
					for (INT_PTR i=0; i<pathlist.GetCount(); ++i)
					{
						filelist += pathlist[i].GetWinPathString();
						filelist += _T("|");
					}
					filelist += _T("|");
					int len = filelist.GetLength();
					TCHAR * buf = new TCHAR[len+2];
					_tcscpy_s(buf, len+2, filelist);
					for (int i=0; i<len; ++i)
						if (buf[i] == '|')
							buf[i] = 0;
					SHFILEOPSTRUCT fileop;
					fileop.hwnd = this->m_hWnd;
					fileop.wFunc = FO_DELETE;
					fileop.pFrom = buf;
					fileop.pTo = NULL;
					fileop.fFlags = FOF_NO_CONNECTED_ELEMENTS | ((GetAsyncKeyState(VK_SHIFT) & 0x8000) ? 0 : FOF_ALLOWUNDO);
					fileop.lpszProgressTitle = _T("deleting file");
					int result = SHFileOperation(&fileop);
					delete [] buf;

					if ( (result==0) && (!fileop.fAnyOperationsAborted) )
					{
						SetRedraw(FALSE);
						POSITION pos = NULL;
						CTSVNPathList deletedlist;	// to store list of deleted folders
						while ((pos = GetFirstSelectedItemPosition()) != 0)
						{
							int index = GetNextSelectedItem(pos);
							if (GetCheck(index))
								m_nSelected--;
							m_nTotal--;
							FileEntry * fentry = GetListEntry(index);
							if ((fentry)&&(fentry->isfolder))
								deletedlist.AddPath(fentry->path);
							RemoveListEntry(index);
						}
						// now go through the list of deleted folders
						// and remove all their children from the list too!
						int nListboxEntries = GetItemCount();
						for (int folderindex = 0; folderindex < deletedlist.GetCount(); ++folderindex)
						{
							CTSVNPath folderpath = deletedlist[folderindex];
							for (int i=0; i<nListboxEntries; ++i)
							{
								FileEntry * entry = GetListEntry(i);
								if (folderpath.IsAncestorOf(entry->path))
								{
									RemoveListEntry(i--);
									nListboxEntries--;
								}
							}
						}
						SetRedraw(TRUE);
					}
				}
				break;


			case IDSVNLC_ADD:
				{
					SVN git;
					CTSVNPathList itemsToAdd;
					FillListOfSelectedItemPaths(itemsToAdd);

					// We must sort items before adding, so that folders are always added
					// *before* any of their children
					itemsToAdd.SortByPathname();

					ProjectProperties props;
					props.ReadPropsPathList(itemsToAdd);
					if (git.Add(itemsToAdd, &props, git_depth_empty, TRUE, TRUE, TRUE))
					{
						// The add went ok, but we now need to run through the selected items again
						// and update their status
						POSITION pos = GetFirstSelectedItemPosition();
						int index;
						while ((index = GetNextSelectedItem(pos)) >= 0)
						{
							FileEntry * e = GetListEntry(index);
							e->textstatus = git_wc_status_added;
							e->propstatus = git_wc_status_none;
							e->status = git_wc_status_added;
							SetEntryCheck(e,index,true);
						}
					}
					else
					{
						CMessageBox::Show(m_hWnd, git.GetLastErrorMessage(), _T("TortoiseGit"), MB_ICONERROR);
					}
					SaveColumnWidths();
					Show(m_dwShow, 0, m_bShowFolders);
					NotifyCheck();
				}
				break;
			case IDSVNLC_ADD_RECURSIVE:
				{
					CTSVNPathList itemsToAdd;
					FillListOfSelectedItemPaths(itemsToAdd);

					CAddDlg dlg;
					dlg.m_pathList = itemsToAdd;
					if (dlg.DoModal() == IDOK)
					{
						if (dlg.m_pathList.GetCount() == 0)
							break;
						CSVNProgressDlg progDlg;
						progDlg.SetCommand(CSVNProgressDlg::SVNProgress_Add);
						progDlg.SetPathList(dlg.m_pathList);
						ProjectProperties props;
						props.ReadPropsPathList(dlg.m_pathList);
						progDlg.SetProjectProperties(props);
						progDlg.SetItemCount(dlg.m_pathList.GetCount());
						progDlg.DoModal();

						// refresh!
						CWnd* pParent = GetParent();
						if (NULL != pParent && NULL != pParent->GetSafeHwnd())
						{
							pParent->SendMessage(SVNSLNM_NEEDSREFRESH);
						}
					}
				}
				break;

			case IDSVNLC_REPAIRMOVE:
				{
					POSITION pos = GetFirstSelectedItemPosition();
					int index = GetNextSelectedItem(pos);
					FileEntry * entry1 = NULL;
					FileEntry * entry2 = NULL;
					if (index >= 0)
					{
						entry1 = GetListEntry(index);
						git_wc_status_kind status1 = git_wc_status_none;
						git_wc_status_kind status2 = git_wc_status_none;
						if (entry1)
						{
							status1 = entry1->status;
							index = GetNextSelectedItem(pos);
							if (index >= 0)
							{
								entry2 = GetListEntry(index);
								if (entry2)
								{
									status2 = entry2->status;
									if (status2 == git_wc_status_missing && status1 == git_wc_status_unversioned)
									{
										FileEntry * tempentry = entry1;
										entry1 = entry2;
										entry2 = tempentry;
									}
									// entry1 was renamed to entry2 but outside of Subversion
									// fix this by moving entry2 back to entry1 first,
									// then do an git-move from entry1 to entry2
									if (MoveFile(entry2->GetPath().GetWinPath(), entry1->GetPath().GetWinPath()))
									{
										SVN git;
										if (!git.Move(CTSVNPathList(entry1->GetPath()), entry2->GetPath(), TRUE))
										{
											CMessageBox::Show(m_hWnd, git.GetLastErrorMessage(), _T("TortoiseGit"), MB_ICONERROR);
										}
										else
										{
											// check the previously unversioned item
											entry1->checked = true;
											// fixing the move was successful. We have to adjust the new status of the
											// files.
											// Since we don't know if the moved/renamed file had local modifications or not,
											// we can't guess the new status. That means we have to refresh...
											CWnd* pParent = GetParent();
											if (NULL != pParent && NULL != pParent->GetSafeHwnd())
											{
												pParent->SendMessage(SVNSLNM_NEEDSREFRESH);
											}
										}
									}
									else
									{
										LPVOID lpMsgBuf;
										FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
											FORMAT_MESSAGE_FROM_SYSTEM | 
											FORMAT_MESSAGE_IGNORE_INSERTS,
											NULL,
											GetLastError(),
											MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
											(LPTSTR) &lpMsgBuf,
											0,
											NULL 
											);
										MessageBox((LPCTSTR)lpMsgBuf, _T("Error"), MB_OK | MB_ICONINFORMATION );
										LocalFree( lpMsgBuf );
									}
								}
							}
						}
					}
				}
				break;
			case IDSVNLC_REMOVEFROMCS:
				{
					CTSVNPathList changelistItems;
					FillListOfSelectedItemPaths(changelistItems);
					SVN git;
					SetRedraw(FALSE);
					if (git.RemoveFromChangeList(changelistItems, CStringArray(), git_depth_empty))
					{
						// The changelists were removed, but we now need to run through the selected items again
						// and update their changelist
						POSITION pos = GetFirstSelectedItemPosition();
						int index;
						std::vector<int> entriesToRemove;
						while ((index = GetNextSelectedItem(pos)) >= 0)
						{
							FileEntry * e = GetListEntry(index);
							if (e)
							{
								e->changelist.Empty();
								if (e->status == git_wc_status_normal)
								{
									// remove the entry completely
									entriesToRemove.push_back(index);
								}
								else
									SetItemGroup(index, 0);
							}
						}
						for (std::vector<int>::reverse_iterator it = entriesToRemove.rbegin(); it != entriesToRemove.rend(); ++it)
						{
							RemoveListEntry(*it);
						}
						// TODO: Should we go through all entries here and check if we also could
						// remove the changelist from m_changelists ?
					}
					else
					{
						CMessageBox::Show(m_hWnd, git.GetLastErrorMessage(), _T("TortoiseGit"), MB_ICONERROR);
					}
					SetRedraw(TRUE);
				}
				break;
			case IDSVNLC_CREATEIGNORECS:
				CreateChangeList(SVNSLC_IGNORECHANGELIST);
				break;
			case IDSVNLC_CREATECS:
				{
					CCreateChangelistDlg dlg;
					if (dlg.DoModal() == IDOK)
					{
						CreateChangeList(dlg.m_sName);
					}
				}
				break;
			default:
				{
					if (cmd < IDSVNLC_MOVETOCS)
						break;
					CTSVNPathList changelistItems;
					FillListOfSelectedItemPaths(changelistItems);

					// find the changelist name
					CString sChangelist;
					int cmdID = IDSVNLC_MOVETOCS;
					SetRedraw(FALSE);
					for (std::map<CString, int>::const_iterator it = m_changelists.begin(); it != m_changelists.end(); ++it)
					{
						if ((it->first.Compare(SVNSLC_IGNORECHANGELIST))&&(entry->changelist.Compare(it->first)))
						{
							if (cmd == cmdID)
							{
								sChangelist = it->first;
							}
							cmdID++;
						}
					}
					if (!sChangelist.IsEmpty())
					{
						SVN git;
						if (git.AddToChangeList(changelistItems, sChangelist, git_depth_empty))
						{
							// The changelists were moved, but we now need to run through the selected items again
							// and update their changelist
							POSITION pos = GetFirstSelectedItemPosition();
							int index;
							while ((index = GetNextSelectedItem(pos)) >= 0)
							{
								FileEntry * e = GetListEntry(index);
								e->changelist = sChangelist;
								if (!e->IsFolder())
								{
									if (m_changelists.find(e->changelist)!=m_changelists.end())
										SetItemGroup(index, m_changelists[e->changelist]);
									else
										SetItemGroup(index, 0);
								}
							}
						}
						else
						{
							CMessageBox::Show(m_hWnd, git.GetLastErrorMessage(), _T("TortoiseGit"), MB_ICONERROR);
						}
					}
					SetRedraw(TRUE);
				}
				break;
#endif

			} // switch (cmd)
			m_bBlock = FALSE;
			AfxGetApp()->DoWaitCursor(-1);
			GetStatisticsString();
			int iItemCountAfterMenuCmd = GetItemCount();
			//if (iItemCountAfterMenuCmd != iItemCountBeforeMenuCmd)
			//{
			//	CWnd* pParent = GetParent();
			//	if (NULL != pParent && NULL != pParent->GetSafeHwnd())
			//	{
			//		pParent->SendMessage(SVNSLNM_ITEMCOUNTCHANGED);
			//	}
			//}
		} // if (popup.CreatePopupMenu())
	} // if (selIndex >= 0)

}

void CGitStatusListCtrl::OnContextMenuHeader(CWnd * pWnd, CPoint point)
{
	bool XPorLater = false;
	OSVERSIONINFOEX inf;
	SecureZeroMemory(&inf, sizeof(OSVERSIONINFOEX));
	inf.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((OSVERSIONINFO *)&inf);
	WORD fullver = MAKEWORD(inf.dwMinorVersion, inf.dwMajorVersion);
	if (fullver >= 0x0501)
		XPorLater = true;

	CHeaderCtrl * pHeaderCtrl = (CHeaderCtrl *)pWnd;
	if ((point.x == -1) && (point.y == -1))
	{
		CRect rect;
		pHeaderCtrl->GetItemRect(0, &rect);
		ClientToScreen(&rect);
		point = rect.CenterPoint();
	}
	Locker lock(m_critSec);
	CMenu popup;
	if (popup.CreatePopupMenu())
	{
		int columnCount = m_ColumnManager.GetColumnCount();

		CString temp;
		UINT uCheckedFlags = MF_STRING | MF_ENABLED | MF_CHECKED;
		UINT uUnCheckedFlags = MF_STRING | MF_ENABLED;

		// build control menu

		if (XPorLater)
		{
			temp.LoadString(IDS_STATUSLIST_SHOWGROUPS);
			popup.AppendMenu(IsGroupViewEnabled() ? uCheckedFlags : uUnCheckedFlags, columnCount, temp);
		}

		if (m_ColumnManager.AnyUnusedProperties())
		{
			temp.LoadString(IDS_STATUSLIST_REMOVEUNUSEDPROPS);
			popup.AppendMenu(uUnCheckedFlags, columnCount+1, temp);
		}

		temp.LoadString(IDS_STATUSLIST_RESETCOLUMNORDER);
		popup.AppendMenu(uUnCheckedFlags, columnCount+2, temp);
		popup.AppendMenu(MF_SEPARATOR);

		// standard columns

		for (int i = 1; i < SVNSLC_NUMCOLUMNS; ++i)
		{
			popup.AppendMenu ( m_ColumnManager.IsVisible(i) 
				? uCheckedFlags 
				: uUnCheckedFlags
				, i
				, m_ColumnManager.GetName(i));
		}

		// user-prop columns:
		// find relevant ones and sort 'em

		std::map<CString, int> sortedProps;
		for (int i = SVNSLC_NUMCOLUMNS; i < columnCount; ++i)
			if (m_ColumnManager.IsRelevant(i))
				sortedProps[m_ColumnManager.GetName(i)] = i;

		if (!sortedProps.empty())
		{
			// add 'em to the menu

			popup.AppendMenu(MF_SEPARATOR);

			typedef std::map<CString, int>::const_iterator CIT;
			for ( CIT iter = sortedProps.begin(), end = sortedProps.end()
				; iter != end
				; ++iter)
			{
				popup.AppendMenu ( m_ColumnManager.IsVisible(iter->second) 
					? uCheckedFlags 
					: uUnCheckedFlags
					, iter->second
					, iter->first);
			}
		}

		// show menu & let user pick an entry

		int cmd = popup.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN | TPM_NONOTIFY, point.x, point.y, this, 0);
		if ((cmd >= 1)&&(cmd < columnCount))
		{
			m_ColumnManager.SetVisible (cmd, !m_ColumnManager.IsVisible(cmd));
		} 
		else if (cmd == columnCount)
		{
			EnableGroupView(!IsGroupViewEnabled());
		} 
		else if (cmd == columnCount+1)
		{
			m_ColumnManager.RemoveUnusedProps();
		} 
		else if (cmd == columnCount+2)
		{
			m_ColumnManager.ResetColumns (m_dwDefaultColumns);
		}
	}
}

void CGitStatusListCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{

	if (pWnd == this)
	{
		OnContextMenuList(pWnd, point);
	} // if (pWnd == this)
	else if (pWnd == GetHeaderCtrl())
	{
		OnContextMenuHeader(pWnd, point);
	}
}

void CGitStatusListCtrl::CreateChangeList(const CString& name)
{
#if 0
	CTGitPathList changelistItems;
	FillListOfSelectedItemPaths(changelistItems);
	Git git;
	if (git.AddToChangeList(changelistItems, name, git_depth_empty))
	{
		TCHAR groupname[1024];
		LVGROUP grp = {0};
		grp.cbSize = sizeof(LVGROUP);
		grp.mask = LVGF_ALIGN | LVGF_GROUPID | LVGF_HEADER;
		_tcsncpy_s(groupname, 1024, name, 1023);
		grp.pszHeader = groupname;
		grp.iGroupId = (int)m_changelists.size();
		grp.uAlign = LVGA_HEADER_LEFT;
		m_changelists[name] = InsertGroup(-1, &grp);

		PrepareGroups(true);

		POSITION pos = GetFirstSelectedItemPosition();
		int index;
		while ((index = GetNextSelectedItem(pos)) >= 0)
		{
			FileEntry * e = GetListEntry(index);
			e->changelist = name;
			SetEntryCheck(e, index, FALSE);
		}

		for (index = 0; index < GetItemCount(); ++index)
		{
			FileEntry * e = GetListEntry(index);
			if (m_changelists.find(e->changelist)!=m_changelists.end())
				SetItemGroup(index, m_changelists[e->changelist]);
			else
				SetItemGroup(index, 0);
		}
	}
	else
	{
		CMessageBox::Show(m_hWnd, git.GetLastErrorMessage(), _T("Tortoisegit"), MB_ICONERROR);
	}
#endif
}

void CGitStatusListCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{

	Locker lock(m_critSec);
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
	if (m_bBlock)
		return;

	if (pNMLV->iItem < 0)
	{
		if (!IsGroupViewEnabled())
			return;
		POINT pt;
		DWORD ptW = GetMessagePos();
		pt.x = GET_X_LPARAM(ptW);
		pt.y = GET_Y_LPARAM(ptW);
		ScreenToClient(&pt);
		int group = GetGroupFromPoint(&pt);
		if (group < 0)
			return;
		// check/uncheck the whole group depending on the check-state 
		// of the first item in the group
		m_bBlock = true;
		bool bCheck = false;
		bool bFirst = false;
		LVITEM lv;
		for (int i=0; i<GetItemCount(); ++i)
		{
			SecureZeroMemory(&lv, sizeof(LVITEM));
			lv.mask = LVIF_GROUPID;
			lv.iItem = i;
			GetItem(&lv);
			if (lv.iGroupId == group)
			{
				CTGitPath *entry=(CTGitPath*)GetItemData(i);

				if (!bFirst)
				{
					bCheck = !GetCheck(i);
					bFirst = true;
				}
				if (entry)
				{
					bool bOldCheck = !!GetCheck(i);
					SetEntryCheck(entry, i, bCheck);
					if (bCheck != bOldCheck)
					{
						if (bCheck)
							m_nSelected++;
						else
							m_nSelected--;
					}
				}
			}
		}
		GetStatisticsString();
		m_bBlock = false;
		NotifyCheck();
		return;
	}

	CTGitPath *file=(CTGitPath*)GetItemData(pNMLV->iItem);

	if( file->m_Action&CTGitPath::LOGACTIONS_UNMERGED )
	{
		CAppUtils::ConflictEdit(*file,false,m_bIsRevertTheirMy);

	}else
	{
		if( (!m_Rev1.IsEmpty()) || (!m_Rev1.IsEmpty()))
			StartDiffTwo(pNMLV->iItem);
		else
			StartDiff(pNMLV->iItem);
	}

}
void CGitStatusListCtrl::StartDiffTwo(int fileindex)
{
	if(fileindex<0)
		return;

	CTGitPath file1=*(CTGitPath*)GetItemData(fileindex);

	CGitDiff::Diff(&file1,&file1,
			        m_Rev1,
					m_Rev2);

}
void CGitStatusListCtrl::StartDiffWC(int fileindex)
{
	if(fileindex<0)
		return;

	CString Ver;
	if(this->m_CurrentVersion.IsEmpty() || m_CurrentVersion== GIT_REV_ZERO)
		return;

	CTGitPath file1=*(CTGitPath*)GetItemData(fileindex);

	CGitDiff::Diff(&file1,&file1,
			        CString(GIT_REV_ZERO),
					m_CurrentVersion);

}

void CGitStatusListCtrl::StartDiff(int fileindex)
{
	if(fileindex<0)
		return;

	CTGitPath file1=*(CTGitPath*)GetItemData(fileindex);
	CTGitPath file2;
	if(file1.m_Action & (CTGitPath::LOGACTIONS_REPLACED|CTGitPath::LOGACTIONS_COPY))
	{
		file2.SetFromGit(file1.GetGitOldPathString());
	}else
	{
		file2=file1;
	}

	if(this->m_CurrentVersion.IsEmpty() || m_CurrentVersion== GIT_REV_ZERO)
	{
		if( g_Git.IsInitRepos())
			CGitDiff::DiffNull((CTGitPath*)GetItemData(fileindex),
			        CString(GIT_REV_ZERO));			
		else if( file1.m_Action&CTGitPath::LOGACTIONS_DELETED )
			CGitDiff::DiffNull((CTGitPath*)GetItemData(fileindex),
					GitRev::GetHead(),false);				
		else
			CGitDiff::Diff(&file1,&file2,
			        CString(GIT_REV_ZERO),
					GitRev::GetHead());
	}else
	{
		CGitDiff::Diff(&file1,&file2,
			        m_CurrentVersion,
					m_CurrentVersion+_T("~1"));
	}
#if 0
	if (fileindex < 0)
		return;
	FileEntry * entry = GetListEntry(fileindex);
	ASSERT(entry != NULL);
	if (entry == NULL)
		return;
	if (((entry->status == git_wc_status_normal)&&(entry->remotestatus <= git_wc_status_normal))||
		(entry->status == git_wc_status_unversioned)||(entry->status == git_wc_status_none))
	{
		int ret = (int)ShellExecute(this->m_hWnd, NULL, entry->path.GetWinPath(), NULL, NULL, SW_SHOW);
		if (ret <= HINSTANCE_ERROR)
		{
			CString cmd = _T("RUNDLL32 Shell32,OpenAs_RunDLL ");
			cmd += entry->path.GetWinPathString();
			CAppUtils::LaunchApplication(cmd, NULL, false);
		}
		return;
	}

	GitDiff diff(NULL, m_hWnd, true);
	diff.SetAlternativeTool(!!(GetAsyncKeyState(VK_SHIFT) & 0x8000));
	diff.DiffWCFile(
		entry->path, entry->textstatus, entry->propstatus,
		entry->remotetextstatus, entry->remotepropstatus);
#endif
}

CString CGitStatusListCtrl::GetStatisticsString()
{

	CString sNormal, sAdded, sDeleted, sModified, sConflicted, sUnversioned;
	WORD langID = (WORD)(DWORD)CRegStdWORD(_T("Software\\TortoiseGit\\LanguageID"), GetUserDefaultLangID());
	TCHAR buf[MAX_STATUS_STRING_LENGTH];
	GitStatus::GetStatusString(AfxGetResourceHandle(), git_wc_status_normal, buf, sizeof(buf)/sizeof(TCHAR), langID);
	sNormal = buf;
	GitStatus::GetStatusString(AfxGetResourceHandle(), git_wc_status_added, buf, sizeof(buf)/sizeof(TCHAR), langID);
	sAdded = buf;
	GitStatus::GetStatusString(AfxGetResourceHandle(), git_wc_status_deleted, buf, sizeof(buf)/sizeof(TCHAR), langID);
	sDeleted = buf;
	GitStatus::GetStatusString(AfxGetResourceHandle(), git_wc_status_modified, buf, sizeof(buf)/sizeof(TCHAR), langID);
	sModified = buf;
	GitStatus::GetStatusString(AfxGetResourceHandle(), git_wc_status_conflicted, buf, sizeof(buf)/sizeof(TCHAR), langID);
	sConflicted = buf;
	GitStatus::GetStatusString(AfxGetResourceHandle(), git_wc_status_unversioned, buf, sizeof(buf)/sizeof(TCHAR), langID);
	sUnversioned = buf;
	CString sToolTip;
	sToolTip.Format(_T("%s = %d\n%s = %d\n%s = %d\n%s = %d\n%s = %d\n%s = %d"),
		(LPCTSTR)sNormal, m_nNormal,
		(LPCTSTR)sUnversioned, m_nUnversioned,
		(LPCTSTR)sModified, m_nModified,
		(LPCTSTR)sAdded, m_nAdded,
		(LPCTSTR)sDeleted, m_nDeleted,
		(LPCTSTR)sConflicted, m_nConflicted
		);
	CString sStats;
	sStats.Format(IDS_COMMITDLG_STATISTICSFORMAT, m_nSelected, GetItemCount());
	if (m_pStatLabel)
	{
		m_pStatLabel->SetWindowText(sStats);
	}

	if (m_pSelectButton)
	{
		if (m_nSelected == 0)
			m_pSelectButton->SetCheck(BST_UNCHECKED);
		else if (m_nSelected != GetItemCount())
			m_pSelectButton->SetCheck(BST_INDETERMINATE);
		else
			m_pSelectButton->SetCheck(BST_CHECKED);
	}

	if (m_pConfirmButton)
	{
		m_pConfirmButton->EnableWindow(m_nSelected>0);
	}

	return sToolTip;


}

CString CGitStatusListCtrl::GetCommonDirectory(bool bStrict)
{
	if (!bStrict)
	{
		// not strict means that the selected folder has priority
		if (!m_StatusFileList.GetCommonDirectory().IsEmpty())
			return m_StatusFileList.GetCommonDirectory().GetWinPath();
	}

	CTGitPath commonBaseDirectory;
	int nListItems = GetItemCount();
	for (int i=0; i<nListItems; ++i)
	{
		CTGitPath baseDirectory,*p= (CTGitPath*)this->GetItemData(i);
		ASSERT(p);
		if(p==NULL)
			continue;
		baseDirectory = p->GetDirectory();

		if(commonBaseDirectory.IsEmpty())
		{
			commonBaseDirectory = baseDirectory;
		}
		else
		{
			if (commonBaseDirectory.GetWinPathString().GetLength() > baseDirectory.GetWinPathString().GetLength())
			{
				if (baseDirectory.IsAncestorOf(commonBaseDirectory))
					commonBaseDirectory = baseDirectory;
			}
		}
	}
	return g_Git.m_CurrentDir+CString(_T("\\"))+commonBaseDirectory.GetWinPath();
}

CTGitPath CGitStatusListCtrl::GetCommonURL(bool bStrict)
{
	CTGitPath commonBaseURL;
#if 0
	
	if (!bStrict)
	{
		// not strict means that the selected folder has priority
		if (!m_StatusUrlList.GetCommonDirectory().IsEmpty())
			return m_StatusUrlList.GetCommonDirectory();
	}

	int nListItems = GetItemCount();
	for (int i=0; i<nListItems; ++i)
	{
		const CTGitPath& baseURL = CTGitPath(GetListEntry(i)->GetURL());
		if (baseURL.IsEmpty())
			continue;			// item has no url
		if(commonBaseURL.IsEmpty())
		{
			commonBaseURL = baseURL;
		}
		else
		{
			if (commonBaseURL.GetGitPathString().GetLength() > baseURL.GetGitPathString().GetLength())
			{
				if (baseURL.IsAncestorOf(commonBaseURL))
					commonBaseURL = baseURL;
			}
		}
	}
	
#endif
	return commonBaseURL;
}

void CGitStatusListCtrl::SelectAll(bool bSelect, bool bIncludeNoCommits)
{
	CWaitCursor waitCursor;
	// block here so the LVN_ITEMCHANGED messages
	// get ignored
	m_bBlock = TRUE;
	SetRedraw(FALSE);

	int nListItems = GetItemCount();
	if (bSelect)
		m_nSelected = nListItems;
	else
		m_nSelected = 0;

	for (int i=0; i<nListItems; ++i)
	{
		//FileEntry * entry = GetListEntry(i);
		//ASSERT(entry != NULL);
		CTGitPath *path = (CTGitPath *) GetItemData(i);
		if (path == NULL)
			continue;
		//if ((bIncludeNoCommits)||(entry->GetChangeList().Compare(SVNSLC_IGNORECHANGELIST)))
		SetEntryCheck(path,i,bSelect);
	}

	// unblock before redrawing
	m_bBlock = FALSE;
	SetRedraw(TRUE);
	GetStatisticsString();
	NotifyCheck();
}

void CGitStatusListCtrl::OnLvnGetInfoTip(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVGETINFOTIP pGetInfoTip = reinterpret_cast<LPNMLVGETINFOTIP>(pNMHDR);
	*pResult = 0;
	if (m_bBlock)
		return;
#if 0
	if (GetListEntry(pGetInfoTip->iItem >= 0))
		if (pGetInfoTip->cchTextMax > GetListEntry(pGetInfoTip->iItem)->path.GetGitPathString().GetLength())
		{
			if (GetListEntry(pGetInfoTip->iItem)->GetRelativeGitPath().Compare(GetListEntry(pGetInfoTip->iItem)->path.GetGitPathString())!= 0)
				_tcsncpy_s(pGetInfoTip->pszText, pGetInfoTip->cchTextMax, GetListEntry(pGetInfoTip->iItem)->path.GetGitPathString(), pGetInfoTip->cchTextMax);
			else if (GetStringWidth(GetListEntry(pGetInfoTip->iItem)->path.GetGitPathString()) > GetColumnWidth(pGetInfoTip->iItem))
				_tcsncpy_s(pGetInfoTip->pszText, pGetInfoTip->cchTextMax, GetListEntry(pGetInfoTip->iItem)->path.GetGitPathString(), pGetInfoTip->cchTextMax);
		}
#endif
}

void CGitStatusListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

	// Take the default processing unless we set this to something else below.
	*pResult = CDRF_DODEFAULT;

	// First thing - check the draw stage. If it's the control's prepaint
	// stage, then tell Windows we want messages for every item.

	switch (pLVCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
		{
			// This is the prepaint stage for an item. Here's where we set the
			// item's text color. Our return value will tell Windows to draw the
			// item itself, but it will use the new color we set here.

			// Tell Windows to paint the control itself.
			*pResult = CDRF_DODEFAULT;
			if (m_bBlock)
				return;

			COLORREF crText = GetSysColor(COLOR_WINDOWTEXT);

			if (m_arStatusArray.size() > (DWORD_PTR)pLVCD->nmcd.dwItemSpec)
			{

				//FileEntry * entry = GetListEntry((int)pLVCD->nmcd.dwItemSpec);
				CTGitPath *entry=(CTGitPath *)GetItemData((int)pLVCD->nmcd.dwItemSpec);
				if (entry == NULL)
					return;

				// coloring
				// ========
				// black  : unversioned, normal
				// purple : added
				// blue   : modified
				// brown  : missing, deleted, replaced
				// green  : merged (or potential merges)
				// red    : conflicts or sure conflicts
				if(entry->m_Action & CTGitPath::LOGACTIONS_GRAY)
				{
					crText = RGB(128,128,128);

				}else if(entry->m_Action & CTGitPath::LOGACTIONS_UNMERGED)
				{
					crText = m_Colors.GetColor(CColors::Conflict);

				}else if(entry->m_Action & (CTGitPath::LOGACTIONS_MODIFIED))
				{
					crText = m_Colors.GetColor(CColors::Modified);

				}else if(entry->m_Action & (CTGitPath::LOGACTIONS_ADDED|CTGitPath::LOGACTIONS_COPY))
				{
					crText = m_Colors.GetColor(CColors::Added);
				}
				else if(entry->m_Action & CTGitPath::LOGACTIONS_DELETED)
				{
					crText = m_Colors.GetColor(CColors::DeletedNode);
				}
				else if(entry->m_Action & CTGitPath::LOGACTIONS_REPLACED)
				{
					crText = m_Colors.GetColor(CColors::RenamedNode);
				}else
				{
					crText = GetSysColor(COLOR_WINDOWTEXT);
				}
				// Store the color back in the NMLVCUSTOMDRAW struct.
				pLVCD->clrText = crText;
			}
		}
		break;
	}
}

BOOL CGitStatusListCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (pWnd != this)
		return CListCtrl::OnSetCursor(pWnd, nHitTest, message);
	if (!m_bBlock)
	{
		HCURSOR hCur = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
		SetCursor(hCur);
		return CListCtrl::OnSetCursor(pWnd, nHitTest, message);
	}
	HCURSOR hCur = LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT));
	SetCursor(hCur);
	return TRUE;
}

void CGitStatusListCtrl::RemoveListEntry(int index)
{

	Locker lock(m_critSec);
	DeleteItem(index);

	m_arStatusArray.erase(m_arStatusArray.begin()+index);

#if 0
	delete m_arStatusArray[m_arListArray[index]];
	m_arStatusArray.erase(m_arStatusArray.begin()+m_arListArray[index]);
	m_arListArray.erase(m_arListArray.begin()+index);
	for (int i=index; i< (int)m_arListArray.size(); ++i)
	{
		m_arListArray[i]--;
	}
#endif
}

///< Set a checkbox on an entry in the listbox
// NEVER, EVER call SetCheck directly, because you'll end-up with the checkboxes and the 'checked' flag getting out of sync
void CGitStatusListCtrl::SetEntryCheck(CTGitPath* pEntry, int listboxIndex, bool bCheck)
{
	pEntry->m_Checked = bCheck;
	SetCheck(listboxIndex, bCheck);
}

#if 0
void CGitStatusListCtrl::SetCheckOnAllDescendentsOf(const FileEntry* parentEntry, bool bCheck)
{

	int nListItems = GetItemCount();
	for (int j=0; j< nListItems ; ++j)
	{
		FileEntry * childEntry = GetListEntry(j);
		ASSERT(childEntry != NULL);
		if (childEntry == NULL || childEntry == parentEntry)
			continue;
		if (childEntry->checked != bCheck)
		{
			if (parentEntry->path.IsAncestorOf(childEntry->path))
			{
				SetEntryCheck(childEntry,j,bCheck);
				if(bCheck)
				{
					m_nSelected++;
				}
				else
				{
					m_nSelected--;
				}
			}
		}
	}

}
#endif

void CGitStatusListCtrl::WriteCheckedNamesToPathList(CTGitPathList& pathList)
{

	pathList.Clear();
	int nListItems = GetItemCount();
	for (int i=0; i< nListItems; i++)
	{
		CTGitPath * entry = (CTGitPath*)GetItemData(i);
		ASSERT(entry != NULL);
		if (entry->m_Checked)
		{
			pathList.AddPath(*entry);
		}
	}
	pathList.SortByPathname();

}


/// Build a path list of all the selected items in the list (NOTE - SELECTED, not CHECKED)
void CGitStatusListCtrl::FillListOfSelectedItemPaths(CTGitPathList& pathList, bool bNoIgnored)
{
	pathList.Clear();

	POSITION pos = GetFirstSelectedItemPosition();
	int index;
	while ((index = GetNextSelectedItem(pos)) >= 0)
	{
		CTGitPath * entry = (CTGitPath*)GetItemData(index);
		//if ((bNoIgnored)&&(entry->status == git_wc_status_ignored))
		//	continue;
		pathList.AddPath(*entry);
	}
}

UINT CGitStatusListCtrl::OnGetDlgCode()
{
	// we want to process the return key and not have that one
	// routed to the default pushbutton
	return CListCtrl::OnGetDlgCode() | DLGC_WANTALLKEYS;
}

void CGitStatusListCtrl::OnNMReturn(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	*pResult = 0;
	if (m_bBlock)
		return;
	POSITION pos = GetFirstSelectedItemPosition();
	while ( pos )
	{
		int index = GetNextSelectedItem(pos);
		StartDiff(index);
	}
}

void CGitStatusListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Since we catch all keystrokes (to have the enter key processed here instead
	// of routed to the default pushbutton) we have to make sure that other
	// keys like Tab and Esc still do what they're supposed to do
	// Tab = change focus to next/previous control
	// Esc = quit the dialog
	switch (nChar)
	{
	case (VK_TAB):
		{
			::PostMessage(GetParent()->GetSafeHwnd(), WM_NEXTDLGCTL, GetKeyState(VK_SHIFT)&0x8000, 0);
			return;
		}
		break;
	case (VK_ESCAPE):
		{
			::SendMessage(GetParent()->GetSafeHwnd(), WM_CLOSE, 0, 0);
		}
		break;
	}

	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CGitStatusListCtrl::PreSubclassWindow()
{
	CListCtrl::PreSubclassWindow();
	EnableToolTips(TRUE);
	m_Theme.SetWindowTheme(GetSafeHwnd(), L"Explorer", NULL);
}

INT_PTR CGitStatusListCtrl::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	int row, col;
	RECT cellrect;
	row = CellRectFromPoint(point, &cellrect, &col );

	if (row == -1)
		return -1;


	pTI->hwnd = m_hWnd;
	pTI->uId = (UINT)((row<<10)+(col&0x3ff)+1);
	pTI->lpszText = LPSTR_TEXTCALLBACK;

	pTI->rect = cellrect;

	return pTI->uId;
}

int CGitStatusListCtrl::CellRectFromPoint(CPoint& point, RECT *cellrect, int *col) const
{
	int colnum;

	// Make sure that the ListView is in LVS_REPORT
	if ((GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)
		return -1;

	// Get the top and bottom row visible
	int row = GetTopIndex();
	int bottom = row + GetCountPerPage();
	if (bottom > GetItemCount())
		bottom = GetItemCount();

	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();

	// Loop through the visible rows
	for ( ;row <=bottom;row++)
	{
		// Get bounding rect of item and check whether point falls in it.
		CRect rect;
		GetItemRect(row, &rect, LVIR_BOUNDS);
		if (rect.PtInRect(point))
		{
			// Now find the column
			for (colnum = 0; colnum < nColumnCount; colnum++)
			{
				int colwidth = GetColumnWidth(colnum);
				if (point.x >= rect.left && point.x <= (rect.left + colwidth))
				{
					RECT rectClient;
					GetClientRect(&rectClient);
					if (col)
						*col = colnum;
					rect.right = rect.left + colwidth;

					// Make sure that the right extent does not exceed
					// the client area
					if (rect.right > rectClient.right)
						rect.right = rectClient.right;
					*cellrect = rect;
					return row;
				}
				rect.left += colwidth;
			}
		}
	}
	return -1;
}

BOOL CGitStatusListCtrl::OnToolTipText(UINT /*id*/, NMHDR *pNMHDR, LRESULT *pResult)
{
#if 0
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	CString strTipText;
	UINT_PTR nID = pNMHDR->idFrom;

	if (nID == 0)
		return FALSE;

	UINT_PTR row = ((nID-1) >> 10) & 0x3fffff;
	UINT_PTR col = (nID-1) & 0x3ff;

	if (col == 0)
		return FALSE;	// no custom tooltip for the path, we use the infotip there!

	// get the internal column from the visible columns
	int internalcol = 0;
	UINT_PTR currentcol = 0;
    for (;    (currentcol != col) 
           && (internalcol < m_ColumnManager.GetColumnCount()-1)
         ; ++internalcol)
	{
        if (m_ColumnManager.IsVisible (internalcol))
			currentcol++;
	}

	AFX_MODULE_THREAD_STATE* pModuleThreadState = AfxGetModuleThreadState();
	CToolTipCtrl* pToolTip = pModuleThreadState->m_pToolTip;
	pToolTip->SendMessage(TTM_SETMAXTIPWIDTH, 0, 300);

	*pResult = 0;
	if ((internalcol == 2)||(internalcol == 4))
	{
		FileEntry *fentry = GetListEntry(row);
		if (fentry)
		{
			if (fentry->copied)
			{
				CString url;
				url.Format(IDS_STATUSLIST_COPYFROM, (LPCTSTR)CPathUtils::PathUnescape(fentry->copyfrom_url), (LONG)fentry->copyfrom_rev);
				lstrcpyn(pTTTW->szText, (LPCTSTR)url, 80);
				return TRUE;
			}
			if (fentry->switched)
			{
				CString url;
				url.Format(IDS_STATUSLIST_SWITCHEDTO, (LPCTSTR)CPathUtils::PathUnescape(fentry->url));
				lstrcpyn(pTTTW->szText, (LPCTSTR)url, 80);
				return TRUE;
			}
			if (fentry->keeplocal)
			{
				lstrcpyn(pTTTW->szText, (LPCTSTR)CString(MAKEINTRESOURCE(IDS_STATUSLIST_KEEPLOCAL)), 80);
				return TRUE;
			}
		}
	}
#endif
	return FALSE;
}

void CGitStatusListCtrl::OnPaint()
{
	Default();
	if ((m_bBusy)||(m_bEmpty))
	{
		CString str;
		if (m_bBusy)
		{
			if (m_sBusy.IsEmpty())
				str.LoadString(IDS_STATUSLIST_BUSYMSG);
			else
				str = m_sBusy;
		}
		else
		{
			if (m_sEmpty.IsEmpty())
				str.LoadString(IDS_STATUSLIST_EMPTYMSG);
			else
				str = m_sEmpty;
		}
		COLORREF clrText = ::GetSysColor(COLOR_WINDOWTEXT);
		COLORREF clrTextBk;
		if (IsWindowEnabled())
			clrTextBk = ::GetSysColor(COLOR_WINDOW);
		else
			clrTextBk = ::GetSysColor(COLOR_3DFACE);

		CRect rc;
		GetClientRect(&rc);
		CHeaderCtrl* pHC;
		pHC = GetHeaderCtrl();
		if (pHC != NULL)
		{
			CRect rcH;
			pHC->GetItemRect(0, &rcH);
			rc.top += rcH.bottom;
		}
		CDC* pDC = GetDC();
		{
			CMyMemDC memDC(pDC, &rc);

			memDC.SetTextColor(clrText);
			memDC.SetBkColor(clrTextBk);
			memDC.FillSolidRect(rc, clrTextBk);
			rc.top += 10;
			CGdiObject * oldfont = memDC.SelectStockObject(DEFAULT_GUI_FONT);
			memDC.DrawText(str, rc, DT_CENTER | DT_VCENTER |
				DT_WORDBREAK | DT_NOPREFIX | DT_NOCLIP);
			memDC.SelectObject(oldfont);
		}
		ReleaseDC(pDC);
	}
}

// prevent users from extending our hidden (size 0) columns
void CGitStatusListCtrl::OnHdnBegintrack(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	*pResult = 0;
	if ((phdr->iItem < 0)||(phdr->iItem >= SVNSLC_NUMCOLUMNS))
		return;

    if (m_ColumnManager.IsVisible (phdr->iItem))
	{
		return;
	}
	*pResult = 1;
}

// prevent any function from extending our hidden (size 0) columns
void CGitStatusListCtrl::OnHdnItemchanging(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	*pResult = 0;
    if ((phdr->iItem < 0)||(phdr->iItem >= m_ColumnManager.GetColumnCount()))
	{
		Default();
		return;
	}

    // visible columns may be modified 

    if (m_ColumnManager.IsVisible (phdr->iItem))
	{
		Default();
		return;
	}

    // columns already marked as "invisible" internally may be (re-)sized to 0

    if (   (phdr->pitem != NULL) 
        && (phdr->pitem->mask == HDI_WIDTH)
        && (phdr->pitem->cxy == 0))
	{
		Default();
		return;
	}

	if (   (phdr->pitem != NULL) 
		&& (phdr->pitem->mask != HDI_WIDTH))
	{
		Default();
		return;
	}

    *pResult = 1;
}

void CGitStatusListCtrl::OnDestroy()
{
	SaveColumnWidths(true);
	CListCtrl::OnDestroy();
}

void CGitStatusListCtrl::OnBeginDrag(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
#if 0
	Locker lock(m_critSec);
	CDropFiles dropFiles; // class for creating DROPFILES struct

	int index;
	POSITION pos = GetFirstSelectedItemPosition();
	while ( (index = GetNextSelectedItem(pos)) >= 0 )
	{
		FileEntry * fentry = m_arStatusArray[m_arListArray[index]];
		CTGitPath path = fentry->GetPath();
		dropFiles.AddFile( path.GetWinPathString() );
	}

	if ( dropFiles.GetCount()>0 )
	{
		m_bOwnDrag = true;
		dropFiles.CreateStructure();
		m_bOwnDrag = false;
	}
#endif
	*pResult = 0;
}

void CGitStatusListCtrl::SaveColumnWidths(bool bSaveToRegistry /* = false */)
{
	int maxcol = ((CHeaderCtrl*)(GetDlgItem(0)))->GetItemCount()-1;
	for (int col = 0; col <= maxcol; col++)
        if (m_ColumnManager.IsVisible (col))
            m_ColumnManager.ColumnResized (col);

	if (bSaveToRegistry)
        m_ColumnManager.WriteSettings();
}

bool CGitStatusListCtrl::EnableFileDrop()
{
	m_bFileDropsEnabled = true;
	return true;
}

bool CGitStatusListCtrl::HasPath(const CTGitPath& path)
{
#if 0
	for (size_t i=0; i < m_arStatusArray.size(); i++)
	{
		FileEntry * entry = m_arStatusArray[i];
		if (entry->GetPath().IsEquivalentTo(path))
			return true;
	}
#endif
	return false;
}

bool CGitStatusListCtrl::IsPathShown(const CTGitPath& path)
{
#if 0
	int itemCount = GetItemCount();
	for (int i=0; i < itemCount; i++)
	{
		FileEntry * entry = GetListEntry(i);
		if (entry->GetPath().IsEquivalentTo(path))
			return true;
	}
#endif
	return false;
}

BOOL CGitStatusListCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case 'A':
			{
				if (GetAsyncKeyState(VK_CONTROL)&0x8000)
				{
					// select all entries
					for (int i=0; i<GetItemCount(); ++i)
					{
						SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
					}
					return TRUE;
				}
			}
			break;
		case 'C':
		case VK_INSERT:
			{
				if (GetAsyncKeyState(VK_CONTROL)&0x8000)
				{
					// copy all selected paths to the clipboard
					if (GetAsyncKeyState(VK_SHIFT)&0x8000)
						CopySelectedEntriesToClipboard(SVNSLC_COLSTATUS);
					else
						CopySelectedEntriesToClipboard(0);
					return TRUE;
				}
			}
			break;
		}
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}

bool CGitStatusListCtrl::CopySelectedEntriesToClipboard(DWORD dwCols)
{

	static CString ponly(MAKEINTRESOURCE(IDS_STATUSLIST_PROPONLY));
	static HINSTANCE hResourceHandle(AfxGetResourceHandle());
	WORD langID = (WORD)CRegStdWORD(_T("Software\\TortoiseGit\\LanguageID"), GetUserDefaultLangID());

	CString sClipboard;
	CString temp;
	TCHAR buf[100];
	if (GetSelectedCount() == 0)
		return false;
	// first add the column titles as the first line
	temp.LoadString(IDS_STATUSLIST_COLFILE);
	sClipboard = temp;

    DWORD selection = 0;
    for (int i = 0, count = m_ColumnManager.GetColumnCount(); i < count; ++i)
        if (   ((dwCols == -1) && m_ColumnManager.IsVisible (i))
            || ((dwCols != 1) && (i < 32) && ((dwCols & (1 << i)) != 0)))
        {
            sClipboard += _T("\t") + m_ColumnManager.GetName(i);

            if (i < 32)
                selection += 1 << i;
        }

	sClipboard += _T("\r\n");

	POSITION pos = GetFirstSelectedItemPosition();
	int index;
	while ((index = GetNextSelectedItem(pos)) >= 0)
	{
		CTGitPath * entry = (CTGitPath*)GetItemData(index);
		if(entry == NULL)
			continue;

		sClipboard += entry->GetWinPathString();
		if (selection & SVNSLC_COLFILENAME)
		{
			sClipboard += _T("\t")+entry->GetFileOrDirectoryName();
		}
		if (selection & SVNSLC_COLEXT)
		{
			sClipboard += _T("\t")+entry->GetFileExtension();
		}
	
		if (selection & SVNSLC_COLSTATUS)
		{
#if 0
			if (entry->isNested)
			{
				temp.LoadString(IDS_STATUSLIST_NESTED);
			}
			else
			{
				GitStatus::GetStatusString(hResourceHandle, entry->status, buf, sizeof(buf)/sizeof(TCHAR), (WORD)langID);
				if ((entry->copied)&&(_tcslen(buf)>1))
					_tcscat_s(buf, 100, _T(" (+)"));
				if ((entry->switched)&&(_tcslen(buf)>1))
					_tcscat_s(buf, 100, _T(" (s)"));
				if ((entry->status == entry->propstatus)&&
					(entry->status != git_wc_status_normal)&&
					(entry->status != git_wc_status_unversioned)&&
					(!GitStatus::IsImportant(entry->textstatus)))
					_tcscat_s(buf, 100, ponly);
				temp = buf;
			}
#endif
			sClipboard += _T("\t")+entry->GetActionName();
		}
#if 0
		if (selection & SVNSLC_COLTEXTSTATUS)
		{

			if (entry->isNested)
			{
				temp.LoadString(IDS_STATUSLIST_NESTED);
			}
			else
			{
				GitStatus::GetStatusString(hResourceHandle, entry->textstatus, buf, sizeof(buf)/sizeof(TCHAR), (WORD)langID);
				if ((entry->copied)&&(_tcslen(buf)>1))
					_tcscat_s(buf, 100, _T(" (+)"));
				if ((entry->switched)&&(_tcslen(buf)>1))
					_tcscat_s(buf, 100, _T(" (s)"));
				temp = buf;
			}
			sClipboard += _T("\t")+temp;
		}
#endif
#if 0
		if (selection & SVNSLC_COLREMOTESTATUS)
		{
			if (entry->isNested)
			{
				temp.LoadString(IDS_STATUSLIST_NESTED);
			}
			else
			{
				GitStatus::GetStatusString(hResourceHandle, entry->remotestatus, buf, sizeof(buf)/sizeof(TCHAR), (WORD)langID);
				if ((entry->copied)&&(_tcslen(buf)>1))
					_tcscat_s(buf, 100, _T(" (+)"));
				if ((entry->switched)&&(_tcslen(buf)>1))
					_tcscat_s(buf, 100, _T(" (s)"));
				if ((entry->remotestatus == entry->remotepropstatus)&&
					(entry->remotestatus != git_wc_status_none)&&
					(entry->remotestatus != git_wc_status_normal)&&
					(entry->remotestatus != git_wc_status_unversioned)&&
					(!SVNStatus::IsImportant(entry->remotetextstatus)))
					_tcscat_s(buf, 100, ponly);
				temp = buf;
			}
			sClipboard += _T("\t")+temp;
		}
		if (selection & GitSLC_COLPROPSTATUS)
		{
			if (entry->isNested)
			{
				temp.Empty();
			}
			else
			{
				GitStatus::GetStatusString(hResourceHandle, entry->propstatus, buf, sizeof(buf)/sizeof(TCHAR), (WORD)langID);
				if ((entry->copied)&&(_tcslen(buf)>1))
					_tcscat_s(buf, 100, _T(" (+)"));
				if ((entry->switched)&&(_tcslen(buf)>1))
					_tcscat_s(buf, 100, _T(" (s)"));
				temp = buf;
			}
			sClipboard += _T("\t")+temp;
		}
		if (selection & SVNSLC_COLREMOTETEXT)
		{
			if (entry->isNested)
			{
				temp.Empty();
			}
			else
			{
				GitStatus::GetStatusString(hResourceHandle, entry->remotetextstatus, buf, sizeof(buf)/sizeof(TCHAR), (WORD)langID);
				temp = buf;
			}
			sClipboard += _T("\t")+temp;
		}
		if (selection & SVNSLC_COLREMOTEPROP)
		{
			// SVNSLC_COLREMOTEPROP
			if (entry->isNested)
			{
				temp.Empty();
			}
			else
			{
				GitStatus::GetStatusString(hResourceHandle, entry->remotepropstatus, buf, sizeof(buf)/sizeof(TCHAR), (WORD)langID);
				temp = buf;
			}
			sClipboard += _T("\t")+temp;
		}
		if (selection & SVNSLC_COLURL)
			sClipboard += _T("\t")+entry->url;
		if (selection & SVNSLC_COLLOCK)
		{
			if (!m_HeadRev.IsHead())
			{
				// we have contacted the repository

				// decision-matrix
				// wc		repository		text
				// ""		""				""
				// ""		UID1			owner
				// UID1		UID1			owner
				// UID1		""				lock has been broken
				// UID1		UID2			lock has been stolen
				if (entry->lock_token.IsEmpty() || (entry->lock_token.Compare(entry->lock_remotetoken)==0))
				{
					if (entry->lock_owner.IsEmpty())
						temp = entry->lock_remoteowner;
					else
						temp = entry->lock_owner;
				}
				else if (entry->lock_remotetoken.IsEmpty())
				{
					// broken lock
					temp.LoadString(IDS_STATUSLIST_LOCKBROKEN);
				}
				else
				{
					// stolen lock
					temp.Format(IDS_STATUSLIST_LOCKSTOLEN, (LPCTSTR)entry->lock_remoteowner);
				}
			}
			else
				temp = entry->lock_owner;
			sClipboard += _T("\t")+temp;
		}
		if (selection & SVNSLC_COLLOCKCOMMENT)
			sClipboard += _T("\t")+entry->lock_comment;
		if (selection & SVNSLC_COLAUTHOR)
			sClipboard += _T("\t")+entry->last_commit_author;
		if (selection & SVNSLC_COLREVISION)
		{
			temp.Format(_T("%ld"), entry->last_commit_rev);
			if (entry->last_commit_rev == 0)
				temp.Empty();
			sClipboard += _T("\t")+temp;
		}
		if (selection & SVNSLC_COLREMOTEREVISION)
		{
			temp.Format(_T("%ld"), entry->remoterev);
			if (entry->remoterev == 0)
				temp.Empty();
			sClipboard += _T("\t")+temp;
		}

		if (selection & SVNSLC_COLDATE)
		{
			TCHAR datebuf[SVN_DATE_BUFFER];
			apr_time_t date = entry->last_commit_date;
			SVN::formatDate(datebuf, date, true);
			if (date)
				temp = datebuf;
			else
				temp.Empty();
			sClipboard += _T("\t")+temp;
		}
		if (selection & SVNSLC_COLCOPYFROM)
		{
			if (m_sURL.Compare(entry->copyfrom_url.Left(m_sURL.GetLength()))==0)
				temp = entry->copyfrom_url.Mid(m_sURL.GetLength());
			else
				temp = entry->copyfrom_url;
			sClipboard += _T("\t")+temp;
		}

        for ( int i = SVNSLC_NUMCOLUMNS, count = m_ColumnManager.GetColumnCount()
            ; i < count
            ; ++i)
        {
            if ((dwCols == -1) && m_ColumnManager.IsVisible (i))
            {
                CString value 
                    = entry->present_props[m_ColumnManager.GetName(i)];
                sClipboard += _T("\t") + value;
            }
        }
#endif
		if (selection & SVNSLC_COLADD)
		{
			sClipboard += _T("\t")+entry->m_StatAdd;
		}
		if (selection & SVNSLC_COLDEL)
		{
			sClipboard += _T("\t")+entry->m_StatDel;
		}

		sClipboard += _T("\r\n");
	}

	return CStringUtils::WriteAsciiStringToClipboard(sClipboard);

	return TRUE;

}

size_t CGitStatusListCtrl::GetNumberOfChangelistsInSelection()
{
#if 0
	std::set<CString> changelists;
	POSITION pos = GetFirstSelectedItemPosition();
	int index;
	while ((index = GetNextSelectedItem(pos)) >= 0)
	{
		FileEntry * entry = GetListEntry(index);
		if (!entry->changelist.IsEmpty())
			changelists.insert(entry->changelist);
	}
	return changelists.size();
#endif 
	return 0;
}

bool CGitStatusListCtrl::PrepareGroups(bool bForce /* = false */)
{

	bool bHasGroups=false;
	if ( this->m_UnRevFileList.GetCount()>0 || 
		this->m_IgnoreFileList.GetCount()>0 || bForce)
	{
		bHasGroups = true;
	}

	RemoveAllGroups();
	EnableGroupView(bHasGroups);
	
	TCHAR groupname[1024];
	int groupindex = 0;

	if(bHasGroups)
	{
		LVGROUP grp = {0};
		grp.cbSize = sizeof(LVGROUP);
		grp.mask = LVGF_ALIGN | LVGF_GROUPID | LVGF_HEADER;
		CString sUnassignedName(_T("Modified File"));
		_tcsncpy_s(groupname, 1024, (LPCTSTR)sUnassignedName, 1023);
		grp.pszHeader = groupname;
		grp.iGroupId = groupindex;
		grp.uAlign = LVGA_HEADER_LEFT;
		InsertGroup(groupindex++, &grp);

		//if(m_UnRevFileList.GetCount()>0)
		{
			_tcsncpy_s(groupname, 1024, (LPCTSTR)_T("Not Versioned"), 1023);
			grp.pszHeader = groupname;
			grp.iGroupId = groupindex;
			grp.uAlign = LVGA_HEADER_LEFT;
			InsertGroup(groupindex++, &grp);
		}

		//if(m_IgnoreFileList.GetCount()>0)
		{
			_tcsncpy_s(groupname, 1024, (LPCTSTR)_T("Ignored"), 1023);
			grp.pszHeader = groupname;
			grp.iGroupId = groupindex;
			grp.uAlign = LVGA_HEADER_LEFT;
			InsertGroup(groupindex++, &grp);
		}

	}

#if 0
	m_bHasIgnoreGroup = false;

	// now add the items which don't belong to a group
	LVGROUP grp = {0};
	grp.cbSize = sizeof(LVGROUP);
	grp.mask = LVGF_ALIGN | LVGF_GROUPID | LVGF_HEADER;
	CString sUnassignedName(MAKEINTRESOURCE(IDS_STATUSLIST_UNASSIGNED_CHANGESET));
	_tcsncpy_s(groupname, 1024, (LPCTSTR)sUnassignedName, 1023);
	grp.pszHeader = groupname;
	grp.iGroupId = groupindex;
	grp.uAlign = LVGA_HEADER_LEFT;
	InsertGroup(groupindex++, &grp);

	for (std::map<CString,int>::iterator it = m_changelists.begin(); it != m_changelists.end(); ++it)
	{
		if (it->first.Compare(SVNSLC_IGNORECHANGELIST)!=0)
		{
			LVGROUP grp = {0};
			grp.cbSize = sizeof(LVGROUP);
			grp.mask = LVGF_ALIGN | LVGF_GROUPID | LVGF_HEADER;
			_tcsncpy_s(groupname, 1024, it->first, 1023);
			grp.pszHeader = groupname;
			grp.iGroupId = groupindex;
			grp.uAlign = LVGA_HEADER_LEFT;
			it->second = InsertGroup(groupindex++, &grp);
		}
		else
			m_bHasIgnoreGroup = true;
	}

	if (m_bHasIgnoreGroup)
	{
		// and now add the group 'ignore-on-commit'
		std::map<CString,int>::iterator it = m_changelists.find(SVNSLC_IGNORECHANGELIST);
		if (it != m_changelists.end())
		{
			grp.cbSize = sizeof(LVGROUP);
			grp.mask = LVGF_ALIGN | LVGF_GROUPID | LVGF_HEADER;
			_tcsncpy_s(groupname, 1024, SVNSLC_IGNORECHANGELIST, 1023);
			grp.pszHeader = groupname;
			grp.iGroupId = groupindex;
			grp.uAlign = LVGA_HEADER_LEFT;
			it->second = InsertGroup(groupindex, &grp);
		}
	}
#endif
	return bHasGroups;
}

void CGitStatusListCtrl::NotifyCheck()
{
	CWnd* pParent = GetParent();
	if (NULL != pParent && NULL != pParent->GetSafeHwnd())
	{
		pParent->SendMessage(SVNSLNM_CHECKCHANGED, m_nSelected);
	}
}

int CGitStatusListCtrl::UpdateFileList(git_revnum_t hash,CTGitPathList *list)
{
	BYTE_VECTOR out;
	this->m_bBusy=TRUE;
	m_CurrentVersion=hash;

	int count = 0;
	if(list == NULL)
		count = 1;
	else
		count = list->GetCount();

	if(hash == GIT_REV_ZERO)
	{
		for(int i=0;i<count;i++)
		{	
			BYTE_VECTOR cmdout;
			cmdout.clear();
			CString cmd;
			if(!g_Git.IsInitRepos())
			{
				if(list == NULL)
					cmd=(_T("git.exe diff-index --raw HEAD --numstat -C -M -z"));
				else
					cmd.Format(_T("git.exe diff-index  --raw HEAD --numstat -C -M -z -- \"%s\""),(*list)[i].GetGitPathString());
	
				if(g_Git.Run(cmd,&cmdout))
				{
					cmdout.clear();
					CString strout;
					if(g_Git.Run(_T("git.exe rev-parse --revs-only HEAD"),&strout,CP_UTF8))
					{
						CMessageBox::Show(NULL,strout,_T("TortoiseGit"),MB_OK);
						return -1;
					}
					if(strout.IsEmpty())
						break; //this is initial repositoyr, there are no any history

					CMessageBox::Show(NULL,strout,_T("TortoiseGit"),MB_OK);
					return -1;

				}
				
				if(list == NULL)
					cmd=(_T("git.exe diff-index --cached --raw HEAD --numstat -C -M -z"));
				else
					cmd.Format(_T("git.exe diff-index  --cached --raw HEAD --numstat -C -M -z -- \"%s\""),(*list)[i].GetGitPathString());

				g_Git.Run(cmd,&cmdout);
				//out+=cmdout;
				out.append(cmdout,0);
			}
			else // Init Repository
			{
				if(list == NULL)
					cmd=_T("git.exe ls-files -s -t -z");
				else
					cmd.Format(_T("git.exe ls-files -s -t -z -- \"%s\""),(*list)[i].GetGitPathString());

				g_Git.Run(cmd,&cmdout);
				//out+=cmdout;
				out.append(cmdout,0);
			}
		}

		if(g_Git.IsInitRepos())
		{
			m_StatusFileList.ParserFromLsFile(out);
			for(int i=0;i<m_StatusFileList.GetCount();i++)
				((CTGitPath&)(m_StatusFileList[i])).m_Action=CTGitPath::LOGACTIONS_ADDED;
		}
		else
			this->m_StatusFileList.ParserFromLog(out);

		//handle delete conflict case, when remote : modified, local : deleted. 
		for(int i=0;i<count;i++)
		{	
			BYTE_VECTOR cmdout;
			CString cmd;

			if(list == NULL)
				cmd=_T("git.exe ls-files -u -t -z");
			else
				cmd.Format(_T("git.exe ls-files -u -t -z -- \"%s\""),(*list)[i].GetGitPathString());

			g_Git.Run(cmd,&cmdout);

			CTGitPathList conflictlist;
			conflictlist.ParserFromLog(cmdout);
			for(int i=0;i<conflictlist.GetCount();i++)
			{
				CTGitPath *p=m_StatusFileList.LookForGitPath(conflictlist[i].GetGitPathString());
				if(p)
					p->m_Action|=CTGitPath::LOGACTIONS_UNMERGED;
				else
					m_StatusFileList.AddPath(conflictlist[i]);
			}	
		}

	}else
	{
		int count = 0;
		if(list == NULL)
			count = 1;
		else
			count = list->GetCount();

		for(int i=0;i<count;i++)
		{	
			BYTE_VECTOR cmdout;
			CString cmd;
			if(list == NULL)
				cmd.Format(_T("git.exe diff-tree --raw --numstat -C -M -z %s"),hash);
			else
				cmd.Format(_T("git.exe diff-tree --raw  --numstat -C -M %s -z -- \"%s\""),hash,(*list)[i].GetGitPathString());

			g_Git.Run(cmd,&cmdout);

			out.append(cmdout);
		}
		this->m_StatusFileList.ParserFromLog(out);

	}
	
	for(int i=0;i<m_StatusFileList.GetCount();i++)
	{
		CTGitPath * gitpatch=(CTGitPath*)&m_StatusFileList[i];
		gitpatch->m_Checked = TRUE;
		m_arStatusArray.push_back((CTGitPath*)&m_StatusFileList[i]);
	}

	this->m_bBusy=FALSE;
	return 0;
}

int CGitStatusListCtrl::UpdateWithGitPathList(CTGitPathList &list)
{
	m_arStatusArray.clear();
	for(int i=0;i<list.GetCount();i++)
	{
		CTGitPath * gitpath=(CTGitPath*)&list[i];
		
		if(gitpath ->m_Action & CTGitPath::LOGACTIONS_HIDE)
			continue;

		gitpath->m_Checked = TRUE;
		m_arStatusArray.push_back((CTGitPath*)&list[i]);
	}
	return 0;
}

int CGitStatusListCtrl::UpdateUnRevFileList(CTGitPathList *List)
{
	this->m_UnRevFileList.FillUnRev(CTGitPath::LOGACTIONS_UNVER,List);
	for(int i=0;i<m_UnRevFileList.GetCount();i++)
	{
		CTGitPath * gitpatch=(CTGitPath*)&m_UnRevFileList[i];
		gitpatch->m_Checked = FALSE;
		m_arStatusArray.push_back((CTGitPath*)&m_UnRevFileList[i]);
	}
	return 0;
}

int CGitStatusListCtrl::UpdateIgnoreFileList(CTGitPathList *List)
{
	this->m_IgnoreFileList.FillUnRev(CTGitPath::LOGACTIONS_UNVER|CTGitPath::LOGACTIONS_IGNORE,List);
	for(int i=0;i<m_IgnoreFileList.GetCount();i++)
	{
		CTGitPath * gitpatch=(CTGitPath*)&m_IgnoreFileList[i];
		gitpatch->m_Checked = FALSE;
		m_arStatusArray.push_back((CTGitPath*)&m_IgnoreFileList[i]);
	}
	return 0;
}
int CGitStatusListCtrl::UpdateFileList(int mask,bool once,CTGitPathList *List)
{
	if(mask&CGitStatusListCtrl::FILELIST_MODIFY)
	{
		if(once || (!(m_FileLoaded&CGitStatusListCtrl::FILELIST_MODIFY)))
		{
			UpdateFileList(git_revnum_t(GIT_REV_ZERO),List);
			m_FileLoaded|=CGitStatusListCtrl::FILELIST_MODIFY;
		}
	}
	if(mask&CGitStatusListCtrl::FILELIST_UNVER)
	{
		if(once || (!(m_FileLoaded&CGitStatusListCtrl::FILELIST_UNVER)))
		{
			UpdateUnRevFileList(List);
			m_FileLoaded|=CGitStatusListCtrl::FILELIST_UNVER;
		}
	}
	return 0;
}

void CGitStatusListCtrl::Clear()
{
	m_FileLoaded=0;
	this->DeleteAllItems();
	this->m_arListArray.clear();
	this->m_arStatusArray.clear();
	this->m_changelists.clear();
}
//////////////////////////////////////////////////////////////////////////
#if 0
bool CGitStatusListCtrlDropTarget::OnDrop(FORMATETC* pFmtEtc, STGMEDIUM& medium, DWORD * /*pdwEffect*/, POINTL pt)
{
	if(pFmtEtc->cfFormat == CF_HDROP && medium.tymed == TYMED_HGLOBAL)
	{
		HDROP hDrop = (HDROP)GlobalLock(medium.hGlobal);
		if(hDrop != NULL)
		{
			TCHAR szFileName[MAX_PATH];

			UINT cFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

			POINT clientpoint;
			clientpoint.x = pt.x;
			clientpoint.y = pt.y;
			ScreenToClient(m_hTargetWnd, &clientpoint);
			if ((m_pSVNStatusListCtrl->IsGroupViewEnabled())&&(m_pSVNStatusListCtrl->GetGroupFromPoint(&clientpoint) >= 0))
			{
				CTSVNPathList changelistItems;
				for(UINT i = 0; i < cFiles; ++i)
				{
					DragQueryFile(hDrop, i, szFileName, sizeof(szFileName));
					changelistItems.AddPath(CTSVNPath(szFileName));
				}
				// find the changelist name
				CString sChangelist;
				LONG_PTR nGroup = m_pSVNStatusListCtrl->GetGroupFromPoint(&clientpoint);
				for (std::map<CString, int>::iterator it = m_pSVNStatusListCtrl->m_changelists.begin(); it != m_pSVNStatusListCtrl->m_changelists.end(); ++it)
					if (it->second == nGroup)
						sChangelist = it->first;
				if (!sChangelist.IsEmpty())
				{
					SVN git;
					if (git.AddToChangeList(changelistItems, sChangelist, git_depth_empty))
					{
						for (int l=0; l<changelistItems.GetCount(); ++l)
						{
							int index = m_pSVNStatusListCtrl->GetIndex(changelistItems[l]);
							if (index >= 0)
							{
								CSVNStatusListCtrl::FileEntry * e = m_pSVNStatusListCtrl->GetListEntry(index);
								if (e)
								{
									e->changelist = sChangelist;
									if (!e->IsFolder())
									{
										if (m_pSVNStatusListCtrl->m_changelists.find(e->changelist)!=m_pSVNStatusListCtrl->m_changelists.end())
											m_pSVNStatusListCtrl->SetItemGroup(index, m_pSVNStatusListCtrl->m_changelists[e->changelist]);
										else
											m_pSVNStatusListCtrl->SetItemGroup(index, 0);
									}
								}
							}
							else
							{
								HWND hParentWnd = GetParent(m_hTargetWnd);
								if (hParentWnd != NULL)
									::SendMessage(hParentWnd, CSVNStatusListCtrl::SVNSLNM_ADDFILE, 0, (LPARAM)changelistItems[l].GetWinPath());
							}
						}
					}
					else
					{
						CMessageBox::Show(m_pSVNStatusListCtrl->m_hWnd, git.GetLastErrorMessage(), _T("TortoiseGit"), MB_ICONERROR);
					}
				}
				else
				{
					SVN git;
					if (git.RemoveFromChangeList(changelistItems, CStringArray(), git_depth_empty))
					{
						for (int l=0; l<changelistItems.GetCount(); ++l)
						{
							int index = m_pSVNStatusListCtrl->GetIndex(changelistItems[l]);
							if (index >= 0)
							{
								CSVNStatusListCtrl::FileEntry * e = m_pSVNStatusListCtrl->GetListEntry(index);
								if (e)
								{
									e->changelist = sChangelist;
									m_pSVNStatusListCtrl->SetItemGroup(index, 0);
								}
							}
							else
							{
								HWND hParentWnd = GetParent(m_hTargetWnd);
								if (hParentWnd != NULL)
									::SendMessage(hParentWnd, CSVNStatusListCtrl::SVNSLNM_ADDFILE, 0, (LPARAM)changelistItems[l].GetWinPath());
							}
						}
					}
					else
					{
						CMessageBox::Show(m_pSVNStatusListCtrl->m_hWnd, git.GetLastErrorMessage(), _T("TortoiseGit"), MB_ICONERROR);
					}
				}
			}
			else
			{
				for(UINT i = 0; i < cFiles; ++i)
				{
					DragQueryFile(hDrop, i, szFileName, sizeof(szFileName));
					HWND hParentWnd = GetParent(m_hTargetWnd);
					if (hParentWnd != NULL)
						::SendMessage(hParentWnd, CSVNStatusListCtrl::SVNSLNM_ADDFILE, 0, (LPARAM)szFileName);
				}
			}
		}
		GlobalUnlock(medium.hGlobal);
	}
	return true; //let base free the medium
}
HRESULT STDMETHODCALLTYPE CSVNStatusListCtrlDropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD __RPC_FAR *pdwEffect)
{
	CIDropTarget::DragOver(grfKeyState, pt, pdwEffect);
	*pdwEffect = DROPEFFECT_COPY;
	if (m_pSVNStatusListCtrl)
	{
		POINT clientpoint;
		clientpoint.x = pt.x;
		clientpoint.y = pt.y;
		ScreenToClient(m_hTargetWnd, &clientpoint);
		if ((m_pSVNStatusListCtrl->IsGroupViewEnabled())&&(m_pSVNStatusListCtrl->GetGroupFromPoint(&clientpoint) >= 0))
		{
			*pdwEffect = DROPEFFECT_MOVE;
		}
		else if ((!m_pSVNStatusListCtrl->m_bFileDropsEnabled)||(m_pSVNStatusListCtrl->m_bOwnDrag))
		{
			*pdwEffect = DROPEFFECT_NONE;
		}
	}
	return S_OK;
}f

#endif

void CGitStatusListCtrl::FileSaveAs(CTGitPath *path)
{
	CString filename;
	filename.Format(_T("%s-%s%s"),path->GetBaseFilename(),this->m_CurrentVersion.Left(6),path->GetFileExtension());
	CFileDialog dlg(FALSE,NULL,
					filename,		
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					NULL);
	CString currentpath;
	currentpath=g_Git.m_CurrentDir+_T("\\");
	currentpath+=path->GetWinPathString();

	dlg.m_ofn.lpstrInitialDir=currentpath.GetBuffer();

	CString cmd,out;		
	if(dlg.DoModal()==IDOK)
	{
		filename = dlg.GetFileName();
		if(m_CurrentVersion == GIT_REV_ZERO)
		{
			cmd.Format(_T("copy /Y \"%s\" \"%s\""),path->GetWinPath(),filename);
			if(g_Git.Run(cmd,&out,CP_ACP))
			{
				CMessageBox::Show(NULL,out,_T("TortoiseGit"),MB_OK);
				return;
			}

		}else
		{
			cmd.Format(_T("git.exe cat-file -p %s:\"%s\""),m_CurrentVersion,path->GetGitPathString());
			if(g_Git.RunLogFile(cmd,filename))
			{
				CMessageBox::Show(NULL,out,_T("TortoiseGit"),MB_OK);
				return;
			}
		}
	}

}

int CGitStatusListCtrl::RevertSelectedItemToVersion()
{
	if(this->m_CurrentVersion.IsEmpty())
		return 0;
	if(this->m_CurrentVersion == GIT_REV_ZERO)
		return 0;

	POSITION pos = GetFirstSelectedItemPosition();
	int index;
	CString cmd,out;
	int count =0;
	while ((index = GetNextSelectedItem(pos)) >= 0)
	{
		CTGitPath *fentry=(CTGitPath*)GetItemData(index);
		cmd.Format(_T("git.exe checkout %s -- \"%s\""),m_CurrentVersion,fentry->GetGitPathString());
		out.Empty();
		if(g_Git.Run(cmd,&out,CP_ACP))
		{
			CMessageBox::Show(NULL,out,_T("TortoiseGit"),MB_OK);
		}
		count++;
	}

	out.Format(_T("%d files revert to %s"),count,m_CurrentVersion.Left(6));
	CMessageBox::Show(NULL,out,_T("TortoiseGit"),MB_OK);
	return 0;
}

void CGitStatusListCtrl::OpenFile(CTGitPath*filepath,int mode)
{
	CString file;
	if(this->m_CurrentVersion.IsEmpty() || m_CurrentVersion == GIT_REV_ZERO)
	{
		file= filepath->GetWinPath();
	}else
	{
		CString temppath;
		GetTempPath(temppath);
		file.Format(_T("%s%s_%s%s"),
					temppath,						
					filepath->GetBaseFilename(),
					m_CurrentVersion.Left(6),
					filepath->GetFileExtension());
		CString cmd,out;
		cmd.Format(_T("git.exe cat-file -p %s:\"%s\""),m_CurrentVersion,filepath->GetGitPathString());
		if(g_Git.RunLogFile(cmd,file))
		{
			CMessageBox::Show(NULL,out,_T("TortoiseGit"),MB_OK);
			return;
		}

	}
	if(mode == NOTEPAD2)
	{
		CString sCmd;
		sCmd.Format(_T("\"%s\" \"%s\""),
			(LPCTSTR)(CPathUtils::GetAppDirectory()+_T("notepad2.exe")), file);

		CAppUtils::LaunchApplication(sCmd, NULL, false);
		return ;
	}
	int ret = HINSTANCE_ERROR;

	if(mode == OPEN )
	{
		ret = (int)ShellExecute(this->m_hWnd, NULL,file, NULL, NULL, SW_SHOW);
	
		if (ret > HINSTANCE_ERROR)
		{
			return;
		}
	}

	{
		CString cmd = _T("RUNDLL32 Shell32,OpenAs_RunDLL ");
		cmd += file;
		CAppUtils::LaunchApplication(cmd, NULL, false);
	}
	
}