#include <functional>

template<class Result_T, class Functor_T>
class CSimpleBinder_0
{
public:
	CSimpleBinder_0(const Functor_T& functor):m_Functor(functor){}

	Result_T operator()(){return m_Functor();}
	
	Functor_T m_Functor;
};

template<class Result_T, class Functor_T, class P1_T>
class CSimpleBinder_1
{
public:
	CSimpleBinder_1(const Functor_T& functor, const P1_T& p1):m_Functor(functor), m_p1(p1){}

	Result_T operator()(){return m_Functor(m_p1);}
	
	Functor_T	m_Functor;
	P1_T		m_p1;
};

template<class Result_T, class Functor_T, class P1_T, class P2_T>
class CSimpleBinder_2
{
public:
	CSimpleBinder_2(const Functor_T& functor, const P1_T& p1, const P2_T& p2):m_Functor(functor), m_p1(p1), m_p2(p2){}

	Result_T operator()(){return m_Functor(m_p1, m_p2);}
	
	Functor_T	m_Functor;
	P1_T		m_p1;
	P2_T		m_p2;
};

template<class Result_T, class Obj_T>
CSimpleBinder_1<Result_T, std::mem_fun_t<Result_T, Obj_T>, Obj_T*> simplebind(Result_T (Obj_T::*func)(), Obj_T* pThis)
{
	return CSimpleBinder_1<Result_T, std::mem_fun_t<Result_T, Obj_T>, Obj_T*>(std::mem_fun(func),pThis);
}

template<class Result_T, class Obj_T, class P1_T>
CSimpleBinder_2<Result_T, std::mem_fun1_t<Result_T, Obj_T, P1_T>, Obj_T*, P1_T> simplebind(Result_T (Obj_T::*func)(P1_T), Obj_T* pThis, const P1_T& p1)
{
	return CSimpleBinder_2<Result_T, std::mem_fun1_t<Result_T, Obj_T, P1_T>, Obj_T*, P1_T>(std::mem_fun(func), pThis, p1);
}

template<class Result_T>
CSimpleBinder_0<Result_T, Result_T (*)()> simplebind(Result_T (*func)())
{
	return CSimpleBinder_0<Result_T, Result_T (*)()>(func);
}

template<class Result_T, class P1_T>
CSimpleBinder_1<Result_T, Result_T (*)(P1_T), P1_T> simplebind(Result_T (*func)(P1_T), const P1_T& p1)
{
	return CSimpleBinder_1<Result_T, Result_T (*)(P1_T), P1_T>(func, p1);
}

template<class Result_T, class P1_T, class P2_T>
CSimpleBinder_2<Result_T, Result_T (*)(P1_T, P2_T), P1_T, P2_T> simplebind(Result_T (*func)(P1_T, P2_T), const P1_T& p1, const P2_T& p2)
{
	return CSimpleBinder_2<Result_T, Result_T (*)(P1_T, P2_T), P1_T, P2_T>(func, p1, p2);
}
