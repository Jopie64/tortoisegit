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
//#include "vld.h"
#include "TortoiseProc.h"
#include "SysImageList.h"
//#include "CrashReport.h"
#include "CmdLineParser.h"
#include "Hooks.h"
#include "AppUtils.h"
#include "PathUtils.h"
#include "UnicodeUtils.h"
#include "MessageBox.h"
//#include "libintl.h"
#include "DirFileEnum.h"
//#include "SoundUtils.h"
//#include "SVN.h"
#include "GitAdminDir.h"
#include "Git.h"
//#include "SVNGlobal.h"
//#include "svn_types.h"
//#include "svn_dso.h"
//#include <openssl/ssl.h>
//#include <openssl/err.h>

#include "Commands\Command.h"

#include "..\version.h"
#define STRUCT_IOVEC_DEFINED
//#include "sasl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


BEGIN_MESSAGE_MAP(CTortoiseProcApp, CWinAppEx)
	ON_COMMAND(ID_HELP, CWinAppEx::OnHelp)
END_MESSAGE_MAP()

CString g_version;
CString CGit::m_MsysGitPath;
//////////////////////////////////////////////////////////////////////////

CTortoiseProcApp::CTortoiseProcApp()
{
	EnableHtmlHelp();
	int argc = 0;
	g_version=_T("abc");
	const char* const * argv = NULL;
//	apr_app_initialize(&argc, &argv, NULL);
//	svn_dso_initialize2();
	SYS_IMAGE_LIST();
//	CHooks::Create();
	g_GitAdminDir.Init();
	m_bLoadUserToolbars = FALSE;
	m_bSaveState = FALSE;
	retSuccess = false;

	//This doesnt work here because m_MsysGitPath is not yet initialized
//	CGit git;
//	git.GetUserName();
}

CTortoiseProcApp::~CTortoiseProcApp()
{
//	sasl_done();

	// global application exit cleanup (after all SSL activity is shutdown)
	// we have to clean up SSL ourselves, since neon doesn't do that (can't do it)
	// because those cleanup functions work globally per process.
	//ERR_free_strings();
	//EVP_cleanup();
	//CRYPTO_cleanup_all_ex_data();

	// since it is undefined *when* the global object SVNAdminDir is
	// destroyed, we tell it to destroy the memory pools and terminate apr
	// *now* instead of later when the object itself is destroyed.
	g_GitAdminDir.Close();
//	CHooks::Destroy();
	SYS_IMAGE_LIST().Cleanup();
	//apr_terminate();
}

// The one and only CTortoiseProcApp object
CTortoiseProcApp theApp;
HWND hWndExplorer;
CString sOrigCWD;

BOOL CTortoiseProcApp::CheckMsysGitDir()
{
	CRegString msysdir=CRegString(_T("Software\\TortoiseGit\\MSysGit"),_T(""),FALSE,HKEY_LOCAL_MACHINE);
	CString str=msysdir;
	if(str.IsEmpty())
	{
		CRegString msysinstalldir=CRegString(_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Git_is1\\InstallLocation"),_T(""),FALSE,HKEY_LOCAL_MACHINE);
		str=msysinstalldir;
		str+="\\bin";
		msysdir=str;
		msysdir.write();

	}
	CGit::m_MsysGitPath=str;

	TCHAR *oldpath;
	size_t size;
	
	_tdupenv_s(&oldpath,&size,_T("PATH")); 

	CString path;
	path.Format(_T("%s;"),str);
	path+=oldpath;

	_tputenv_s(_T("PATH"),path);

	CString cmd,out;
	cmd=_T("git.exe --version");
	if(g_Git.Run(cmd,&out))
	{
		return false;
	}
	else
		return true;
	
}
//CCrashReport crasher("crashreports@tortoisesvn.tigris.org", "Crash Report for TortoiseSVN " APP_X64_STRING " : " STRPRODUCTVER, TRUE);// crash

// CTortoiseProcApp initialization

BOOL CTortoiseProcApp::InitInstance()
{
	EnableCrashHandler();
	CheckUpgrade();
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
	CMFCButton::EnableWindowsTheming();

	if(!CheckMsysGitDir())
	{
		if(CMessageBox::Show(NULL,_T("MSysGit(http://code.google.com/p/msysgit) have not installed Correctly\n\
or MSysGit Path setting error\n\
Click Yes to open setting dialog to setup MSysGit Path"),
							_T("TortoiseGit"),MB_YESNO|MB_ICONERROR)==IDYES)
		{
			//todo open setting
		}
		return FALSE;	
	}

	//set the resource dll for the required language
	CRegDWORD loc = CRegDWORD(_T("Software\\TortoiseGit\\LanguageID"), 1033);
	long langId = loc;
	CString langDll;
	CStringA langpath = CStringA(CPathUtils::GetAppParentDirectory());
	langpath += "Languages";
//	bindtextdomain("subversion", (LPCSTR)langpath);
//	bind_textdomain_codeset("subversion", "UTF-8");
	HINSTANCE hInst = NULL;
	do
	{
		langDll.Format(_T("..\\Languages\\TortoiseProc%d.dll"), langId);
		
		hInst = LoadLibrary(langDll);

		CString sVer = _T(STRPRODUCTVER);
		CString sFileVer = CPathUtils::GetVersionFromFile(langDll);
		if (sFileVer.Compare(sVer)!=0)
		{
			FreeLibrary(hInst);
			hInst = NULL;
		}
		if (hInst != NULL)
		{
			AfxSetResourceHandle(hInst);
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
	TCHAR buf[6];
	_tcscpy_s(buf, _T("en"));
	langId = loc;
	CString sHelppath;
	sHelppath = this->m_pszHelpFilePath;
	sHelppath = sHelppath.MakeLower();
	// MFC uses a help file with the same name as the application by default,
	// which means we have to change that default to our language specific help files
	sHelppath.Replace(_T("tortoiseproc.chm"), _T("TortoiseSVN_en.chm"));
	free((void*)m_pszHelpFilePath);
	m_pszHelpFilePath=_tcsdup(sHelppath);
	sHelppath = CPathUtils::GetAppParentDirectory() + _T("Languages\\TortoiseGit_en.chm");
	do
	{
		CString sLang = _T("_");
		if (GetLocaleInfo(MAKELCID(langId, SORT_DEFAULT), LOCALE_SISO639LANGNAME, buf, sizeof(buf)))
		{
			sLang += buf;
			sHelppath.Replace(_T("_en"), sLang);
			if (PathFileExists(sHelppath))
			{
				free((void*)m_pszHelpFilePath);
				m_pszHelpFilePath=_tcsdup(sHelppath);
				break;
			}
		}
		sHelppath.Replace(sLang, _T("_en"));
		if (GetLocaleInfo(MAKELCID(langId, SORT_DEFAULT), LOCALE_SISO3166CTRYNAME, buf, sizeof(buf)))
		{
			sLang += _T("_");
			sLang += buf;
			sHelppath.Replace(_T("_en"), sLang);
			if (PathFileExists(sHelppath))
			{
				free((void*)m_pszHelpFilePath);
				m_pszHelpFilePath=_tcsdup(sHelppath);
				break;
			}
		}
		sHelppath.Replace(sLang, _T("_en"));

		DWORD lid = SUBLANGID(langId);
		lid--;
		if (lid > 0)
		{
			langId = MAKELANGID(PRIMARYLANGID(langId), lid);
		}
		else
			langId = 0;
	} while (langId);
	setlocale(LC_ALL, "");
	
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	
    INITCOMMONCONTROLSEX used = {
        sizeof(INITCOMMONCONTROLSEX),
			ICC_ANIMATE_CLASS | ICC_BAR_CLASSES | ICC_COOL_CLASSES | ICC_DATE_CLASSES |
			ICC_HOTKEY_CLASS | ICC_INTERNET_CLASSES | ICC_LISTVIEW_CLASSES |
			ICC_NATIVEFNTCTL_CLASS | ICC_PAGESCROLLER_CLASS | ICC_PROGRESS_CLASS |
			ICC_TAB_CLASSES | ICC_TREEVIEW_CLASSES | ICC_UPDOWN_CLASS |
			ICC_USEREX_CLASSES | ICC_WIN95_CLASSES
    };
    InitCommonControlsEx(&used);
	AfxOleInit();
	AfxEnableControlContainer();
	AfxInitRichEdit2();
	CWinAppEx::InitInstance();
	SetRegistryKey(_T("TortoiseGit"));

	CCmdLineParser parser(AfxGetApp()->m_lpCmdLine);

	// if HKCU\Software\TortoiseSVN\Debug is not 0, show our command line
	// in a message box
	if (CRegDWORD(_T("Software\\TortoiseGit\\Debug"), FALSE)==TRUE)
		AfxMessageBox(AfxGetApp()->m_lpCmdLine, MB_OK | MB_ICONINFORMATION);

	if ( parser.HasKey(_T("path")) && parser.HasKey(_T("pathfile")))
	{
		CMessageBox::Show(NULL, IDS_ERR_INVALIDPATH, IDS_APPNAME, MB_ICONERROR);
		return FALSE;
	}

	CTGitPath cmdLinePath;
	CTGitPathList pathList;
	if ( parser.HasKey(_T("pathfile")) )
	{

		CString sPathfileArgument = CPathUtils::GetLongPathname(parser.GetVal(_T("pathfile")));

		cmdLinePath.SetFromUnknown(sPathfileArgument);
		if (pathList.LoadFromFile(cmdLinePath)==false)
			return FALSE;		// no path specified!
		if ( parser.HasKey(_T("deletepathfile")) )
		{
			// We can delete the temporary path file, now that we've loaded it
			::DeleteFile(cmdLinePath.GetWinPath());
		}
		// This was a path to a temporary file - it's got no meaning now, and
		// anybody who uses it again is in for a problem...
		cmdLinePath.Reset();

	}
	else
	{

		CString sPathArgument = CPathUtils::GetLongPathname(parser.GetVal(_T("path")));
		int asterisk = sPathArgument.Find('*');
		cmdLinePath.SetFromUnknown(asterisk >= 0 ? sPathArgument.Left(asterisk) : sPathArgument);
		pathList.LoadFromAsteriskSeparatedString(sPathArgument);

	}
	
	hWndExplorer = NULL;
	CString sVal = parser.GetVal(_T("hwnd"));
	if (!sVal.IsEmpty())
		hWndExplorer = (HWND)_ttoi64(sVal);

	while (GetParent(hWndExplorer)!=NULL)
		hWndExplorer = GetParent(hWndExplorer);
	if (!IsWindow(hWndExplorer))
	{
		hWndExplorer = NULL;
	}
	
	// Subversion sometimes writes temp files to the current directory!
	// Since TSVN doesn't need a specific CWD anyway, we just set it
	// to the users temp folder: that way, Subversion is guaranteed to
	// have write access to the CWD
	{
		DWORD len = GetCurrentDirectory(0, NULL);
		if (len)
		{
			TCHAR * originalCurrentDirectory = new TCHAR[len];
			if (GetCurrentDirectory(len, originalCurrentDirectory))
			{
				sOrigCWD = originalCurrentDirectory;
				sOrigCWD = CPathUtils::GetLongPathname(sOrigCWD);
			}
			delete [] originalCurrentDirectory;
		}
		TCHAR pathbuf[MAX_PATH];
		GetTempPath(MAX_PATH, pathbuf);
		SetCurrentDirectory(pathbuf);		
	}

	// check for newer versions
	if (CRegDWORD(_T("Software\\TortoiseSVN\\CheckNewer"), TRUE) != FALSE)
	{
		time_t now;
		struct tm ptm;

		time(&now);
		if ((now != 0) && (localtime_s(&ptm, &now)==0))
		{
			int week = 0;
			// we don't calculate the real 'week of the year' here
			// because just to decide if we should check for an update
			// that's not needed.
			week = ptm.tm_yday / 7;

			CRegDWORD oldweek = CRegDWORD(_T("Software\\TortoiseSVN\\CheckNewerWeek"), (DWORD)-1);
			if (((DWORD)oldweek) == -1)
				oldweek = week;		// first start of TortoiseProc, no update check needed
			else
			{
				if ((DWORD)week != oldweek)
				{
					oldweek = week;

					TCHAR com[MAX_PATH+100];
					GetModuleFileName(NULL, com, MAX_PATH);
					_tcscat_s(com, MAX_PATH+100, _T(" /command:updatecheck"));

					//CAppUtils::LaunchApplication(com, 0, false);
				}
			}
		}
	}

	if (parser.HasVal(_T("configdir")))
	{
		// the user can override the location of the Subversion config directory here
		CString sConfigDir = parser.GetVal(_T("configdir"));
//		g_GitGlobal.SetConfigDir(sConfigDir);
	}
	// to avoid that SASL will look for and load its plugin dlls all around the
	// system, we set the path here.
	// Note that SASL doesn't have to be initialized yet for this to work
//	sasl_set_path(SASL_PATH_TYPE_PLUGIN, (LPSTR)(LPCSTR)CUnicodeUtils::GetUTF8(CPathUtils::GetAppDirectory().TrimRight('\\')));

	HANDLE TSVNMutex = ::CreateMutex(NULL, FALSE, _T("TortoiseGitProc.exe"));	
	{
#if 0
		CString err = Git::CheckConfigFile();
		if (!err.IsEmpty())
		{
			CMessageBox::Show(hWndExplorer, err, _T("TortoiseGit"), MB_ICONERROR);
			// Normally, we give-up and exit at this point, but there is a trap here
			// in that the user might need to use the settings dialog to edit the config file.
			if (CString(parser.GetVal(_T("command"))).Compare(_T("settings"))==0)
			{
				// just open the config file
				TCHAR buf[MAX_PATH];
				SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, buf);
				CString path = buf;
				path += _T("\\Git\\config");
				CAppUtils::StartTextViewer(path);
				return FALSE;
			}
		}
#endif
	}

	// execute the requested command
	CommandServer server;
	Command * cmd = server.GetCommand(parser.GetVal(_T("command")));
	if (cmd)
	{
		cmd->SetExplorerHwnd(hWndExplorer);
		
		if(!g_Git.SetCurrentDir(cmdLinePath.GetWinPathString()))
		{
			int i=0;
			for(i=0;i<pathList.GetCount();i++)
				if(g_Git.SetCurrentDir(pathList[i].GetWinPath()))
					break;
		}

		if(g_Git.m_CurrentDir)
			SetCurrentDirectory(g_Git.m_CurrentDir);

		cmd->SetParser(parser);
		cmd->SetPaths(pathList, cmdLinePath);

		retSuccess = cmd->Execute();
		delete cmd;
	}

	if (TSVNMutex)
		::CloseHandle(TSVNMutex);

	// Look for temporary files left around by TortoiseSVN and
	// remove them. But only delete 'old' files because some
	// apps might still be needing the recent ones.
	{
		DWORD len = ::GetTempPath(0, NULL);
		TCHAR * path = new TCHAR[len + 100];
		len = ::GetTempPath (len+100, path);
		if (len != 0)
		{
			CSimpleFileFind finder = CSimpleFileFind(path, _T("*svn*.*"));
			FILETIME systime_;
			::GetSystemTimeAsFileTime(&systime_);
			__int64 systime = (((_int64)systime_.dwHighDateTime)<<32) | ((__int64)systime_.dwLowDateTime);
			while (finder.FindNextFileNoDirectories())
			{
				CString filepath = finder.GetFilePath();
				HANDLE hFile = ::CreateFile(filepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
				if (hFile != INVALID_HANDLE_VALUE)
				{
					FILETIME createtime_;
					if (::GetFileTime(hFile, &createtime_, NULL, NULL))
					{
						::CloseHandle(hFile);
						__int64 createtime = (((_int64)createtime_.dwHighDateTime)<<32) | ((__int64)createtime_.dwLowDateTime);
						if ((createtime + 864000000000) < systime)		//only delete files older than a day
						{
							::SetFileAttributes(filepath, FILE_ATTRIBUTE_NORMAL);
							::DeleteFile(filepath);
						}
					}
					else
						::CloseHandle(hFile);
				}
			}
		}	
		delete[] path;		
	}


	// Since the dialog has been closed, return FALSE so that we exit the
	// application, rather than start the application's message pump.
	return FALSE;
}



void CTortoiseProcApp::CheckUpgrade()
{
	CRegString regVersion = CRegString(_T("Software\\TortoiseSVN\\CurrentVersion"));
	CString sVersion = regVersion;
	if (sVersion.Compare(_T(STRPRODUCTVER))==0)
		return;
	// we're starting the first time with a new version!
	
	LONG lVersion = 0;
	int pos = sVersion.Find(',');
	if (pos > 0)
	{
		lVersion = (_ttol(sVersion.Left(pos))<<24);
		lVersion |= (_ttol(sVersion.Mid(pos+1))<<16);
		pos = sVersion.Find(',', pos+1);
		lVersion |= (_ttol(sVersion.Mid(pos+1))<<8);
	}
	
	CRegDWORD regval = CRegDWORD(_T("Software\\TortoiseSVN\\DontConvertBase"), 999);
	if ((DWORD)regval != 999)
	{
		// there's a leftover registry setting we have to convert and then delete it
		CRegDWORD newregval = CRegDWORD(_T("Software\\TortoiseSVN\\ConvertBase"));
		newregval = !regval;
		regval.removeValue();
	}
#if 0
	if (lVersion <= 0x01010300)
	{
		CSoundUtils::RegisterTSVNSounds();
		// remove all saved dialog positions
		CRegString(_T("Software\\TortoiseSVN\\TortoiseProc\\ResizableState\\")).removeKey();
		CRegDWORD(_T("Software\\TortoiseSVN\\RecursiveOverlay")).removeValue();
		// remove the external cache key
		CRegDWORD(_T("Software\\TortoiseSVN\\ExternalCache")).removeValue();
	}
#endif	
	if (lVersion <= 0x01020200)
	{
		// upgrade to > 1.2.3 means the doc diff scripts changed from vbs to js
		// so remove the diff/merge scripts if they're the defaults
		CRegString diffreg = CRegString(_T("Software\\TortoiseSVN\\DiffTools\\.doc"));
		CString sDiff = diffreg;
		CString sCL = _T("wscript.exe \"") + CPathUtils::GetAppParentDirectory()+_T("Diff-Scripts\\diff-doc.vbs\"");
		if (sDiff.Left(sCL.GetLength()).CompareNoCase(sCL)==0)
			diffreg = _T("");
		CRegString mergereg = CRegString(_T("Software\\TortoiseSVN\\MergeTools\\.doc"));
		sDiff = mergereg;
		sCL = _T("wscript.exe \"") + CPathUtils::GetAppParentDirectory()+_T("Diff-Scripts\\merge-doc.vbs\"");
		if (sDiff.Left(sCL.GetLength()).CompareNoCase(sCL)==0)
			mergereg = _T("");
	}
	if (lVersion <= 0x01040000)
	{
		CRegStdWORD(_T("Software\\TortoiseSVN\\OwnerdrawnMenus")).removeValue();
	}
	
	// set the custom diff scripts for every user
	CString scriptsdir = CPathUtils::GetAppParentDirectory();
	scriptsdir += _T("Diff-Scripts");
	CSimpleFileFind files(scriptsdir);
	while (files.FindNextFileNoDirectories())
	{
		CString file = files.GetFilePath();
		CString filename = files.GetFileName();
		CString ext = file.Mid(file.ReverseFind('-')+1);
		ext = _T(".")+ext.Left(ext.ReverseFind('.'));
		CString kind;
		if (file.Right(3).CompareNoCase(_T("vbs"))==0)
		{
			kind = _T(" //E:vbscript");
		}
		if (file.Right(2).CompareNoCase(_T("js"))==0)
		{
			kind = _T(" //E:javascript");
		}
		
		if (filename.Left(5).CompareNoCase(_T("diff-"))==0)
		{
			CRegString diffreg = CRegString(_T("Software\\TortoiseSVN\\DiffTools\\")+ext);
			CString diffregstring = diffreg;
			if ((diffregstring.IsEmpty()) || (diffregstring.Find(filename)>=0))
				diffreg = _T("wscript.exe \"") + file + _T("\" %base %mine") + kind;
		}
		if (filename.Left(6).CompareNoCase(_T("merge-"))==0)
		{
			CRegString diffreg = CRegString(_T("Software\\TortoiseSVN\\MergeTools\\")+ext);
			CString diffregstring = diffreg;
			if ((diffregstring.IsEmpty()) || (diffregstring.Find(filename)>=0))
				diffreg = _T("wscript.exe \"") + file + _T("\" %merged %theirs %mine %base") + kind;
		}
	}

	// Initialize "Software\\TortoiseSVN\\DiffProps" once with the same value as "Software\\TortoiseSVN\\Diff"
	CRegString regDiffPropsPath = CRegString(_T("Software\\TortoiseSVN\\DiffProps"),_T("non-existant"));
	CString strDiffPropsPath = regDiffPropsPath;
	if ( strDiffPropsPath==_T("non-existant") )
	{
		CString strDiffPath = CRegString(_T("Software\\TortoiseSVN\\Diff"));
		regDiffPropsPath = strDiffPath;
	}

	// set the current version so we don't come here again until the next update!
	regVersion = _T(STRPRODUCTVER);	
}

void CTortoiseProcApp::EnableCrashHandler()
{
	// the crash handler is enabled by default, but we disable it
	// after 3 months after a release

#define YEAR ((((__DATE__ [7] - '0') * 10 + (__DATE__ [8] - '0')) * 10  \
              + (__DATE__ [9] - '0')) * 10 + (__DATE__ [10] - '0'))

#define MONTH (__DATE__ [2] == 'n' ? (__DATE__ [1] == 'a' ? 1 : 6)      \
                : __DATE__ [2] == 'b' ? 2                               \
                : __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? 3 : 4)   \
                : __DATE__ [2] == 'y' ? 5								\
                : __DATE__ [2] == 'l' ? 7                               \
                : __DATE__ [2] == 'g' ? 8                               \
                : __DATE__ [2] == 'p' ? 9                               \
                : __DATE__ [2] == 't' ? 10                               \
                : __DATE__ [2] == 'v' ? 11 : 12)

 #define DAY ((__DATE__ [4] == ' ' ? 0 : __DATE__ [4] - '0') * 10       \
              + (__DATE__ [5] - '0'))

 #define DATE_AS_INT (((YEAR - 2000) * 12 + MONTH) * 31 + DAY)

	CTime compiletime(YEAR, MONTH, DAY, 0, 0, 0);
	CTime now = CTime::GetCurrentTime();

	CTimeSpan timediff = now-compiletime;
	if (timediff.GetDays() > 3*31)
	{
//		crasher.Enable(FALSE);
	}
}

int CTortoiseProcApp::ExitInstance()
{
	CWinAppEx::ExitInstance();
	if (retSuccess)
		return 0;
	return -1;
}
