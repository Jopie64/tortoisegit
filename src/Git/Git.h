#pragma once
#include "GitRev.h"
#include "GitStatus.h"
#include "GitAdminDir.h"
#ifndef __AFXCOLL_H__
#include "GitStringList.h"
typedef CGitStringList CStringList;
#endif //ifndef __AFXCOLL_H__

enum
{
	GIT_SUCCESS=0,
	GIT_ERROR_OPEN_PIP,
	GIT_ERROR_CREATE_PROCESS,
	GIT_ERROR_GET_EXIT_CODE
};

typedef std::vector<CString> STRING_VECTOR;
typedef std::map<CString, STRING_VECTOR> MAP_HASH_NAME;

class CGit
{
private:
	GitAdminDir m_GitDir;
public:
	static CString m_MsysGitPath;
	CGit(void);
	~CGit(void);
	int Run(CString cmd, CString* output);
	int RunAsync(CString cmd,PROCESS_INFORMATION *pi, HANDLE* hRead, CString *StdioFile=NULL);
	int RunLogFile(CString cmd, CString &filename);
	CString GetUserName(void);
	CString GetUserEmail(void);
	CString GetCurrentBranch(void);

	bool SetCurrentDir(CString path)
	{
		return m_GitDir.HasAdminDir(path,&m_CurrentDir);
	}
	CString m_CurrentDir;

	typedef enum
	{
		BRANCH_LOCAL=0x1,
		BRANCH_REMOTE=0x2,
		BRANCH_ALL=BRANCH_LOCAL|BRANCH_REMOTE,
	}BRANCH_TYPE;

	int GetRemoteList(CStringList &list);
	int GetBranchList(CStringList &list, int *Current,BRANCH_TYPE type=BRANCH_LOCAL);
	int GetTagList(CStringList &list);
	int GetMapHashToFriendName(MAP_HASH_NAME &map);
	
	//hash is empty means all. -1 means all
	int GetLog(CString& logOut,CString &hash, int count=-1);

	git_revnum_t GetHash(CString &friendname);

	int BuildOutputFormat(CString &format,bool IsFull=TRUE);
	int GetShortLog(CString &log);
	
	
};
extern void GetTempPath(CString &path);
extern CString GetTempFile();


extern CGit g_Git;