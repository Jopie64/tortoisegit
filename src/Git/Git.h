#pragma once
#include "GitType.h"
#include "GitRev.h"
#include "GitStatus.h"
#include "GitAdminDir.h"

class CGitCall
{
public:
	CGitCall(){}
	CGitCall(CString cmd):m_Cmd(cmd){}

	CString			GetCmd()const{return m_Cmd;}
	void			SetCmd(CString cmd){m_Cmd=cmd;}

	//This function is called when command output data is available.
	//When this function returns 'true' the git command should be aborted.
	//This behavior is not implemented yet.
	virtual bool	OnOutputData(const BYTE* data, size_t size)=0;
	virtual void	OnEnd(){}

private:
	CString m_Cmd;

};

class CGitCall_Collector : public CGitCall
{
public:
	CGitCall_Collector(const CString& cmd, const CStringA sep):CGitCall(cmd), m_SepToken(sep), m_bInOnCollected(false){}
	bool			OnOutputData(const BYTE* data, size_t size);

	void			SetSepToken(const CStringA m_SepToken);
	void			CheckCollectedData();


	virtual bool	Abort(){return false;}
	virtual void	OnCollected(BYTE_VECTOR& Data) = 0;
	virtual void	OnEnd(){if(!m_ByteCollector.empty())OnCollected(m_ByteCollector);}

private:
	BYTE_VECTOR		m_ByteCollector;
	CStringA		m_SepToken;
	bool			m_bInOnCollected;
};

class CTGitPath;

class CGit
{
private:
	GitAdminDir m_GitDir;
public:
	static BOOL CheckMsysGitDir();
	static CString ms_LastMsysGitDir;	// the last msysgitdir added to the path, blank if none
	static int m_LogEncode;
	unsigned int Hash2int(CString &hash);
//	static CString m_MsysGitPath;
	
	PROCESS_INFORMATION m_CurrentGitPi;

	CGit(void);
	~CGit(void);
	
	int Run(CString cmd, CString* output,int code);
	int Run(CString cmd, BYTE_VECTOR *byte_array);
	int Run(CGitCall* pcall);

	int RunAsync(CString cmd,PROCESS_INFORMATION *pi, HANDLE* hRead, CString *StdioFile=NULL);
	int RunLogFile(CString cmd, CString &filename);

	bool IsFastForward(CString &from, CString &to);
	CString GetConfigValue(CString name);
	CString GetUserName(void);
	CString GetUserEmail(void);
	CString GetCurrentBranch(void);
	CString GetSymbolicRef(const wchar_t* symbolicRefName = L"HEAD", bool bStripRefsHeads = true);
	// read current branch name from HEAD file, returns 0 on success, -1 on failure, 1 detached (branch name "HEAD" returned)
	int GetCurrentBranchFromFile(const CString &sProjectRoot, CString &sBranchOut);
	BOOL CheckCleanWorkTree();
	int Revert(CTGitPath &path,bool keep=true);
	int Revert(CTGitPathList &list,bool keep=true);

	bool SetCurrentDir(CString path)
	{
		bool b = m_GitDir.HasAdminDir(path,&m_CurrentDir);
		if(m_CurrentDir.GetLength() == 2 && m_CurrentDir[1] == _T(':')) //C: D:
		{
			m_CurrentDir+=_T('\\');
		}
		return b;
	}
	CString m_CurrentDir;

	typedef enum
	{
		BRANCH_LOCAL=0x1,
		BRANCH_REMOTE=0x2,
		BRANCH_ALL=BRANCH_LOCAL|BRANCH_REMOTE,
	}BRANCH_TYPE;

	typedef enum
	{
		LOG_INFO_STAT=0x1,
		LOG_INFO_FILESTATE=0x2,
		LOG_INFO_PATCH=0x4,
		LOG_INFO_FULLHISTORY=0x8,
		LOG_INFO_BOUNDARY=0x10,
        LOG_INFO_ALL_BRANCH=0x20,
		LOG_INFO_ONLY_HASH=0x40,
		LOG_INFO_DETECT_RENAME=0x80,
		LOG_INFO_DETECT_COPYRENAME=0x100,
		LOG_INFO_FIRST_PARENT = 0x200,
		LOG_INFO_NO_MERGE = 0x400,
		LOG_INFO_FOLLOW = 0x800,
		LOG_INFO_SHOW_MERGEDFILE=0x1000,
		LOG_INFO_FULL_DIFF = 0x2000,
	}LOG_INFO_MASK;

	int GetRemoteList(STRING_VECTOR &list);
	int GetBranchList(STRING_VECTOR &list, int *Current,BRANCH_TYPE type=BRANCH_LOCAL);
	int GetTagList(STRING_VECTOR &list);
	int GetMapHashToFriendName(MAP_HASH_NAME &map);
	
	//hash is empty means all. -1 means all

	int GetLog(CGitCall* pgitCall, CString &hash, CTGitPath *path = NULL,int count=-1,int InfoMask=LOG_INFO_FULL_DIFF|LOG_INFO_STAT|LOG_INFO_FILESTATE|LOG_INFO_BOUNDARY|LOG_INFO_DETECT_COPYRENAME|LOG_INFO_SHOW_MERGEDFILE,
								CString *from=NULL,CString *to=NULL);
	int GetLog(BYTE_VECTOR& logOut,CString &hash, CTGitPath *path = NULL,int count=-1,int InfoMask=LOG_INFO_FULL_DIFF|LOG_INFO_STAT|LOG_INFO_FILESTATE|LOG_INFO_BOUNDARY|LOG_INFO_DETECT_COPYRENAME|LOG_INFO_SHOW_MERGEDFILE,
								CString *from=NULL,CString *to=NULL);

	BOOL EnumFiles(const TCHAR *pszProjectPath, const TCHAR *pszSubPath, unsigned int nFlags, WGENUMFILECB *pEnumCb, void *pUserData);

	git_revnum_t GetHash(const CString &friendname);

	int BuildOutputFormat(CString &format,bool IsFull=TRUE);
	//int GetShortLog(CString &log,CTGitPath * path=NULL, int count =-1);
	static void StringAppend(CString *str,BYTE *p,int code=CP_UTF8,int length=-1);

	BOOL IsInitRepos();
	int ListConflictFile(CTGitPathList &list,CTGitPath *path=NULL);
	int GetRefList(STRING_VECTOR &list);


	//Example: master -> refs/heads/master
	CString GetFullRefName(CString shortRefName);
	//Removes 'refs/heads/' or just 'refs'. Example: refs/heads/master -> master
	static CString StripRefName(CString refName);

	int GetCommitDiffList(CString &rev1,CString &rev2,CTGitPathList &outpathlist);
	
};
extern void GetTempPath(CString &path);
extern CString GetTempFile();


extern CGit g_Git;

inline static BOOL wgEnumFiles(const TCHAR *pszProjectPath, const TCHAR *pszSubPath, unsigned int nFlags, WGENUMFILECB *pEnumCb, void *pUserData) { return g_Git.EnumFiles(pszProjectPath, pszSubPath, nFlags, pEnumCb, pUserData); }
