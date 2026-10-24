
/*

Copyright 2007 University of Utah


This file is part of Afront.

Afront is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Afront is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA

*/


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
