#pragma once
#include <queue>
#include "simplebind.h"

namespace Threading
{

class CHandle
{
public:
	CHandle(HANDLE handle = NULL);
	virtual ~CHandle();

	void Close();

	HANDLE	H() const {return m_Handle;}
	void	Attach(HANDLE handle);
	HANDLE	Detach();

private:
	HANDLE m_Handle;
};

class CEvent : public CHandle
{
public:
	CEvent();

};

class CCritSect
{
public:
	CCritSect();
	~CCritSect();

private:
	void Lock();
	void Unlock();

	CRITICAL_SECTION m_CritSect;

	friend class CScopeLock;
};

class CScopeLock
{
public:
	CScopeLock(CCritSect& critSect):m_CritSect(critSect){m_CritSect.Lock();}
	~CScopeLock(){m_CritSect.Unlock();}

private:
	CCritSect& m_CritSect;
};

class CMsg
{
public:
	virtual void Call() = 0;
};

template<class callback_T>
class CCallbackMsg : public CMsg
{
public:
	CCallbackMsg(const callback_T& callback):m_callback(callback){}
	virtual void Call()
	{
		m_callback();
	}
	callback_T m_callback;
};

template <class callback_T>
CCallbackMsg<callback_T>* CreateCallbackMsg(const callback_T& callback)
{
	return new CCallbackMsg<callback_T>(callback);
}

class CMsgQueue
{
public:
	typedef std::queue<CMsg*> tMsgQueue;

	CMsgQueue();
	~CMsgQueue();

	void  Add(CMsg* pMsg);
	CMsg* Get();
	CMsg* Curr();
	CMsg* Peek();

	void  Clear();

private:
	CMsg*	  m_pCurrMsg;
	CCritSect m_QueueCs;
	tMsgQueue m_MsgQueue;
};

class CMsgThread
{
public:
	CMsgThread();
	void PostMessage(CMsg* pMsg);
	
	template<class callback_T>
	void PostCallback(const callback_T& callback)
	{
		PostMessage(CreateCallbackMsg(callback));
	}



	virtual void Trigger(){}// = 0;//Notify thread a message is in the queue

	bool CallMessages();//call pending messages. Returns true when thread should exit.

	void PostQuitMessage(int exitCode = 0);

	int  GetThreadExitCode()const {return m_threadExitCode;}

private:
	void		SetQuitInfo(int threadExitCode);

	CMsgQueue	m_MsgQueue;
	bool		m_bQuit;
	int			m_threadExitCode;
};


class CThreading
{
public:
	CThreading(void);
	~CThreading(void);
};

}