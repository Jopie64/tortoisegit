#include "StdAfx.h"
#include "Threading.h"
//#include <boost/bind.hpp>

namespace Threading
{

CHandle::CHandle(HANDLE handle)
:	m_Handle(handle)
{
}

CHandle::~CHandle()
{
	Close();
}

void CHandle::Close()
{
	if(m_Handle != NULL)
		CloseHandle(m_Handle);
	m_Handle = NULL;
}

void CHandle::Attach(HANDLE handle)
{
	Close();
	m_Handle = handle;
}

HANDLE CHandle::Detach()
{
	HANDLE returnHandle = m_Handle;
	m_Handle = NULL;
	return returnHandle;
}

CEvent::CEvent()
{
	CreateEvent(NULL, FALSE, FALSE, NULL);
}

CCritSect::CCritSect()
{
	InitializeCriticalSection(&m_CritSect);
}

CCritSect::~CCritSect()
{
	DeleteCriticalSection(&m_CritSect);
}

void CCritSect::Lock()
{
	EnterCriticalSection(&m_CritSect);
}

void CCritSect::Unlock()
{
	LeaveCriticalSection(&m_CritSect);
}

UINT ExecAsync_Entry(LPVOID pVoid)
{
	CRunnableBase* pTd = (CRunnableBase*)pVoid;
	UINT Result = pTd->Run();
	delete pTd;
	return Result;
}

DWORD ExecAsync_Td(CRunnableBase* pTd)
{
	CWinThread* pWinTd = AfxBeginThread(ExecAsync_Entry, pTd);
	if(pWinTd == NULL)
		return 0;
	return pWinTd->m_nThreadID;
}


CMsgQueue::CMsgQueue()
:	m_pCurrMsg(NULL)
{
}

CMsgQueue::~CMsgQueue()
{
	Clear();
}

void CMsgQueue::Clear()
{
	CScopeLock lock(m_QueueCs);
	while(Get());
}

void  CMsgQueue::Add(CMsg* pMsg)
{
	CScopeLock lock(m_QueueCs);
	m_MsgQueue.push(pMsg);
}

int CMsgQueue::Size()
{
	CScopeLock lock(m_QueueCs);
	return m_MsgQueue.size();
}


CMsg* CMsgQueue::Get()
{
	CScopeLock lock(m_QueueCs);
	delete m_pCurrMsg;
	if(m_MsgQueue.empty())
		m_pCurrMsg = NULL;
	else
	{
		m_pCurrMsg = m_MsgQueue.front();
		m_MsgQueue.pop();
	}
	return m_pCurrMsg;
}

CMsg* CMsgQueue::Curr()
{
	CScopeLock lock(m_QueueCs);
	return m_pCurrMsg;
}

CMsg* CMsgQueue::Peek()
{
	CScopeLock lock(m_QueueCs);
	if(m_MsgQueue.empty())
		return NULL;
	return m_MsgQueue.front();
}

CMsgThread::CMsgThread()
:	m_bQuit(false),
	m_threadExitCode(0),
	m_IdThread(0)
{
}

CMsgThread::~CMsgThread()
{
	Unregister();
}

void CMsgThread::Register(DWORD IdThread)
{
	if(IdThread == 0)
		IdThread = GetCurrentThreadId();
	m_IdThread = IdThread;
	CThreads::I()->RegisterThread(this, IdThread);
}

void CMsgThread::Unregister()
{
	if(m_IdThread == 0)
		return;
	CThreads::I()->UnregisterThread(m_IdThread);
	m_IdThread = 0;
}

void CMsgThread::PostMessage(CMsg* pMsg)
{
	m_MsgQueue.Add(pMsg);
	Trigger();
}

bool CMsgThread::CallMessages()
{
	CMsg* pMsg;
	while((pMsg = m_MsgQueue.Get()) != NULL)
	{
		if(m_bQuit)
			break;
		pMsg->Call();
	}
	return m_bQuit;
}

void CMsgThread::PostQuitMessage(int exitCode)
{
//	PostCallback(std::bind1st(std::bind1st(std::mem_fun_ref(&CMsgThread::SetQuitInfo),this),exitCode));
	PostCallback(simplebind(&CMsgThread::SetQuitInfo, this, exitCode));
}

void CMsgThread::SetQuitInfo(int threadExitCode)
{
	m_threadExitCode = threadExitCode;
	m_bQuit = true;
}
/*

class a
{
public:
	int b(int c)
	{
		ASSERT(c);
		return c;
	}
};

int test2(int i, int j)
{
	return i+j;
}

int test()
{
	a h;
//	return simplebind(&a::b,&h,3)();
	return simplebind(&test2, 5, 7)();
}

int G_i = test();
*/


CWinMlHook::CWinMlHook()
:	m_hHook(0)
{
}
CWinMlHook::~CWinMlHook()
{
	Detach();
}

LRESULT CWinMlHook::Callback(int code, WPARAM wParam, LPARAM lParam)
{
	MSG* pMsg = (MSG*)lParam;
	if(pMsg->message != WM_NULL)
		return CallNextHookEx(NULL, code, wParam, lParam);
	CallMessages();
	return CallNextHookEx(NULL, code, wParam, lParam);
}

LRESULT CWinMlHook::StaticCallback(int code, WPARAM wParam, LPARAM lParam)
{
	CWinMlHook* pThis = dynamic_cast<CWinMlHook*>(CThreads::I()->Get());
	if(pThis == NULL)
	{
		ASSERT(FALSE);
		return CallNextHookEx(NULL, code, wParam, lParam);;
	}
	return pThis->Callback(code, wParam, lParam);
}

void CWinMlHook::Attach()
{
	Detach();
	Register();
	m_hHook = SetWindowsHookEx(WH_GETMESSAGE, &CWinMlHook::StaticCallback, NULL, GetIdThread());
	PostThreadMessage(GetIdThread(), WM_NULL, 0, 0);//Manual trigger
}

void CWinMlHook::Detach()
{
	if(m_hHook == NULL)
		return; //Nothing to detach...
	Unregister();
	UnhookWindowsHookEx(m_hHook);
	m_hHook = 0;
}

void CWinMlHook::Trigger()
{
	if(!IsRegistered())
		return;
	if(!IsTriggered())
		PostThreadMessage(GetIdThread(), WM_NULL, 0, 0);
}



CThreads::CThreads(void)
{
}

CThreads::~CThreads(void)
{
}

CMsgThread* CThreads::Get(DWORD IdThread)
{
	CScopeLock Lock(m_CS);
	if(IdThread == 0)
		IdThread = GetCurrentThreadId();
	T_MsgThreadMap::iterator it = m_MsgThreadMap.find(IdThread);
	if(it == m_MsgThreadMap.end())
		return NULL;
	return it->second;
}

void CThreads::RegisterThread(CMsgThread* pThread, DWORD IdThread)
{
	CScopeLock Lock(m_CS);
	if(IdThread == 0)
		IdThread = GetCurrentThreadId();
	m_MsgThreadMap[IdThread] = pThread;
}

void CThreads::UnregisterThread(DWORD IdThread)
{
	CScopeLock Lock(m_CS);
	m_MsgThreadMap.erase(IdThread);
}


}