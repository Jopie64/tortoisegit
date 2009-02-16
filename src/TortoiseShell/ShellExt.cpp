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
#include "stdafx.h"

#pragma warning (disable : 4786)

// Initialize GUIDs (should be done only and at-least once per DLL/EXE)
#include <initguid.h>
#include "Guids.h"
#include "git.h"
#include "ShellExt.h"
//#include "..\version.h"
//#include "libintl.h"
#undef swprintf

std::set<CShellExt *> g_exts;

// *********************** CShellExt *************************
CShellExt::CShellExt(FileState state)
{
	OSVERSIONINFOEX inf;
	SecureZeroMemory(&inf, sizeof(OSVERSIONINFOEX));
	inf.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((OSVERSIONINFO *)&inf);
	fullver = MAKEWORD(inf.dwMinorVersion, inf.dwMajorVersion);

    m_State = state;

    m_cRef = 0L;
    g_cRefThisDll++;

	g_exts.insert(this);
	
    INITCOMMONCONTROLSEX used = {
        sizeof(INITCOMMONCONTROLSEX),
			ICC_LISTVIEW_CLASSES | ICC_WIN95_CLASSES | ICC_BAR_CLASSES | ICC_USEREX_CLASSES
    };
    InitCommonControlsEx(&used);
	LoadLangDll();

	if (fullver >= 0x0600)
	{
		HMODULE hUxTheme = ::GetModuleHandle (_T("UXTHEME.DLL"));

		pfnGetBufferedPaintBits = (FN_GetBufferedPaintBits)::GetProcAddress(hUxTheme, "GetBufferedPaintBits");
		pfnBeginBufferedPaint = (FN_BeginBufferedPaint)::GetProcAddress(hUxTheme, "BeginBufferedPaint");
		pfnEndBufferedPaint = (FN_EndBufferedPaint)::GetProcAddress(hUxTheme, "EndBufferedPaint");
	}
}

CShellExt::~CShellExt()
{
	std::map<UINT, HBITMAP>::iterator it;
	for (it = bitmaps.begin(); it != bitmaps.end(); ++it)
	{
		::DeleteObject(it->second);
	}
	bitmaps.clear();
	g_cRefThisDll--;
	g_exts.erase(this);
}

void LoadLangDll()
{
	if ((g_langid != g_ShellCache.GetLangID())&&((g_langTimeout == 0)||(g_langTimeout < GetTickCount())))
	{
		g_langid = g_ShellCache.GetLangID();
		DWORD langId = g_langid;
		TCHAR langDll[MAX_PATH*4];
		HINSTANCE hInst = NULL;
		TCHAR langdir[MAX_PATH] = {0};
		char langdirA[MAX_PATH] = {0};
		if (GetModuleFileName(g_hmodThisDll, langdir, MAX_PATH)==0)
			return;
		if (GetModuleFileNameA(g_hmodThisDll, langdirA, MAX_PATH)==0)
			return;
		TCHAR * dirpoint = _tcsrchr(langdir, '\\');
		char * dirpointA = strrchr(langdirA, '\\');
		if (dirpoint)
			*dirpoint = 0;
		if (dirpointA)
			*dirpointA = 0;
		dirpoint = _tcsrchr(langdir, '\\');
		dirpointA = strrchr(langdirA, '\\');
		if (dirpoint)
			*dirpoint = 0;
		if (dirpointA)
			*dirpointA = 0;
		strcat_s(langdirA, MAX_PATH, "\\Languages");
//		bindtextdomain ("subversion", langdirA);

		do
		{
			_stprintf_s(langDll, MAX_PATH*4, _T("%s\\Languages\\TortoiseProc%d.dll"), langdir, langId);
			BOOL versionmatch = TRUE;

			struct TRANSARRAY
			{
				WORD wLanguageID;
				WORD wCharacterSet;
			};

			DWORD dwReserved,dwBufferSize;
			dwBufferSize = GetFileVersionInfoSize((LPTSTR)langDll,&dwReserved);

			if (dwBufferSize > 0)
			{
				LPVOID pBuffer = (void*) malloc(dwBufferSize);

				if (pBuffer != (void*) NULL)
				{
					UINT        nInfoSize = 0,
						nFixedLength = 0;
					LPSTR       lpVersion = NULL;
					VOID*       lpFixedPointer;
					TRANSARRAY* lpTransArray;
					TCHAR       strLangProduktVersion[MAX_PATH];

					if (GetFileVersionInfo((LPTSTR)langDll,
						dwReserved,
						dwBufferSize,
						pBuffer))
					{
						// Query the current language
						if (VerQueryValue(	pBuffer,
							_T("\\VarFileInfo\\Translation"),
							&lpFixedPointer,
							&nFixedLength))
						{
							lpTransArray = (TRANSARRAY*) lpFixedPointer;

							_stprintf_s(strLangProduktVersion, MAX_PATH, _T("\\StringFileInfo\\%04x%04x\\ProductVersion"),
								lpTransArray[0].wLanguageID, lpTransArray[0].wCharacterSet);

							if (VerQueryValue(pBuffer,
								(LPTSTR)strLangProduktVersion,
								(LPVOID *)&lpVersion,
								&nInfoSize))
							{
//								versionmatch = (_tcscmp((LPCTSTR)lpVersion, _T(STRPRODUCTVER)) == 0);
							}

						}
					}
					free(pBuffer);
				} // if (pBuffer != (void*) NULL) 
			} // if (dwBufferSize > 0) 
			else
				versionmatch = FALSE; 

			if (versionmatch)
				hInst = LoadLibrary(langDll);
			if (hInst != NULL)
			{
				if (g_hResInst != g_hmodThisDll)
					FreeLibrary(g_hResInst);
				g_hResInst = hInst;
			}
			else
			{
				DWORD lid = SUBLANGID(langId);
				lid--;
				if (lid > 0)
				{
					langId = MAKELANGID(PRIMARYLANGID(langId), lid);
				}
				else
					langId = 0;
			} 
		} while ((hInst == NULL) && (langId != 0));
		if (hInst == NULL)
		{
			// either the dll for the selected language is not present, or
			// it is the wrong version.
			// fall back to English and set a timeout so we don't retry
			// to load the language dll too often
			if (g_hResInst != g_hmodThisDll)
				FreeLibrary(g_hResInst);
			g_hResInst = g_hmodThisDll;
			g_langid = 1033;
			// set a timeout of 10 seconds
			if (g_ShellCache.GetLangID() != 1033)
				g_langTimeout = GetTickCount() + 10000;
		}
		else
			g_langTimeout = 0;
	} // if (g_langid != g_ShellCache.GetLangID()) 
}

STDMETHODIMP CShellExt::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    *ppv = NULL; 

    if (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown))
    {
        *ppv = (LPSHELLEXTINIT)this;
    }
    else if (IsEqualIID(riid, IID_IContextMenu))
    {
        *ppv = (LPCONTEXTMENU)this;
    }
    else if (IsEqualIID(riid, IID_IContextMenu2))
    {
        *ppv = (LPCONTEXTMENU2)this;
    }
    else if (IsEqualIID(riid, IID_IContextMenu3))
    {
        *ppv = (LPCONTEXTMENU3)this;
    }
    else if (IsEqualIID(riid, IID_IShellIconOverlayIdentifier))
    {
        *ppv = (IShellIconOverlayIdentifier*)this;
    }
    else if (IsEqualIID(riid, IID_IShellPropSheetExt))
    {
        *ppv = (LPSHELLPROPSHEETEXT)this;
    }
	else if (IsEqualIID(riid, IID_IColumnProvider)) 
	{ 
		*ppv = (IColumnProvider *)this;
	} 
	else if (IsEqualIID(riid, IID_IShellCopyHook))
	{
		*ppv = (ICopyHook *)this;
	}
    if (*ppv)
    {
        AddRef();
		
        return NOERROR;
    }
	
    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExt::AddRef()
{
    return ++m_cRef;
}

STDMETHODIMP_(ULONG) CShellExt::Release()
{
    if (--m_cRef)
        return m_cRef;
	
    delete this;
	
    return 0L;
}

// IPersistFile members
STDMETHODIMP CShellExt::GetClassID(CLSID *pclsid) 
{
    *pclsid = CLSID_Tortoisegit_UNCONTROLLED;
    return S_OK;
}

STDMETHODIMP CShellExt::Load(LPCOLESTR /*pszFileName*/, DWORD /*dwMode*/)
{
    return S_OK;
}

// ICopyHook member
UINT __stdcall CShellExt::CopyCallback(HWND /*hWnd*/, UINT wFunc, UINT /*wFlags*/, LPCTSTR pszSrcFile, DWORD /*dwSrcAttribs*/, LPCTSTR /*pszDestFile*/, DWORD /*dwDestAttribs*/)
{
	if (wFunc == FO_COPY)
		return IDYES;	// copying is not a problem for us

	m_remoteCacheLink.ReleaseLockForPath(CTGitPath(pszSrcFile));
	// we could now wait a little bit to give the cache time to release the handles.
	// but the explorer/shell already retries any action for about two seconds
	// if it first fails. So if the cache hasn't released the handle yet, the explorer
	// will retry anyway, so we just leave here immediately.
	return IDYES;
}
// CShellExt member functions (needed for IShellPropSheetExt)
STDMETHODIMP CShellExt::AddPages (LPFNADDPROPSHEETPAGE lpfnAddPage,
                                  LPARAM lParam)
{
#if 0
	for (std::vector<stdstring>::iterator I = files_.begin(); I != files_.end(); ++I)
	{
		GitStatus svn = GitStatus();
		if (svn.GetStatus(CTGitPath(I->c_str())) == (-2))
			return NOERROR;			// file/directory not under version control

		if (svn.status->entry == NULL)
			return NOERROR;
	}

	if (files_.size() == 0)
		return NOERROR;

	LoadLangDll();
    PROPSHEETPAGE psp;
	SecureZeroMemory(&psp, sizeof(PROPSHEETPAGE));
	HPROPSHEETPAGE hPage;
	CGitPropertyPage *sheetpage = new CGitPropertyPage(files_);

    psp.dwSize = sizeof (psp);
    psp.dwFlags = PSP_USEREFPARENT | PSP_USETITLE | PSP_USEICONID | PSP_USECALLBACK;	
	psp.hInstance = g_hResInst;
	psp.pszTemplate = MAKEINTRESOURCE(IDD_PROPPAGE);
    psp.pszIcon = MAKEINTRESOURCE(IDI_APPSMALL);
    psp.pszTitle = _T("Subversion");
    psp.pfnDlgProc = (DLGPROC) PageProc;
    psp.lParam = (LPARAM) sheetpage;
    psp.pfnCallback = PropPageCallbackProc;
    psp.pcRefParent = &g_cRefThisDll;

    hPage = CreatePropertySheetPage (&psp);

	if (hPage != NULL)
	{
        if (!lpfnAddPage (hPage, lParam))
        {
            delete sheetpage;
            DestroyPropertySheetPage (hPage);
        }
	}
#endif
    return NOERROR;
}



STDMETHODIMP CShellExt::ReplacePage (UINT /*uPageID*/, LPFNADDPROPSHEETPAGE /*lpfnReplaceWith*/, LPARAM /*lParam*/)
{
    return E_FAIL;
}

