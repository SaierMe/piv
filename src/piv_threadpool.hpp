#ifndef _PIV_STD_THREAD_POOL_HPP
#define _PIV_STD_THREAD_POOL_HPP

#include "detail/piv_base.hpp"
#include <future>

class PivThreadPoolPro
{
protected:
    enum ThreadPoolState // 线程状态
    {
        ThreadPoolState_Closed = 0,   // 线程池已被关闭
        ThreadPoolState_Closing = 1,  // 正在关闭线程池
        ThreadPoolState_Starting = 2, // 正在创建线程池
        ThreadPoolState_Normal = 3,   // 线程池正常工作
        ThreadPoolState_Adjust = 4,   // 调整线程池容量
        ThreadPoolState_Suspend = 5,  // 线程池暂停
        ThreadPoolState_Clear = 6     // 正在清空任务
    };
    struct THREADPOOL_INFO // 线程池信息
    {
        HANDLE hCompletionPort = NULL;                               // IOCP完成端口
        HANDLE hResumeSemaphore = NULL;                              // 暂停继续的信号灯
        HANDLE hExitSemaphore = NULL;                                // 是否需要退出信号灯
        std::mutex lock;                                             // 互斥锁
        std::atomic<ThreadPoolState> state = ThreadPoolState_Closed; // 工作状态(ThreadPoolState)
        std::atomic<int32_t> ThreadsCount = 0;                       // 现行线程数
        std::atomic<int32_t> Capacity = 0;                           // 线程池容量
        std::atomic<int32_t> WorkerTask = 0;                         // 执行任务数
        std::atomic<int64_t> QueueTask = 0;                          // 队列任务数量
        std::atomic<int64_t> ComplateTask = 0;                       // 已完成任务数
        THREADPOOL_INFO()
        {
            hCompletionPort = ::CreateIoCompletionPort((HANDLE)-1, NULL, 0, 0);
            hResumeSemaphore = ::CreateSemaphoreW(NULL, 0, LONG_MAX, NULL);
            hExitSemaphore = ::CreateSemaphoreW(NULL, 0, LONG_MAX, NULL);
        }
        ~THREADPOOL_INFO()
        {
            if (hResumeSemaphore)
                ::CloseHandle(hResumeSemaphore);
            if (hExitSemaphore)
                ::CloseHandle(hExitSemaphore);
            if (hCompletionPort)
                ::CloseHandle(hCompletionPort);
        }
    };
    std::shared_ptr<THREADPOOL_INFO> m_ThreadPool{nullptr};
    int32_t m_Processors = 0; // CPU逻辑核心数

public:
    PivThreadPoolPro()
    {
    }
    ~PivThreadPoolPro()
    {
        // 销毁线程池
        DestroyThreadPool(FALSE, 2000);
    }

    /**
     * @brief 创建
     * @param Capacity 线程数量
     * @return
     */
    virtual BOOL CreateThreadPool(int32_t Capacity = 0)
    {
        // 线程池已创建则直接返回真,其他情况返回假
        if (m_ThreadPool)
            return (m_ThreadPool->state >= ThreadPoolState_Normal) ? TRUE : FALSE;
        m_ThreadPool.reset(new THREADPOOL_INFO());
        if (!m_ThreadPool)
            return FALSE;
        if (!m_ThreadPool->hCompletionPort || !m_ThreadPool->hResumeSemaphore || !m_ThreadPool->hExitSemaphore)
        {
            m_ThreadPool.reset();
            return FALSE;
        }
        m_ThreadPool->state = ThreadPoolState_Starting;
        m_ThreadPool->Capacity = (Capacity <= 0) ? (GetProcessorsCount() + 1) : Capacity;
        // 创建线程
        try
        {
            for (int32_t i = 0; i < m_ThreadPool->Capacity; i++)
            {
                std::thread(&ThreadPoolWorkerThread, m_ThreadPool).detach();
            }
        }
        catch (const std::system_error &e)
        {
            (void)e;
            m_ThreadPool.reset();
            return FALSE;
        }
        // 创建成功,状态设置为正常工作
        m_ThreadPool->state = ThreadPoolState_Normal;
        return TRUE;
    }

    /**
     * @brief 销毁
     * @param ForceDestroy 是否强制销毁
     * @param WaitTime 最长等待时间
     * @return
     */
    BOOL DestroyThreadPool(BOOL ForceDestroy = TRUE, int32_t WaitTime = -1)
    {
        if (!m_ThreadPool)
            return TRUE;
        // 如果当前为暂停状态,转为强制销毁
        ForceDestroy = (m_ThreadPool->state == ThreadPoolState_Suspend) ? TRUE : ForceDestroy;
        // 如果正在销毁返回假
        if (m_ThreadPool->state == ThreadPoolState_Closing)
            return FALSE;
        m_ThreadPool->state = ThreadPoolState_Closing;
        // 非强制模式时限时等待队列的任务执行完成
        if (!ForceDestroy)
        {
            WaitTime = (std::max)(WaitTime, -1);
            std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
            // 循环到线程都退出或超时
            while (m_ThreadPool->ThreadsCount > 0)
            {
                // 超时退出循环,将强制销毁线程池
                if (WaitTime >= 0)
                {
                    if (std::chrono::steady_clock::now() - startTime >= std::chrono::milliseconds(WaitTime))
                        break;
                }
                Sleep(50);
                ProcessMessage(); // UI线程下自动处理事件,防止窗口卡死
            }
        }
        m_ThreadPool->Capacity = 0;
        int32_t ThreadsCount = m_ThreadPool->ThreadsCount;
        ::ReleaseSemaphore(m_ThreadPool->hResumeSemaphore, static_cast<LONG>(ThreadsCount), NULL);
        ::ReleaseSemaphore(m_ThreadPool->hExitSemaphore, static_cast<LONG>(ThreadsCount), NULL);
        // 通知IOCP结束所有线程
        for (int32_t i = 0; i < ThreadsCount; i++)
        {
            ::PostQueuedCompletionStatus(m_ThreadPool->hCompletionPort, 10086, 0, NULL);
        }
        // 清空所有任务
        if (m_ThreadPool->QueueTask > 0)
        {
            std::thread(&ThreadPoolClearTask, m_ThreadPool).join();
        }
        m_ThreadPool.reset();
        return TRUE;
    }

    /**
     * @brief 暂停
     * @return
     */
    inline BOOL SuspendThreadPool()
    {
        if (!m_ThreadPool || m_ThreadPool->state != ThreadPoolState_Normal)
            return FALSE;
        ReSetSemaphore(m_ThreadPool->hResumeSemaphore);
        m_ThreadPool->state = ThreadPoolState_Suspend;
        return TRUE;
    }

    /**
     * @brief 继续
     * @return
     */
    inline BOOL ResumeThreadPool()
    {
        if (!m_ThreadPool || m_ThreadPool->state != ThreadPoolState_Suspend)
            return FALSE;
        // 发信号让等待事件结束
        ::ReleaseSemaphore(m_ThreadPool->hResumeSemaphore, static_cast<LONG>(m_ThreadPool->ThreadsCount), NULL);
        m_ThreadPool->state = ThreadPoolState_Normal;
        return TRUE;
    }

    /**
     * @brief 投递任务
     * @param fun 执行函数
     * @param ...args 参数列表
     * @return
     */
    template <typename Fun, typename... Args>
    inline BOOL PostTask(Fun &&fun, Args &&...args)
    {
        if (!m_ThreadPool || m_ThreadPool->state < ThreadPoolState_Normal)
            return FALSE;
        std::packaged_task<void()> *task = new std::packaged_task<void()>(std::bind(std::forward<Fun>(fun), std::forward<Args>(args)...));
        m_ThreadPool->QueueTask++;
        return ::PostQueuedCompletionStatus(m_ThreadPool->hCompletionPort, 1, 0, reinterpret_cast<LPOVERLAPPED>(task));
    }

    inline BOOL PostTask2(std::function<void()> &&fun)
    {
        if (!m_ThreadPool || m_ThreadPool->state < ThreadPoolState_Normal)
            return FALSE;
        std::packaged_task<void()> *task = new std::packaged_task<void()>(std::forward<std::function<void()>>(fun));
        m_ThreadPool->QueueTask++;
        return ::PostQueuedCompletionStatus(m_ThreadPool->hCompletionPort, 1, 0, reinterpret_cast<LPOVERLAPPED>(task));
    }

    inline BOOL PostTask2(const std::function<void()> &fun)
    {
        if (!m_ThreadPool || m_ThreadPool->state < ThreadPoolState_Normal)
            return FALSE;
        std::packaged_task<void()> *task = new std::packaged_task<void()>(fun);
        m_ThreadPool->QueueTask++;
        return ::PostQueuedCompletionStatus(m_ThreadPool->hCompletionPort, 1, 0, reinterpret_cast<LPOVERLAPPED>(task));
    }

    /**
     * @brief 清空任务
     * @return
     */
    inline BOOL ClearTask()
    {
        if (!m_ThreadPool)
            return FALSE;
        if (m_ThreadPool->QueueTask <= 0)
            return TRUE;
        if (m_ThreadPool->state == ThreadPoolState_Suspend)
        {
            std::thread(&ThreadPoolClearTask, m_ThreadPool).join();
            return TRUE;
        }
        if (m_ThreadPool->state != ThreadPoolState_Normal)
            return FALSE;
        m_ThreadPool->state = ThreadPoolState_Clear;
        return TRUE;
    }

    /**
     * @brief 调整线程池
     * @param nNewThreadNum
     * @return
     */
    BOOL SetThreadPoolCapacity(int32_t nNewThreadNum)
    {
        // 非正常状态时返回假
        if (!m_ThreadPool || m_ThreadPool->state < ThreadPoolState_Normal)
            return FALSE;
        nNewThreadNum = (nNewThreadNum <= 0) ? m_Processors + 1 : nNewThreadNum;
        // 新旧线程数量一样时直接返回真
        m_ThreadPool->Capacity = nNewThreadNum;
        if (m_ThreadPool->ThreadsCount == nNewThreadNum)
        {
            return TRUE;
        }
        if (m_ThreadPool->ThreadsCount < nNewThreadNum)
        {
            for (int32_t i = m_ThreadPool->ThreadsCount; i < nNewThreadNum; i++)
                std::thread(&ThreadPoolWorkerThread, m_ThreadPool).detach();
        }
        else
        {
            m_ThreadPool->state = ThreadPoolState_Adjust;
            for (int32_t i = m_ThreadPool->ThreadsCount - nNewThreadNum; i > 0; i--)
                ::PostQueuedCompletionStatus(m_ThreadPool->hCompletionPort, 0, 0, nullptr);
        }
        return TRUE;
    }

    /**
     * @brief CPU核心数
     * @return
     */
    inline int32_t GetProcessorsCount()
    {
        if (m_Processors <= 0)
        {
            SYSTEM_INFO stSysInfo;
            ::GetSystemInfo(&stSysInfo);
            m_Processors = static_cast<int32_t>(stSysInfo.dwNumberOfProcessors);
        }
        return (m_Processors <= 0) ? 1 : m_Processors;
    }

    /**
     * @brief 现行线程数
     * @return
     */
    inline int32_t GetThreadsCount() const
    {
        return (!m_ThreadPool) ? 0 : m_ThreadPool->ThreadsCount.load();
    }

    /**
     * @brief 线程池容量
     * @return
     */
    inline int32_t GetThreadsCapacity() const
    {
        return (!m_ThreadPool) ? 0 : m_ThreadPool->Capacity.load();
    }

    /**
     * @brief 执行任务数
     * @return
     */
    inline int32_t GetWorkerTaskCount() const
    {
        return (!m_ThreadPool) ? 0 : m_ThreadPool->WorkerTask.load();
    }

    /**
     * @brief 队列任务数
     * @return
     */
    inline int64_t GetQueueTaskCount() const
    {
        return (!m_ThreadPool) ? 0 : m_ThreadPool->QueueTask.load();
    }

    /**
     * @brief 已完成任务数
     * @return
     */
    inline int64_t GetComplateTaskCount() const
    {
        return (!m_ThreadPool) ? 0 : m_ThreadPool->ComplateTask.load();
    }

    /**
     * @brief 空闲线程数
     * @return
     */
    inline int32_t GetFreeThreadsCount() const
    {
        return (!m_ThreadPool) ? 0 : (m_ThreadPool->ThreadsCount.load() - m_ThreadPool->WorkerTask.load());
    }

    /**
     * @brief 工作状态
     * @return
     */
    inline int32_t GetThreadPoolState() const
    {
        return (!m_ThreadPool) ? 0 : static_cast<int32_t>(m_ThreadPool->state.load());
    }

    /**
     * @brief 是否空闲
     * @return
     */
    inline BOOL ThreadPoolIsFree() const
    {
        return (!m_ThreadPool) ? TRUE : (m_ThreadPool->QueueTask.load() == 0 && m_ThreadPool->WorkerTask.load() == 0);
    }

    /**
     * @brief 是否需要退出
     * @param nMillseconds 最大检查等待时间
     * @return
     */
    inline BOOL IsNeedExit(int32_t nMillseconds = 0)
    {
        if (m_ThreadPool)
            return (::WaitForSingleObject(m_ThreadPool->hExitSemaphore, static_cast<DWORD>(nMillseconds)) == WAIT_OBJECT_0);
        return TRUE;
    }

protected:
    /**
     * @brief (内部)工作线程
     * @param lpThreadPool 线程池参数
     * @return
     */
    static INT CALLBACK ThreadPoolWorkerThread(std::shared_ptr<THREADPOOL_INFO> &ThreadPool)
    {
        ThreadPool->ThreadsCount++;
        DWORD dwStatus = 0;
        ULONG_PTR CompletionKey = 0;
        std::packaged_task<void()> *task = nullptr; // 任务函数指针
        // 无限循环获取完成端口的状态(接收线程任务)
        while (::GetQueuedCompletionStatus(ThreadPool->hCompletionPort, &dwStatus, &CompletionKey, reinterpret_cast<LPOVERLAPPED *>(&task), INFINITE))
        {
            std::unique_ptr<std::packaged_task<void()>> WorkProc(task);
            if (dwStatus == 10086)
                break;
            else if (dwStatus == 1)
                ThreadPool->QueueTask--;
            switch (ThreadPool->state.load())
            {
            case ThreadPoolState_Suspend:
            {
                ::WaitForSingleObject(ThreadPool->hResumeSemaphore, INFINITE);
                break;
            }
            case ThreadPoolState_Clear:
            {
                WorkProc.reset(nullptr);
                if (ThreadPool->QueueTask <= 0)
                    ThreadPool->state = ThreadPoolState_Normal;
                break;
            }
            case ThreadPoolState_Adjust:
            {
                if (ThreadPool->ThreadsCount == ThreadPool->Capacity)
                    ThreadPool->state = ThreadPoolState_Normal;
                break;
            }
            }
            if (WorkProc != nullptr && WorkProc->valid())
            {
                ThreadPool->WorkerTask++; // 原子锁递增执行任务数
                (*WorkProc)(); // 执行任务
                ThreadPool->ComplateTask++; // 原子锁递增已完成任务数
                ThreadPool->WorkerTask--;   // 原子锁递减执行任务数
            }
            if (ThreadPool->ThreadsCount != ThreadPool->Capacity)
            {
                std::lock_guard<std::mutex> lock_g(ThreadPool->lock);
                if (ThreadPool->ThreadsCount > ThreadPool->Capacity)
                {
                    ThreadPool->ThreadsCount--;
                    return (0);
                }
                else
                {
                    std::thread(&ThreadPoolWorkerThread, ThreadPool).detach();
                }
            }
        }
        ThreadPool->ThreadsCount--; // 原子锁递减线程数量
        return 0;
    }

    /**
     * @brief (内部)清空任务线程
     * @param lpThreadPool 线程池参数
     * @return
     */
    static int32_t CALLBACK ThreadPoolClearTask(std::shared_ptr<THREADPOOL_INFO> &ThreadPool)
    {
        DWORD dwStatus = 0;
        ULONG_PTR CompletionKey = 0;
        std::packaged_task<void()> *task = nullptr;
        while (::GetQueuedCompletionStatus(ThreadPool->hCompletionPort, &dwStatus,
                                           &CompletionKey, reinterpret_cast<LPOVERLAPPED *>(&task), 100))
        {
            ThreadPool->QueueTask--; // 原子锁递减队列任务数
            if (task != nullptr)
            {
                delete task; // 释放参数占用的内容
                task = nullptr;
            }
            if (ThreadPool->QueueTask <= 0)
                break;
        }
        ThreadPool->state = ThreadPoolState_Normal;
        return 0;
    }

    /**
     * @brief (内部)UI线程下处理事件,防止窗口卡死
     */
    inline void ProcessMessage()
    {
        MSG msg;
        while (::PeekMessageW(&msg, NULL, NULL, NULL, PM_NOREMOVE) && msg.message != WM_QUIT)
        {
            if (::GetMessageW(&msg, NULL, 0, 0) <= 0)
                break;
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        }
    }

    /**
     * @brief (内部)重置信号灯
     * @param hSemaphore 信号灯句柄
     */
    inline static void ReSetSemaphore(HANDLE hSemaphore)
    {
        if (hSemaphore)
        {
            while (::WaitForSingleObject(hSemaphore, 0) == WAIT_OBJECT_0)
                ;
        }
    }
};

#endif // _PIV_STD_THREAD_POOL_HPP
