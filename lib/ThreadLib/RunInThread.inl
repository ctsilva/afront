BEGIN_THREADLIB_NAMESPACE

template<class T, class ARG>
inline CRunInThread<T,ARG>::CRunInThread(T* p_class, mem_fun_type p_mem, ARG arg, int priority) :
    _p_class(p_class),
    _p_mem(p_mem),
    _arg(arg),
    _h_thread(ThreadEntry, this, priority)
{
}

template<class T, class ARG>
inline CRunInThread<T,ARG>::~CRunInThread()
{
    CloseHandle(h_thread);
    printf("Run in thread is gone\n");
}

template<class T, class ARG>
inline void CRunInThread<T,ARG>::WaitForThread()
{
    void *dummy;
    _h_thread.join(&dummy);
}

template<class T, class ARG>
inline void CRunInThread<T,ARG>::runit()
{
    (_p_class->*_p_mem)(_arg);
}

template<class T, class ARG>
inline void CRunInThread<T,ARG>::ThreadEntry(CRunInThread* that)
{
    that->runit();
}


/*------------------ CRunInThreadAndForget ------------------*/
template <class T, class ARG, class CLEANUP>
inline CRunInThreadAndForget<T,ARG,CLEANUP>::CRunInThreadAndForget(T* p_class, mem_fun_type p_mem, ARG arg, CLEANUP* p_object) :
    CRunInThread<T,ARG>(p_class, p_mem, arg),
    _p_object(p_object)
{
    Thread t(LocalThreadEntry, this);
    //    HANDLE h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)LocalThreadEntry, this, 0, NULL);
    //    CloseHandle(h);
}

template <class T, class ARG, class CLEANUP>
inline void CRunInThreadAndForget<T,ARG,CLEANUP>::LocalThreadEntry(CRunInThreadAndForget* that)
{
    that->runit();
    delete that;
}

template <class T, class ARG, class CLEANUP>
inline void CRunInThreadAndForget<T,ARG,CLEANUP>::runit()
{
    WaitForThread();
    delete _p_object;
}

END_THREADLIB_NAMESPACE
