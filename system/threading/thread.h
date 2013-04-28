// Copyright (c) 2011, The Toft Authors. All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2010-06-18

#ifndef TOFT_SYSTEM_THREADING_THREAD_H_INCLUDED
#define TOFT_SYSTEM_THREADING_THREAD_H_INCLUDED

#include "toft/base/functional.h"
#include "toft/base/uncopyable.h"
#include "toft/system/threading/thread_types.h"

namespace toft {

// Thread 类
// 由用户直接传入线程函数使用
class Thread
{
    TOFT_DECLARE_UNCOPYABLE(Thread);
    class Impl;
public:
    // 默认构造的线程对象
    // 必须调 Initialize 后才能 Start
    Thread();

    explicit Thread(const std::function<void ()>& function);

    // TODO(phongchen): Thread 类不应该被用作基类，这里只是为了配合后面的
    // Entry，将来需要去掉。
    virtual ~Thread();

    // 初始化线程对象
    void Initialize(const std::function<void ()>& function);

    // 只能在 Start 之前调用
    void SetStackSize(size_t size);

    // 必须调用 Start 后才会真正启动
    void Start();
    bool TryStart();

    // 等待正在运行的线程结束
    // 只有线程已经运行了，且没有 Detach，才能 Join
    // 如果线程已经结束，立即返回 true
    bool Join();

    // 把实际线程和线程对象分离，Detach 之后，不能再通过任何函数访问到实际线程
    void Detach();

    void SendStopRequest();
    bool IsStopRequested() const;
    bool StopAndWaitForExit();

    // 返回线程是否还在存活
    bool IsAlive() const;

    // 返回是否可以对线程调 Join
    bool IsJoinable() const;

    // 获得系统级的线程 handle，具体类型与含义，平台相关
    ThreadHandleType GetHandle() const;

    // 获得线程ID（类似PID的整数）
    int GetId() const;

private:
    // Thread 类起初派生自 BaseThread 类，BaseThread 是以 override
    // Entry 函数的方式使用，但是 Thread 类需要支持 Detach，因此不再从
    // BaseThread 派生，Thread 类本来就应该是直接以函数为参数来时初化来使用的。
    // 如果你遇到这里报错，说明你用错了类。
    // 这里故意与 BaseThread::Entry 签名不一样，使得编译器可以检查到这种错误。
    virtual void Entry() const {}

private:
    Impl* m_pimpl;
};

} // namespace toft

#endif // TOFT_SYSTEM_THREADING_THREAD_H_INCLUDED

