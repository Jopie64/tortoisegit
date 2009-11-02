#pragma once
#include <queue>
#include "simplebind.h"

namespace Threading
{

template <class T_Derived>
class CSingleton
{
public:
	CSingleton(){ASSERT(m_pInterface == NULL);}

	static T_Derived* I()
	{
		if(m_pInterface == NULL)
			m_pInterface = new T_Derived;
		return m_pInterface;
	}

	static void Singleton_Destroy()
	{
		delete m_pInterface;
		m_pInterface = NULL;
	}

private:

	static T_Derived* m_pInterface;
};

template <class T_Derived>
T_Derived* CSingleton<T_Derived>::m_pInterface = NULL;

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

class CRunnableBase
{
public:
	virtual UINT Run() = 0;

};

class CThread : public CHandle
{
public:
	CThread(DWORD IdThread):m_IdThread(IdThread){Open();}

	void Open(DWORD IdThread = 0, DWORD dwAccess = SYNCHRONIZE ){if(IdThread != 0)m_IdThread = IdThread; Attach(OpenThread(dwAccess, FALSE, m_IdThread));}
	void Join(){WaitForSingleObject(H(),INFINITE);}
private:
	DWORD m_IdThread;
	
};

template<class T_Callback>
class CRunnableCb : public CRunnableBase
{
public:
	CRunnableCb(const T_Callback& CB):m_CB(CB){}

protected:
	UINT Run(){m_CB();return 0;}

	T_Callback m_CB;
};

DWORD ExecAsync_Td(CRunnableBase* pTd);

template<class T_Callback>
DWORD ExecAsync(const T_Callback& CB)
{
	return ExecAsync_Td(new CRunnableCb<T_Callback>(CB));
}

class CMsg
{
public:
	virtual ~CMsg(){}
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
	int	  Size();

private:
	CMsg*	  m_pCurrMsg;
	CCritSect m_QueueCs;
	tMsgQueue m_MsgQueue;
};

class CMsgThread
{
public:
	CMsgThread();
	virtual ~CMsgThread();

	void Register(DWORD IdThread = 0);
	void Unregister();

	bool IsRegistered() const {return m_IdThread != 0;}
	DWORD GetIdThread() const {return m_IdThread;}

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

	bool IsThisThread() const {return GetIdThread() == GetCurrentThreadId();}
	bool IsTriggered() {return m_MsgQueue.Size() > 1;}


private:
	void		SetQuitInfo(int threadExitCode);

	DWORD		m_IdThread;
	CMsgQueue	m_MsgQueue;
	bool		m_bQuit;
	int			m_threadExitCode;
};

class CWinMlHook : public CMsgThread
{
public:
	CWinMlHook();
	~CWinMlHook();
	
	static  LRESULT CALLBACK StaticCallback(int code, WPARAM wParam, LPARAM lParam);
	LRESULT Callback(int code, WPARAM wParam, LPARAM lParam);

	void Attach();
	void Detach();
	void Trigger();

	HHOOK m_hHook;
};

typedef std::map<DWORD, CMsgThread*> T_MsgThreadMap;
class CThreads : public CSingleton<CThreads>
{
public:
	CThreads(void);
	~CThreads(void);

	void	RegisterThread(CMsgThread* pThread, DWORD IdThread = 0);
	void	UnregisterThread(DWORD IdThread = 0);

	CMsgThread* Get(DWORD IdThread = 0);

private:

	T_MsgThreadMap	m_MsgThreadMap;
	CCritSect		m_CS;

};

}