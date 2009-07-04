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
	m_threadExitCode(0)
{
}

void CMsgThread::PostMessage(CMsg* pMsg)
{
	m_MsgQueue.Add(pMsg);
	Trigger();
}

bool CMsgThread::CallMessages()
{
	CMsg* pMsg;
	while(pMsg = m_MsgQueue.Get())
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


CThreading::CThreading(void)
{
}

CThreading::~CThreading(void)
{
}


}