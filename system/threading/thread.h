// Copyright (c) 2011, The Toft Authors. All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2010-06-18

#ifndef TOFT_SYSTEM_THREADING_THREAD_H_INCLUDED
#define TOFT_SYSTEM_THREADING_THREAD_H_INCLUDED

#include "toft/base/functional.h"
#include "toft/base/uncopyable.h"
#include "toft/system/threading/thread_types.h"

namespace toft {

// Thread ��
// ���û�ֱ�Ӵ����̺߳���ʹ��
class Thread
{
    TOFT_DECLARE_UNCOPYABLE(Thread);
    class Impl;
public:
    // Ĭ�Ϲ�����̶߳���
    // ����� Initialize ����� Start
    Thread();

    explicit Thread(const std::function<void ()>& function);

    // TODO(phongchen): Thread �಻Ӧ�ñ��������࣬����ֻ��Ϊ����Ϻ����
    // Entry��������Ҫȥ����
    virtual ~Thread();

    // ��ʼ���̶߳���
    void Initialize(const std::function<void ()>& function);

    // ֻ���� Start ֮ǰ����
    void SetStackSize(size_t size);

    // ������� Start ��Ż���������
    void Start();
    bool TryStart();

    // �ȴ��������е��߳̽���
    // ֻ���߳��Ѿ������ˣ���û�� Detach������ Join
    // ����߳��Ѿ��������������� true
    bool Join();

    // ��ʵ���̺߳��̶߳�����룬Detach ֮�󣬲�����ͨ���κκ������ʵ�ʵ���߳�
    void Detach();

    void SendStopRequest();
    bool IsStopRequested() const;
    bool StopAndWaitForExit();

    // �����߳��Ƿ��ڴ��
    bool IsAlive() const;

    // �����Ƿ���Զ��̵߳� Join
    bool IsJoinable() const;

    // ���ϵͳ�����߳� handle�����������뺬�壬ƽ̨���
    ThreadHandleType GetHandle() const;

    // ����߳�ID������PID��������
    int GetId() const;

private:
    // Thread ����������� BaseThread �࣬BaseThread ���� override
    // Entry �����ķ�ʽʹ�ã����� Thread ����Ҫ֧�� Detach����˲��ٴ�
    // BaseThread ������Thread �౾����Ӧ����ֱ���Ժ���Ϊ������ʱ������ʹ�õġ�
    // ������������ﱨ��˵�����ô����ࡣ
    // ��������� BaseThread::Entry ǩ����һ����ʹ�ñ��������Լ�鵽���ִ���
    virtual void Entry() const {}

private:
    Impl* m_pimpl;
};

} // namespace toft

#endif // TOFT_SYSTEM_THREADING_THREAD_H_INCLUDED

