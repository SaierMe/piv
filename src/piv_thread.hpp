/*********************************************\
 * 火山视窗PIV模块 - 线程池类                *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
 * 版本: 2023/01/31                          *
\*********************************************/

#ifndef _PIV_THREAD_POOL_HPP
#define _PIV_THREAD_POOL_HPP

// 包含火山视窗基本类,它在火山里的包含顺序比较靠前(全局-110)
#ifndef __VOL_BASE_H__
#include <sys/base/libs/win_base/vol_base.h>
#endif

/*************************************************************************\
 * 线程池类 PivThreadPool                                                *
 * 功能强、效率快、高稳定、无内存和句柄泄漏的线程池.                     *
 * 线程的回调函数在接收事件中编写,不需要使用静态方法,传参和操作UI更简单. *
 * 可在UI线程中执行耗时操作不卡界面.                                     *
 * 完全使用CPP源码和火山语法实现,除了视窗基本类,不依赖其他类库和DLL.     *
\*************************************************************************/

class PivThreadPool
{
protected:
    typedef INT(CALLBACK *WORKERPROC)(LPVOID lpParam);
    enum ThreadPoolState
    {
        StateClosed = 0,   // 线程池已被关闭
        StateClosing = 1,  // 正在关闭线程池
        StateStarting = 2, // 正在创建线程池
        StateNormal = 3,   // 线程池正常工作
        StateAdjust = 4,   // 调整线程池容量
        StateSuspend = 5,  // 线程池暂停
        StateClear = 6     // 正在清空任务
    };
    struct THREADPOOLINFO
    {
        HANDLE hCompletionPort; // IOCP完成端口
        SIZE_T tStackSize;      // 线程堆栈大小
        HANDLE hEvent[4];       // 线程事件(0=创建,1=销毁,2=暂停,3=退出扩展函数)
        DWORD dwState;          // 工作状态(ThreadPoolState)
        DWORD dwThreadsCount;   // 现行线程数
        DWORD dwCapacity;       // 线程池容量/最大线程数(Ex)
        DWORD dwThreadsMin;     // 最小线程数(Ex)
        DWORD dwCycleMs;        // 动态线程检测周期时间(Ex)
        INT nPriority;          // 线程优先级
        DWORD dwWorkerTask;     // 执行任务数
        DWORD dwQueueTask;      // 队列任务数量
        DWORD dwComplateTask;   // 已完成任务数
    };
#define PIV_THREADPOOL_EXIT 10086 // 退出线程
public:
    struct THREADPOOLPARAM
    {
        void *lpThis;     // 对象指针
        INT nUserMark;    // 用户标记
        INT64 lUserParam; // 用户参数
        INT_P lpUserData; // 用户数据
    };
    struct THREADPOOLOBJECT
    {
        INT nMark;             // 类型标记
        void *lpThis;          // 对象指针
        INT nUserMark;         // 用户标记
        CVolObject *lpUserObj; // 用户对象
        INT_P lpUserData;      // 用户数据
    };

protected:
    THREADPOOLINFO *m_pThreadPool = nullptr; // 线程池信息
    DWORD m_dwProcessors = 0;                // CPU逻辑核心数
    HANDLE m_hSemaphore = NULL;              // 信号灯句柄

public:
    PivThreadPool()
    {
        // 创建信号灯
        m_hSemaphore = ::CreateSemaphoreW(NULL, 0, LONG_MAX, NULL);
    }
    ~PivThreadPool()
    {
        // 强制销毁线程池,但不会强制结束仍在执行的耗时或死循环线程
        DestroyThreadPool(TRUE, 0);
        if (m_hSemaphore) // 关闭信号灯
        {
            ::CloseHandle(m_hSemaphore);
        }
    }
    // 启动线程
    BOOL StartThread(void *lpThis, ULONG_PTR lpThreadProc, INT nUserMark = 0, CVolObject &cUserObj = CVolObject(), INT_P lpUserData = 0, BOOL bWaitThread = FALSE, INT nStackSize = 0, INT nPriority = THREAD_PRIORITY_NORMAL)
    {
        // 设置回调参数
        THREADPOOLOBJECT *lpParam = new THREADPOOLOBJECT{0, lpThis, nUserMark, cUserObj.MakeCloneObject(), lpUserData};
        // 在暂停的状态下创建线程
        HANDLE hThread = ::CreateThread(NULL, static_cast<SIZE_T>(nStackSize), reinterpret_cast<LPTHREAD_START_ROUTINE>(lpThreadProc), lpParam, CREATE_SUSPENDED, NULL);
        if (!hThread)
        {
            return FALSE;
        }
        // 设置线程优先级
        if (nPriority != THREAD_PRIORITY_NORMAL)
        {
            ::SetThreadPriority(hThread, nPriority);
        }
        ::ResumeThread(hThread); // 恢复线程
        if (bWaitThread)         // 等待线程结束返回
        {
            ::WaitForSingleObject(hThread, INFINITE);
        }
        ::CloseHandle(hThread); // 关闭线程句柄
        return TRUE;
    }
    // 创建
    virtual BOOL CreateThreadPool(INT nCapacity = 0, INT nStackSize = 0, INT nPriority = THREAD_PRIORITY_NORMAL)
    {
        // 线程池已创建则直接返回真,其他情况返回假
        if (m_pThreadPool)
        {
            return (m_pThreadPool->dwState >= StateNormal) ? TRUE : FALSE;
        }
        // 给线程池参数变量分配内存
        m_pThreadPool = new THREADPOOLINFO{0};
        if (!m_pThreadPool)
        {
            return FALSE;
        }
        // 工作状态设置为正在创建
        ::InterlockedExchange(&m_pThreadPool->dwState, StateStarting);
        // 将信号灯的信号重置为0.
        ReSetSemaphore();
        // 创建事件,用于等待线程句柄被销毁
        ::InterlockedExchangePointer(&m_pThreadPool->hEvent[0], ::CreateEventW(0, FALSE, FALSE, NULL));
        // 创建事件失败时退出
        if (!m_pThreadPool->hEvent[0])
        {
            delete m_pThreadPool;
            m_pThreadPool = nullptr;
            return FALSE;
        }
        // 创建IOCP完成端口
        m_pThreadPool->hCompletionPort = ::CreateIoCompletionPort((HANDLE)-1, NULL, 0, 0);
        // 创建IOCP失败时退出
        if (!m_pThreadPool->hCompletionPort)
        {
            ::CloseHandle(m_pThreadPool->hEvent[0]);
            delete m_pThreadPool;
            m_pThreadPool = nullptr;
            return FALSE;
        }
        // 取CPU逻辑核心数和检查线程数量
        if (m_dwProcessors <= 0)
        {
            SYSTEM_INFO stSysInfo;
            ::GetSystemInfo(&stSysInfo);
            m_dwProcessors = (stSysInfo.dwNumberOfProcessors <= 0) ? 1 : stSysInfo.dwNumberOfProcessors;
        }
        m_pThreadPool->dwCapacity = (nCapacity <= 0) ? m_dwProcessors + 1 : static_cast<DWORD>(nCapacity);
        // 创建线程
        m_pThreadPool->nPriority = nPriority;
        DWORD dwCreatedCount = 0;
        m_pThreadPool->tStackSize = static_cast<SIZE_T>(nStackSize);
        for (DWORD i = 0; i < m_pThreadPool->dwCapacity; i++)
        {
            HANDLE hThreadHandle = ::CreateThread(NULL, m_pThreadPool->tStackSize,
                                                  reinterpret_cast<LPTHREAD_START_ROUTINE>(ThreadPoolWorkerThread), m_pThreadPool, 0, NULL);
            if (hThreadHandle)
            {
                // 设置线程优先级
                if (m_pThreadPool->nPriority != THREAD_PRIORITY_NORMAL)
                {
                    ::SetThreadPriority(hThreadHandle, m_pThreadPool->nPriority);
                }
                // 因为之后不需要改变线程状态,所以立即销毁线程句柄
                ::CloseHandle(hThreadHandle);
                dwCreatedCount++;
            }
        }
        // 加上等待事件防止有部分线程句柄未被销毁
        while (::WaitForSingleObject(m_pThreadPool->hEvent[0], 50) == WAIT_OBJECT_0 && m_pThreadPool->dwThreadsCount < dwCreatedCount)
        {
            ProcessMessage(); // UI线程下自动处理事件,防止窗口卡死
        }
        ::CloseHandle(m_pThreadPool->hEvent[0]); // 关闭事件句柄
        ::InterlockedExchangePointer(&m_pThreadPool->hEvent[0], NULL);
        // 所有线程创建失败时退出
        if (m_pThreadPool->dwThreadsCount == 0)
        {
            CloseIOCP();
            return FALSE;
        }
        // 创建成功,状态设置为正常工作
        ::InterlockedExchange(&m_pThreadPool->dwState, StateNormal);
        return TRUE;
    }
    // 销毁
    BOOL DestroyThreadPool(BOOL bForceDestroy = TRUE, INT nWaitTime = -1)
    {
        if (!m_pThreadPool)
        {
            return TRUE;
        }
        // 如果当前为暂停状态,转为强制销毁
        if (m_pThreadPool->dwState == StateSuspend)
        {
            bForceDestroy = TRUE;
        }
        // 如果正在销毁返回假,但强制销毁为真时无视正在销毁状态,强制销毁
        if (m_pThreadPool->dwState == StateClosing && bForceDestroy == FALSE)
        {
            return FALSE;
        }
        // 状态设置为正在销毁
        ::InterlockedExchange(&m_pThreadPool->dwState, StateClosing);
        DWORD dwThreadsCount = m_pThreadPool->dwThreadsCount;
        // 将信号灯设置为放行,用于通知线程需要退出
        if (m_hSemaphore)
        {
            ::ReleaseSemaphore(m_hSemaphore, static_cast<LONG>(dwThreadsCount), NULL);
        }
        if (m_pThreadPool->hEvent[3]) // 仅线程池EX有效
        {
            ::SetEvent(m_pThreadPool->hEvent[3]);
        }
        // 通知IOCP结束所有线程
        for (DWORD i = 0; i < dwThreadsCount; i++)
        {
            // 如果等待事件的句柄有效,发信号结束等待
            if (m_pThreadPool->hEvent[2])
            {
                ::SetEvent(m_pThreadPool->hEvent[2]);
            }
            ::PostQueuedCompletionStatus(m_pThreadPool->hCompletionPort, PIV_THREADPOOL_EXIT, 0, NULL);
        }
        // 非强制模式时等待队列的任务执行完成
        if (!bForceDestroy)
        {
            nWaitTime = MAX(nWaitTime, -1);
            // 创建事件,等待线程退出的信号
            ::InterlockedExchangePointer(&m_pThreadPool->hEvent[1], ::CreateEventW(0, FALSE, FALSE, NULL));
            DOUBLE dStartTime = ::GetCurrentDateTime();
            // 循环到线程都退出或超时
            while (m_pThreadPool->dwThreadsCount > 0)
            {
                // 超时退出循环,将强制销毁线程池
                if (nWaitTime > 0 && static_cast<INT>(::GetTimeDiff(::GetCurrentDateTime(), dStartTime, 8)) >= nWaitTime)
                {
                    break;
                }
                // 防止重复销毁时出错
                if (!m_pThreadPool)
                {
                    break;
                }
                ::WaitForSingleObject(m_pThreadPool->hEvent[1], 50);
                ProcessMessage(); // UI线程下自动处理事件,防止窗口卡死
                // 若线程池已被其他方法销毁,退出循环,防止重复销毁时出错
                if (!m_pThreadPool)
                {
                    break;
                }
            }
        }
        // 因为有可能重复销毁线程池,需要判断线程池指针是否有效
        if (m_pThreadPool)
        {
            // 如果还有队列任务,继续销毁
            if (m_pThreadPool->dwQueueTask > 0)
            {
                // 现有的线程可能都处于耗时操作,临时创建一条线程来清空队列任务
                HANDLE hThreadHandle = ::CreateThread(NULL, m_pThreadPool->tStackSize,
                                                      reinterpret_cast<LPTHREAD_START_ROUTINE>(ThreadPoolClearTask), m_pThreadPool, 0, NULL);
                // 等待清空任务的线程结束
                if (hThreadHandle)
                {
                    while (::WaitForSingleObject(hThreadHandle, 50) != WAIT_OBJECT_0)
                    {
                        ProcessMessage();
                    }
                    // 销毁线程句柄
                    ::CloseHandle(hThreadHandle);
                }
            }
            // 仅线程池EX有效,释放退出扩展函数的事件句柄,并设置为空指针
            if (m_pThreadPool->hEvent[3])
            {
                ::CloseHandle(m_pThreadPool->hEvent[3]);
                ::InterlockedExchangePointer(&m_pThreadPool->hEvent[3], NULL);
            }
            // 释放暂停事件的句柄,并设置为空指针
            if (m_pThreadPool->hEvent[2])
            {
                ::CloseHandle(m_pThreadPool->hEvent[2]);
                ::InterlockedExchangePointer(&m_pThreadPool->hEvent[2], NULL);
            }
            // 释放销毁事件的句柄,并设置为空指针
            if (m_pThreadPool->hEvent[1])
            {
                ::CloseHandle(m_pThreadPool->hEvent[1]);
                ::InterlockedExchangePointer(&m_pThreadPool->hEvent[1], NULL);
            }
            // 关闭IOCP,使所有线程的生命周期结束(除了部分非常耗时或死循环的回调函数)
            CloseIOCP();
        }
        return TRUE;
    }
    // 暂停
    BOOL SuspendThreadPool()
    {
        if (!m_pThreadPool)
        {
            return FALSE;
        }
        if (::InterlockedCompareExchange(&m_pThreadPool->dwState, StateSuspend, StateNormal) != StateNormal)
        {
            return FALSE;
        }
        // 创建暂停事件句柄
        ::InterlockedCompareExchangePointer(&m_pThreadPool->hEvent[2], ::CreateEventW(0, FALSE, FALSE, NULL), NULL);
        return ::ResetEvent(m_pThreadPool->hEvent[2]);
    }
    // 继续
    BOOL ResumeThreadPool()
    {
        if (!m_pThreadPool)
        {
            return FALSE;
        }
        if (::InterlockedCompareExchange(&m_pThreadPool->dwState, StateNormal, StateSuspend) != StateSuspend)
        {
            return FALSE;
        }
        // 发信号让等待事件结束
        for (DWORD i = 0; i < m_pThreadPool->dwThreadsCount; i++)
        {
            ::SetEvent(m_pThreadPool->hEvent[2]);
        }
        // 释放暂停事件句柄并设置为空指针
        if (m_pThreadPool->hEvent[2])
        {
            ::CloseHandle(m_pThreadPool->hEvent[2]);
            ::InterlockedExchangePointer(&m_pThreadPool->hEvent[2], NULL);
        }
        return TRUE;
    }
    // 投递任务
    BOOL PostParamTask(void *lpThis, ULONG_PTR lpThreadProc, INT nUserMark = 0, INT64 lUserParam = 0, INT_P lpUserData = 0)
    {
        if (!m_pThreadPool)
        {
            return FALSE;
        }
        if (m_pThreadPool->dwState < StateNormal)
        {
            return FALSE;
        }
        // 设置回调参数
        THREADPOOLPARAM *lpParam = new THREADPOOLPARAM{lpThis, nUserMark, lUserParam, lpUserData};
        // 原子锁递增队列任务数
        ::InterlockedIncrement(&m_pThreadPool->dwQueueTask);
        // 投递任务到IOCP
        return ::PostQueuedCompletionStatus(m_pThreadPool->hCompletionPort, NULL,
                                            lpThreadProc, reinterpret_cast<LPOVERLAPPED>(lpParam));
    }
    // 投递对象
    BOOL PostObjectTask(void *lpThis, ULONG_PTR lpThreadProc, INT nUserMark = 0, CVolObject &lpUserObj = CVolObject(), INT_P lpUserData = 0)
    {
        if (!m_pThreadPool)
        {
            return FALSE;
        }
        if (m_pThreadPool->dwState < StateNormal)
        {
            return FALSE;
        }
        // 设置回调参数
        THREADPOOLOBJECT *lpParam = new THREADPOOLOBJECT{53181552, lpThis, nUserMark, lpUserObj.MakeCloneObject(), lpUserData};
        // 原子锁递增队列任务数
        ::InterlockedIncrement(&m_pThreadPool->dwQueueTask);
        // 投递任务到IOCP
        return ::PostQueuedCompletionStatus(m_pThreadPool->hCompletionPort, NULL,
                                            lpThreadProc, reinterpret_cast<LPOVERLAPPED>(lpParam));
    }
    // 清空任务
    BOOL ClearTask()
    {
        if (!m_pThreadPool)
        {
            return FALSE;
        }
        if (::InterlockedCompareExchange(&m_pThreadPool->dwState, StateClear, StateNormal) != StateNormal)
        {
            return FALSE;
        }
        return TRUE;
    }
    // 调整线程池
    BOOL SetThreadPoolCapacity(INT nNewThreadNum)
    {
        if (!m_pThreadPool)
        {
            return FALSE;
        }
        // 非正常状态时返回假
        if (m_pThreadPool->dwState < StateNormal)
        {
            return FALSE;
        }
        DWORD dwNewThreadNum = (nNewThreadNum <= 0) ? m_dwProcessors + 1 : (DWORD)nNewThreadNum;
        // 新旧线程数量一样时直接返回真
        if (m_pThreadPool->dwThreadsCount == dwNewThreadNum)
        {
            return TRUE;
        }
        // 工作状态改为正在调整线程池,防止重复调整线程池
        if (::InterlockedExchange(&m_pThreadPool->dwState, StateAdjust) == StateAdjust)
        {
            return FALSE;
        }
        ::InterlockedExchange(&m_pThreadPool->dwCapacity, dwNewThreadNum);
        if (m_pThreadPool->dwThreadsCount > dwNewThreadNum) // 新线程数量比原来小
        {
            DWORD dwReduce = m_pThreadPool->dwThreadsCount - dwNewThreadNum;
            // 通知退出指定数量的线程,注意这要等之前投递的任务全部完成,才会执行
            for (DWORD i = 0; i < dwReduce; i++)
            {
                ::PostQueuedCompletionStatus(m_pThreadPool->hCompletionPort, PIV_THREADPOOL_EXIT, 0, NULL);
            }
            return TRUE;
        }
        else if (m_pThreadPool->dwThreadsCount < dwNewThreadNum) // 新线程数量比原来大
        {
            DWORD dwIncrease = dwNewThreadNum - m_pThreadPool->dwThreadsCount;
            for (DWORD i = 0; i < dwIncrease; i++)
            {
                HANDLE hThreadHandle = ::CreateThread(NULL, m_pThreadPool->tStackSize,
                                                      reinterpret_cast<LPTHREAD_START_ROUTINE>(ThreadPoolWorkerThread), m_pThreadPool, 0, NULL);
                if (hThreadHandle)
                {
                    // 设置线程优先级
                    if (m_pThreadPool->nPriority != THREAD_PRIORITY_NORMAL)
                    {
                        ::SetThreadPriority(hThreadHandle, m_pThreadPool->nPriority);
                    }
                    // 因为之后不需要改变线程状态,所以立即销毁线程句柄
                    ::CloseHandle(hThreadHandle);
                }
                ProcessMessage(); // UI线程下自动处理事件,防止窗口卡死
            }
        }
        // 状态设置为正常工作
        ::InterlockedExchange(&m_pThreadPool->dwState, StateNormal);
        return TRUE;
    }
    // CPU核心数
    const INT GetProcessorsCount()
    {
        if (m_dwProcessors <= 0)
        {
            SYSTEM_INFO stSysInfo;
            ::GetSystemInfo(&stSysInfo);
            m_dwProcessors = stSysInfo.dwNumberOfProcessors;
        }
        return (m_dwProcessors <= 0) ? 1 : m_dwProcessors;
    }
    // 现行线程数
    const INT GetThreadsCount() const
    {
        return (!m_pThreadPool) ? 0 : static_cast<INT>(m_pThreadPool->dwThreadsCount);
    }
    // 线程池容量
    const INT GetThreadsCapacity() const
    {
        return (!m_pThreadPool) ? 0 : static_cast<INT>(m_pThreadPool->dwCapacity);
    }
    // 执行任务数
    const INT GetWorkerTaskCount() const
    {
        return (!m_pThreadPool) ? 0 : static_cast<INT>(m_pThreadPool->dwWorkerTask);
    }
    // 队列任务数
    const INT64 GetQueueTaskCount() const
    {
        return (!m_pThreadPool) ? 0 : static_cast<INT64>(static_cast<UINT64>(m_pThreadPool->dwQueueTask));
    }
    // 已完成任务数
    const INT64 GetComplateTaskCount() const
    {
        return (!m_pThreadPool) ? 0 : static_cast<INT64>(static_cast<UINT64>(m_pThreadPool->dwComplateTask));
    }
    // 空闲线程数
    const INT GetFreeThreadsCount() const
    {
        return (!m_pThreadPool) ? 0 : static_cast<INT>(m_pThreadPool->dwThreadsCount - m_pThreadPool->dwWorkerTask);
    }
    // 工作状态
    const INT GetThreadPoolState() const
    {
        return (!m_pThreadPool) ? 0 : static_cast<INT>(m_pThreadPool->dwState);
    }
    // 是否空闲
    BOOL ThreadPoolIsFree() const
    {
        return (!m_pThreadPool) ? TRUE : (m_pThreadPool->dwQueueTask == 0 && m_pThreadPool->dwWorkerTask == 0);
    }
    // 是否需要退出
    BOOL IsNeedExit(INT nMillseconds = 0)
    {
        if (m_hSemaphore)
        {
            return (::WaitForSingleObject(m_hSemaphore, static_cast<DWORD>(nMillseconds)) == WAIT_OBJECT_0);
        }
        return FALSE;
    }

protected:
    // (内部)工作线程
    static INT CALLBACK ThreadPoolWorkerThread(LPVOID lpThreadPool)
    {
        THREADPOOLINFO *pThreadPool = reinterpret_cast<THREADPOOLINFO *>(lpThreadPool);
        ::InterlockedIncrement(&pThreadPool->dwThreadsCount);
        // 创建线程时发信号
        if (pThreadPool->hEvent[0])
        {
            ::SetEvent(pThreadPool->hEvent[0]);
        }
        WORKERPROC WorkerProc = nullptr; // 回调函数指针
        DWORD dwParam = 0;               // 线程池参数
        LPVOID lpParam = nullptr;        // 回调函数参数
        // 无限循环获取完成端口的状态(接收线程任务)
        while (::GetQueuedCompletionStatus(pThreadPool->hCompletionPort, &dwParam,
                                           reinterpret_cast<PULONG_PTR>(&WorkerProc), reinterpret_cast<LPOVERLAPPED *>(&lpParam), INFINITE))
        {
            // 收到结束线程信号
            if (dwParam == PIV_THREADPOOL_EXIT)
            {
                // 若调整线程池已达到目标线程数,状态设置为正在工作
                if (pThreadPool->dwState == StateAdjust && pThreadPool->dwThreadsCount == pThreadPool->dwCapacity + 1)
                    ::InterlockedExchange(&pThreadPool->dwState, StateNormal);
                break; // 跳出死循环,之后便结束该线程
            }
            switch (pThreadPool->dwState) // 判断线程池状态
            {
            case StateClosing: // 正在销毁线程池时发信号
            {
                if (pThreadPool->hEvent[1])
                    ::SetEvent(pThreadPool->hEvent[1]);
                break;
            }
            case StateSuspend: // 线程处于暂停状态
            {
                if (pThreadPool->hEvent[2])
                    ::WaitForSingleObject(pThreadPool->hEvent[2], INFINITE);
                break;
            }
            case StateClear: // 清空队列任务
            {
                if (lpParam)
                {
                    // 如果为"投递对象"的参数,释放用户对象副本
                    if (*(INT *)lpParam == 53181552)
                    {
                        ((THREADPOOLOBJECT *)lpParam)->lpUserObj->Destroy();
                    }
                    delete lpParam;    // 释放参数占用的内容
                    lpParam = nullptr; // 将参数指针置空
                }
                // 如果队列任务快清空完了
                if (pThreadPool->dwQueueTask <= 1)
                {
                    ::InterlockedExchange(&pThreadPool->dwState, StateNormal);
                }
                break;
            }
            }
            ::InterlockedDecrement(&pThreadPool->dwQueueTask); // 原子锁递减队列任务数
            if (lpParam)
            {
                ::InterlockedIncrement(&pThreadPool->dwWorkerTask);   // 原子锁递增执行任务数
                WorkerProc(lpParam);                                  // 执行任务
                ::InterlockedIncrement(&pThreadPool->dwComplateTask); // 原子锁递增已完成任务数
                ::InterlockedDecrement(&pThreadPool->dwWorkerTask);   // 原子锁递减执行任务数
            }
        }
        ::InterlockedDecrement(&pThreadPool->dwThreadsCount); // 原子锁递减线程数量
        return 0;
    }
    // (内部)清空任务线程
    static INT CALLBACK ThreadPoolClearTask(LPVOID lpThreadPool)
    {
        THREADPOOLINFO *pThreadPool = reinterpret_cast<THREADPOOLINFO *>(lpThreadPool);
        DWORD dwParam = 0;
        ULONG_PTR CompletionKey = 0;
        LPVOID lpParam = nullptr;
        // 无限循环获取完成端口的状态(接收线程任务)
        while (::GetQueuedCompletionStatus(pThreadPool->hCompletionPort, &dwParam,
                                           &CompletionKey, reinterpret_cast<LPOVERLAPPED *>(&lpParam), 100))
        {
            if (lpParam)
            {
                // 如果为"投递对象"的参数,释放用户对象副本
                if (*(INT *)lpParam == 53181552)
                {
                    ((THREADPOOLOBJECT *)lpParam)->lpUserObj->Destroy();
                }
                delete lpParam; // 释放参数占用的内容
            }
        }
        return 0;
    }
    // (内部)UI线程下自动处理事件,防止窗口卡死
    void ProcessMessage()
    {
        MSG msg;
        while (::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE) && msg.message != WM_QUIT)
        {
            if (::GetMessage(&msg, NULL, 0, 0) <= 0)
                break;
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
    // (内部)重置信号灯
    void ReSetSemaphore()
    {
        if (m_hSemaphore)
        {
            while (::WaitForSingleObject(m_hSemaphore, 0) == WAIT_OBJECT_0)
                ;
        }
    }
    // (内部)关闭IOCP和释放线程池参数
    void CloseIOCP()
    {
        ::CloseHandle(m_pThreadPool->hCompletionPort);
        ::InterlockedExchange(&m_pThreadPool->dwState, StateClosed);
        delete m_pThreadPool;
        m_pThreadPool = nullptr;
    }
}; // PivThreadPool

/**************************************************************************\
 * 线程池类EX PivThreadPoolEx                                             *
 * 带动态调整线程池容量,功能强、效率快、高稳定、无内存和句柄泄漏的线程池. *
 * 线程的回调函数在接收事件中编写,不需要使用静态方法,传参和操作UI更简单.  *
 * 可在UI线程中执行耗时操作不卡界面.                                      *
 * 完全使用CPP源码和火山语法实现,除了视窗基本类,不依赖其他类库和DLL.      *
\**************************************************************************/

class PivThreadPoolEx : public PivThreadPool
{
public:
    PivThreadPoolEx()
    {
    }
    ~PivThreadPoolEx()
    {
    }
    // 创建
    BOOL CreateThreadPool(INT nThreadsMin = 5, INT nThreadsMax = 20, INT nCycleMs = 5000, INT nStackSize = 0, INT nPriority = THREAD_PRIORITY_NORMAL)
    {
        // 线程池已创建则直接返回真,其他情况返回假
        if (m_pThreadPool)
        {
            return (m_pThreadPool->dwState >= StateNormal) ? TRUE : FALSE;
        }
        // 给线程池参数变量分配内存
        m_pThreadPool = new THREADPOOLINFO{0};
        if (!m_pThreadPool)
        {
            return FALSE;
        }
        // 工作状态设置为正在创建
        ::InterlockedExchange(&m_pThreadPool->dwState, StateStarting);
        // 创建事件,用于等待线程句柄被销毁
        ::InterlockedExchangePointer(&m_pThreadPool->hEvent[0], ::CreateEventW(0, FALSE, FALSE, NULL));
        // 创建事件失败时退出
        if (!m_pThreadPool->hEvent[0])
        {
            delete m_pThreadPool;
            m_pThreadPool = nullptr;
            return FALSE;
        }
        // 创建IOCP完成端口
        m_pThreadPool->hCompletionPort = ::CreateIoCompletionPort((HANDLE)-1, NULL, 0, 0);
        // 创建IOCP失败时退出
        if (!m_pThreadPool->hCompletionPort)
        {
            ::CloseHandle(m_pThreadPool->hEvent[0]);
            delete m_pThreadPool;
            m_pThreadPool = nullptr;
            return FALSE;
        }
        // 取CPU逻辑核心数和检查线程数量
        if (m_dwProcessors <= 0)
        {
            SYSTEM_INFO stSysInfo;
            ::GetSystemInfo(&stSysInfo);
            m_dwProcessors = (stSysInfo.dwNumberOfProcessors <= 0) ? 1 : stSysInfo.dwNumberOfProcessors;
        }
        m_pThreadPool->dwThreadsMin = MAX(static_cast<DWORD>(nThreadsMin), 1);
        m_pThreadPool->dwCapacity = (nThreadsMax <= 0) ? m_dwProcessors + 1 : static_cast<DWORD>(nThreadsMax);
        m_pThreadPool->dwCapacity = MAX(m_pThreadPool->dwCapacity, m_pThreadPool->dwThreadsMin + 2);
        m_pThreadPool->dwCycleMs = (nCycleMs < 100) ? 100 : static_cast<DWORD>(nCycleMs);
        // 创建线程
        m_pThreadPool->nPriority = nPriority;
        DWORD dwCreatedCount = 0;
        m_pThreadPool->tStackSize = static_cast<SIZE_T>(nStackSize);
        for (DWORD i = 0; i < m_pThreadPool->dwThreadsMin; i++)
        {
            HANDLE hThreadHandle = ::CreateThread(NULL, m_pThreadPool->tStackSize,
                                                  reinterpret_cast<LPTHREAD_START_ROUTINE>(ThreadPoolWorkerThread), m_pThreadPool, 0, NULL);
            if (hThreadHandle)
            {
                // 设置线程优先级
                if (m_pThreadPool->nPriority != THREAD_PRIORITY_NORMAL)
                {
                    ::SetThreadPriority(hThreadHandle, m_pThreadPool->nPriority);
                }
                // 因为之后不需要改变线程状态,所以立即销毁线程句柄
                ::CloseHandle(hThreadHandle);
                dwCreatedCount++;
            }
        }
        // 加上等待事件防止有部分线程句柄未被销毁
        while (::WaitForSingleObject(m_pThreadPool->hEvent[0], 50) == WAIT_OBJECT_0 && m_pThreadPool->dwThreadsCount < dwCreatedCount)
        {
            ProcessMessage(); // UI线程下自动处理事件,防止窗口卡死
        }
        // 关闭事件句柄
        ::CloseHandle(m_pThreadPool->hEvent[0]);
        ::InterlockedExchangePointer(&m_pThreadPool->hEvent[0], NULL);
        // 创建扩展任务
        m_pThreadPool->hEvent[3] = ::CreateEventW(0, FALSE, FALSE, NULL);
        // 创建扩展任务事件失败时退出
        if (!m_pThreadPool->hEvent[3])
        {
            CloseIOCP();
            return FALSE;
        }
        ::ResetEvent(m_pThreadPool->hEvent[3]);
        HANDLE hExtend = ::CreateThread(NULL, m_pThreadPool->tStackSize,
                                        reinterpret_cast<LPTHREAD_START_ROUTINE>(ThreadPoolExtendProc), m_pThreadPool, 0, NULL);
        if (!hExtend)
        {
            CloseIOCP();
            return FALSE;
        }
        ::CloseHandle(hExtend);
        // 所有线程创建失败时退出
        if (m_pThreadPool->dwThreadsCount == 0)
        {
            CloseIOCP();
            return FALSE;
        }
        // 创建成功,状态设置为正常工作
        ::InterlockedExchange(&m_pThreadPool->dwState, StateNormal);
        return TRUE;
    }
    // 最小线程数
    const INT GetThreadsMin() const
    {
        return (!m_pThreadPool) ? 0 : (INT)m_pThreadPool->dwThreadsMin;
    }
    // 获取动态线程周期
    const INT GetCycleMs() const
    {
        return (!m_pThreadPool) ? 0 : (INT)m_pThreadPool->dwCycleMs;
    }
    // 设置动态线程周期
    void SetCycleMs(INT nCycleMs)
    {
        if (m_pThreadPool)
        {
            m_pThreadPool->dwCycleMs = (nCycleMs < 100) ? 100 : static_cast<DWORD>(nCycleMs);
        }
    }

private:
    // 动态控制函数
    static VOID ThreadPoolExtendProc(LPVOID lpThreadPool)
    {
        THREADPOOLINFO *pThreadPool = reinterpret_cast<THREADPOOLINFO *>(lpThreadPool);
        while (::WaitForSingleObject(pThreadPool->hEvent[3], pThreadPool->dwCycleMs) != WAIT_OBJECT_0)
        {
            if (pThreadPool->dwState == StateNormal)
            {
                if (pThreadPool->dwWorkerTask >= pThreadPool->dwThreadsMin)
                {
                    DWORD dwIncrease = pThreadPool->dwCapacity - pThreadPool->dwThreadsCount;
                    for (DWORD i = 0; i < dwIncrease; i++)
                    {
                        HANDLE hThreadHandle = ::CreateThread(NULL, pThreadPool->tStackSize,
                                                              reinterpret_cast<LPTHREAD_START_ROUTINE>(ThreadPoolWorkerThread), pThreadPool, 0, NULL);
                        if (hThreadHandle)
                        {
                            // 设置线程优先级
                            if (pThreadPool->nPriority != THREAD_PRIORITY_NORMAL)
                            {
                                ::SetThreadPriority(hThreadHandle, pThreadPool->nPriority);
                            }
                            // 因为之后不需要改变线程状态,所以立即销毁线程句柄
                            ::CloseHandle(hThreadHandle);
                        }
                    }
                }
            }
            else
            {
                if (pThreadPool->dwThreadsCount > pThreadPool->dwThreadsMin)
                {
                    ::InterlockedExchange(&pThreadPool->dwState, StateAdjust);
                    DWORD dwReduce = pThreadPool->dwThreadsCount - pThreadPool->dwThreadsMin;
                    // 通知退出指定数量的线程,注意这要等之前投递的任务全部完成,才会执行
                    for (DWORD i = 0; i < dwReduce; i++)
                    {
                        ::PostQueuedCompletionStatus(pThreadPool->hCompletionPort, PIV_THREADPOOL_EXIT, 0, NULL);
                    }
                }
            }
        }
    }
}; // PivThreadPoolEx

/* 线程回调函数(写在火山的封装类中)
private:
    static INT CALLBACK ThreadPoolWorkerProc (INT_P lpParam)
    {
        PivThreadPool::THREADPOOLPARAM *lpThreadParam = (PivThreadPool::THREADPOOLPARAM *)lpParam;
        static_cast<@sn<current_class>*>(lpThreadParam->lpThis)->@<任务回调> (
                                         lpThreadParam->nUserMark, lpThreadParam->lUserParam, lpThreadParam->lpUserData);
        delete lpThreadParam;
        return 0;
    }
    static INT CALLBACK ThreadPoolObjWorkerProc (INT_P lpParam)
    {
        PivThreadPool::THREADPOOLOBJECT *lpThreadParam = (PivThreadPool::THREADPOOLOBJECT *)lpParam;
        static_cast<@sn<current_class>*>(lpThreadParam->lpThis)->@<对象任务回调> (
                                         lpThreadParam->nUserMark, *lpThreadParam->lpUserObj, lpThreadParam->lpUserData);
        lpThreadParam->lpUserObj->Destroy ();
        delete lpThreadParam;
        return 0;
    }
*/

#endif // _PIV_THREAD_POOL_HPP
