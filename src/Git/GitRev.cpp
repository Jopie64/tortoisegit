#include "StdAfx.h"
#include "GitRev.h"
#include "Git.h"

#ifndef ASSERT
void GitAssert(bool P_bVal, const char* P_szErrorPtr)
{
//	if(!P_bVal)
//		MessageBox("Show some error here. Statement is in P_szErrorPtr");
}

#define GIT_STRINGER(P_Str) #P_Str
#define ASSERT(P_Stmt) GitAssert(P_Stmt,GIT_STRINGER(P_Stmt));
#endif //ifndef ASSERT


GitRev::GitRev(void)
{
	m_Action=0;
	m_IsFull = 0;
	m_IsUpdateing = 0;
}

GitRev::~GitRev(void)
{
}

#if 0
GitRev::GitRev(GitRev & rev)
{
}
GitRev& GitRev::operator=(GitRev &rev)
{
	return *this;
}
#endif
void GitRev::Clear()
{
	this->m_Action=0;
	this->m_Files.Clear();
	this->m_Action=0;
	this->m_ParentHash.clear();
	m_CommitterName.Empty();
	m_CommitterEmail.Empty();
	m_Body.Empty();
	m_Subject.Empty();
	m_CommitHash.Empty();

}
int GitRev::CopyFrom(GitRev &rev)
{
	m_AuthorName	=rev.m_AuthorName	;
	m_AuthorEmail	=rev.m_AuthorEmail	;
	m_AuthorDate	=rev.m_AuthorDate	;
	m_CommitterName	=rev.m_CommitterName	;
	m_CommitterEmail=rev.m_CommitterEmail;
	m_CommitterDate	=rev.m_CommitterDate	;
	m_Subject		=rev.m_Subject		;
	m_Body			=rev.m_Body			;
	m_CommitHash	=rev.m_CommitHash	;
	m_ParentHash	=rev.m_ParentHash	;
	m_Files			=rev.m_Files			;	
	m_Action		=rev.m_Action		;
	return 0;
}
int GitRev::ParserFromLog(CString &log)
{
	int pos=0;
	CString one;
	CString key;
	CString text;
	CString filelist;
	TCHAR mode;
	CTGitPath  path;
	this->m_Files.Clear();
    m_Action=0;

	while( pos>=0 )
	{
		one=log.Tokenize(_T("\n"),pos);
		if(one[0]==_T('#') && one[1] == _T('<') && one[3] == _T('>'))
		{
			text = one.Right(one.GetLength()-4);
			mode = one[2];
			switch(mode)
			{
			case LOG_REV_ITEM_BEGIN:
				this->Clear();

			case LOG_REV_AUTHOR_NAME:
				this->m_AuthorName = text;
				break;
			case LOG_REV_AUTHOR_EMAIL:
				this->m_AuthorEmail = text;
				break;
			case LOG_REV_AUTHOR_DATE:
				this->m_AuthorDate =ConverFromString(text);
				break;
			case LOG_REV_COMMIT_NAME:
				this->m_CommitterName = text;
				break;
			case LOG_REV_COMMIT_EMAIL:
				this->m_CommitterEmail = text;
				break;
			case LOG_REV_COMMIT_DATE:
				this->m_CommitterDate =ConverFromString(text);
				break;
			case LOG_REV_COMMIT_SUBJECT:
				this->m_Subject = text;
				break;
			case LOG_REV_COMMIT_BODY:
				this->m_Body = text +_T("\n");
				break;
			case LOG_REV_COMMIT_HASH:
				this->m_CommitHash = text;
				break;
			case LOG_REV_COMMIT_PARENT:
				this->m_ParentHash.insert(this->m_ParentHash.end(),text);
				break;
			case LOG_REV_COMMIT_FILE:
				break;
			}
		}else
		{
			//Todo: Solve warning here. mode is indeed uninitialized here
			switch(mode)
			{
			case LOG_REV_COMMIT_BODY:
				this->m_Body += one+_T("\n");
				break;
			case LOG_REV_COMMIT_FILE:
				filelist += one +_T("\n");
				break;
			}
		}
	}
	
	this->m_Files.ParserFromLog(filelist);
	this->m_Action=this->m_Files.GetAction();
	return 0;
}

CTime GitRev::ConverFromString(CString input)
{
	CTime tm(_wtoi(input.Mid(0,4)),
			 _wtoi(input.Mid(5,2)),
			 _wtoi(input.Mid(8,2)),
			 _wtoi(input.Mid(11,2)),
			 _wtoi(input.Mid(14,2)),
			 _wtoi(input.Mid(17,2)),
			 _wtoi(input.Mid(20,4)));
	return tm;
}

int GitRev::SafeFetchFullInfo(CGit *git)
{
	if(InterlockedExchange(&m_IsUpdateing,TRUE) == FALSE)
	{
		//GitRev rev;
		CString onelog;
		git->GetLog(onelog,m_CommitHash,1);
		CString oldhash=m_CommitHash;
		ParserFromLog(onelog);
		
		ASSERT(oldhash==m_CommitHash);

		InterlockedExchange(&m_IsUpdateing,FALSE);
		InterlockedExchange(&m_IsFull,TRUE);
		return 0;
	}
	return -1;
}