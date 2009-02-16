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
#pragma once

#include "Globals.h"
#include "registry.h"
#include "resource.h"
#include "ShellCache.h"
#include "RemoteCacheLink.h"
#include "GitStatus.h"
#include "GitFolderStatus.h"
#include "uxtheme.h"

extern	UINT				g_cRefThisDll;			// Reference count of this DLL.
extern	HINSTANCE			g_hmodThisDll;			// Instance handle for this DLL
extern	ShellCache			g_ShellCache;			// caching of registry entries, ...
extern	DWORD				g_langid;
extern	DWORD				g_langTimeout;
extern	HINSTANCE			g_hResInst;
extern	stdstring			g_filepath;
extern	git_wc_status_kind	g_filestatus;			///< holds the corresponding status to the file/dir above
extern  bool				g_readonlyoverlay;		///< whether to show the read only overlay or not
extern	bool				g_lockedoverlay;		///< whether to show the locked overlay or not

extern bool					g_normalovlloaded;
extern bool					g_modifiedovlloaded;
extern bool					g_conflictedovlloaded;
extern bool					g_readonlyovlloaded;
extern bool					g_deletedovlloaded;
extern bool					g_lockedovlloaded;
extern bool					g_addedovlloaded;
extern bool					g_ignoredovlloaded;
extern bool					g_unversionedovlloaded;
extern LPCTSTR				g_MenuIDString;

extern	void				LoadLangDll();
extern  CComCriticalSection	g_csGlobalCOMGuard;
typedef CComCritSecLock<CComCriticalSection> AutoLocker;

typedef DWORD ARGB;

typedef HRESULT (WINAPI *FN_GetBufferedPaintBits) (HPAINTBUFFER hBufferedPaint, RGBQUAD **ppbBuffer, int *pcxRow);
typedef HPAINTBUFFER (WINAPI *FN_BeginBufferedPaint) (HDC hdcTarget, const RECT *prcTarget, BP_BUFFERFORMAT dwFormat, BP_PAINTPARAMS *pPaintParams, HDC *phdc);
typedef HRESULT (WINAPI *FN_EndBufferedPaint) (HPAINTBUFFER hBufferedPaint, BOOL fUpdateTarget);


// The actual OLE Shell context menu handler
/**
 * \ingroup TortoiseShell
 * The main class of our COM object / Shell Extension.
 * It contains all Interfaces we implement for the shell to use.
 * \remark The implementations of the different interfaces are
 * split into several *.cpp files to keep them in a reasonable size.
 */
class CShellExt : public IContextMenu3,
							IPersistFile,
							IColumnProvider,
							IShellExtInit,
							IShellIconOverlayIdentifier,
							IShellPropSheetExt,
							ICopyHookW

// COMPILER ERROR? You need the latest version of the
// platform SDK which has references to IColumnProvider 
// in the header files.  Download it here:
// http://www.microsoft.com/msdownload/platformsdk/sdkupdate/
{
protected:

	enum GitCommands
	{
		ShellSeparator = 0,
		ShellSubMenu = 1,
		ShellSubMenuFolder,
		ShellSubMenuFile,
		ShellSubMenuLink,
		ShellSubMenuMultiple,
		ShellMenuCheckout,
		ShellMenuUpdate,
		ShellMenuCommit,
		ShellMenuAdd,
		ShellMenuAddAsReplacement,
		ShellMenuRevert,
		ShellMenuCleanup,
		ShellMenuResolve,
		ShellMenuSwitch,
		ShellMenuImport,
		ShellMenuExport,
		ShellMenuAbout,
		ShellMenuCreateRepos,
		ShellMenuCopy,
		ShellMenuMerge,
		ShellMenuMergeAll,
		ShellMenuSettings,
		ShellMenuRemove,
		ShellMenuRemoveKeep,
		ShellMenuRename,
		ShellMenuUpdateExt,
		ShellMenuDiff,
		ShellMenuPrevDiff,
		ShellMenuUrlDiff,
		ShellMenuDropCopyAdd,
		ShellMenuDropMoveAdd,
		ShellMenuDropMove,
		ShellMenuDropMoveRename,
		ShellMenuDropCopy,
		ShellMenuDropCopyRename,
		ShellMenuDropExport,
		ShellMenuDropExportExtended,
		ShellMenuLog,
		ShellMenuConflictEditor,
		ShellMenuRelocate,
		ShellMenuHelp,
		ShellMenuShowChanged,
		ShellMenuIgnoreSub,
		ShellMenuIgnore,
		ShellMenuIgnoreFile,
		ShellMenuIgnoreCaseSensitive,
		ShellMenuIgnoreCaseInsensitive,
		ShellMenuRepoBrowse,
		ShellMenuBlame,
		ShellMenuApplyPatch,
		ShellMenuCreatePatch,
		ShellMenuRevisionGraph,
		ShellMenuUnIgnoreSub,
		ShellMenuUnIgnoreCaseSensitive,
		ShellMenuUnIgnore,
//		ShellMenuLock,
//		ShellMenuUnlock,
//		ShellMenuUnlockForce,
		ShellMenuProperties,
		ShellMenuDelUnversioned,
		ShellMenuClipPaste,
		ShellMenuPull,
		ShellMenuPush,
		ShellMenuClone,
		ShellMenuBranch,
		ShellMenuTag,
		ShellMenuFormatPatch,
		ShellMenuImportPatch,
		ShellMenuCherryPick,
		ShellMenuFetch,
		ShellMenuLastEntry			// used to mark the menu array end
	};

	// helper struct for context menu entries
	typedef struct MenuInfo
	{
		GitCommands			command;		///< the command which gets executed for this menu entry
		unsigned __int64	menuID;			///< the menu ID to recognize the entry. NULL if it shouldn't be added to the context menu automatically
		UINT				iconID;			///< the icon to show for the menu entry
		UINT				menuTextID;		///< the text of the menu entry
		UINT				menuDescID;		///< the description text for the menu entry
		/// the following 8 params are for checking whether the menu entry should
		/// be added automatically, based on states of the selected item(s).
		/// The 'yes' states must be set, the 'no' states must not be set
		/// the four pairs are OR'ed together, the 'yes'/'no' states are AND'ed together.
		DWORD				firstyes;
		DWORD				firstno;
		DWORD				secondyes;
		DWORD				secondno;
		DWORD				thirdyes;
		DWORD				thirdno;
		DWORD				fourthyes;
		DWORD				fourthno;
	};

	static MenuInfo menuInfo[];
	WORD fullver;
	FileState m_State;
	ULONG	m_cRef;
	//std::map<int,std::string> verbMap;
	std::map<UINT_PTR, UINT_PTR>	myIDMap;
	std::map<UINT_PTR, UINT_PTR>	mySubMenuMap;
	std::map<stdstring, UINT_PTR> myVerbsMap;
	std::map<UINT_PTR, stdstring> myVerbsIDMap;
	stdstring	folder_;
	std::vector<stdstring> files_;
	DWORD itemStates;				///< see the globals.h file for the ITEMIS_* defines
	DWORD itemStatesFolder;			///< used for states of the folder_ (folder background and/or drop target folder)
	stdstring uuidSource;
	stdstring uuidTarget;
	int space;
	TCHAR stringtablebuffer[255];
	stdstring columnfilepath;		///< holds the last file/dir path for the column provider
	stdstring columnauthor;			///< holds the corresponding author of the file/dir above
	stdstring itemurl;
	stdstring itemshorturl;
	stdstring ignoredprops;
	stdstring owner;
	git_revnum_t columnrev;			///< holds the corresponding revision to the file/dir above
	git_wc_status_kind	filestatus;
	std::map<UINT, HBITMAP> bitmaps;

	GitFolderStatus		m_CachedStatus;		// status cache
	CRemoteCacheLink	m_remoteCacheLink;

	FN_GetBufferedPaintBits pfnGetBufferedPaintBits;
	FN_BeginBufferedPaint pfnBeginBufferedPaint;
	FN_EndBufferedPaint pfnEndBufferedPaint;

#define MAKESTRING(ID) LoadStringEx(g_hResInst, ID, stringtablebuffer, sizeof(stringtablebuffer)/sizeof(TCHAR), (WORD)CRegStdWORD(_T("Software\\TortoiseGit\\LanguageID"), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)))
private:
	void			InsertGitMenu(BOOL istop, HMENU menu, UINT pos, UINT_PTR id, UINT stringid, UINT icon, UINT idCmdFirst, GitCommands com, UINT uFlags);
	void			InsertIgnoreSubmenus(UINT &idCmd, UINT idCmdFirst, HMENU hMenu, HMENU subMenu, UINT &indexMenu, int &indexSubMenu, unsigned __int64 topmenu, bool bShowIcons);
	stdstring		WriteFileListToTempFile();
	bool			WriteClipboardPathsToTempFile(stdstring& tempfile);
	LPCTSTR			GetMenuTextFromResource(int id);
	void			GetColumnStatus(const TCHAR * path, BOOL bIsDir);
	HBITMAP			IconToBitmap(UINT uIcon);
	STDMETHODIMP	QueryDropContext(UINT uFlags, UINT idCmdFirst, HMENU hMenu, UINT &indexMenu);
	bool			IsIllegalFolder(std::wstring folder, int * cslidarray);
	HBITMAP			IconToBitmapPARGB32(UINT uIcon);
	HRESULT			Create32BitHBITMAP(HDC hdc, const SIZE *psize, __deref_opt_out void **ppvBits, __out HBITMAP* phBmp);
	HRESULT			ConvertBufferToPARGB32(HPAINTBUFFER hPaintBuffer, HDC hdc, HICON hicon, SIZE& sizIcon);
	bool			HasAlpha(__in ARGB *pargb, SIZE& sizImage, int cxRow);
	HRESULT			ConvertToPARGB32(HDC hdc, __inout ARGB *pargb, HBITMAP hbmp, SIZE& sizImage, int cxRow);


public:
	CShellExt(FileState state);
	virtual ~CShellExt();

	/** \name IUnknown 
	 * IUnknown members
	 */
	//@{
	STDMETHODIMP         QueryInterface(REFIID, LPVOID FAR *);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	//@}

	/** \name IContextMenu2 
	 * IContextMenu2 members
	 */
	//@{
	STDMETHODIMP	QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
	STDMETHODIMP	InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi);
	STDMETHODIMP	GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT FAR *reserved, LPSTR pszName, UINT cchMax);
	STDMETHODIMP	HandleMenuMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//@}

    /** \name IContextMenu3 
	 * IContextMenu3 members
	 */
	//@{
	STDMETHODIMP	HandleMenuMsg2(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *pResult);
	//@}

	/** \name IColumnProvider
	 * IColumnProvider members
	 */
	//@{
	STDMETHODIMP	GetColumnInfo(DWORD dwIndex, SHCOLUMNINFO *psci);
	STDMETHODIMP	GetItemData(LPCSHCOLUMNID pscid, LPCSHCOLUMNDATA pscd, VARIANT *pvarData);
	STDMETHODIMP	Initialize(LPCSHCOLUMNINIT psci);
	//@}

	/** \name IShellExtInit
	 * IShellExtInit methods
	 */
	//@{
	STDMETHODIMP	Initialize(LPCITEMIDLIST pIDFolder, LPDATAOBJECT pDataObj, HKEY hKeyID);
	//@}

    /** \name IPersistFile
	 * IPersistFile methods
	 */
	//@{
    STDMETHODIMP	GetClassID(CLSID *pclsid);
    STDMETHODIMP	Load(LPCOLESTR pszFileName, DWORD dwMode);
    STDMETHODIMP	IsDirty(void) { return S_OK; };
    STDMETHODIMP	Save(LPCOLESTR /*pszFileName*/, BOOL /*fRemember*/) { return S_OK; };
    STDMETHODIMP	SaveCompleted(LPCOLESTR /*pszFileName*/) { return S_OK; };
    STDMETHODIMP	GetCurFile(LPOLESTR * /*ppszFileName*/) { return S_OK; };
	//@}

	/** \name IShellIconOverlayIdentifier 
	 * IShellIconOverlayIdentifier methods
	 */
	//@{
	STDMETHODIMP	GetOverlayInfo(LPWSTR pwszIconFile, int cchMax, int *pIndex, DWORD *pdwFlags);
	STDMETHODIMP	GetPriority(int *pPriority); 
	STDMETHODIMP	IsMemberOf(LPCWSTR pwszPath, DWORD dwAttrib);
	//@}

	/** \name IShellPropSheetExt 
	 * IShellPropSheetExt methods
	 */
	//@{
	STDMETHODIMP	AddPages(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam);
	STDMETHODIMP	ReplacePage (UINT, LPFNADDPROPSHEETPAGE, LPARAM);
	//@}

	/** \name ICopyHook 
	 * ICopyHook members
	 */
	//@{
	STDMETHODIMP_(UINT) CopyCallback(HWND hWnd, UINT wFunc, UINT wFlags, LPCTSTR pszSrcFile, DWORD dwSrcAttribs, LPCTSTR pszDestFile, DWORD dwDestAttribs);
	//@}

};
