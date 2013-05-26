// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_SYSTEM_EVENT_DISPATCHER_EVENT_DISPATCHER_H
#define TOFT_SYSTEM_EVENT_DISPATCHER_EVENT_DISPATCHER_H
#pragma once

#include <stdint.h>
#include "thirdparty/libev/ev++.h"
#include "toft/base/functional.h"
#include "toft/base/uncopyable.h"

namespace toft {

enum EventMask {
    EventMask_Undefined = EV_UNDEF,
    EventMask_None      = EV_NONE,
    EventMask_Read      = EV_READ,
    EventMask_Write     = EV_WRITE,
    EventMask_Timer     = EV_TIMER,
    EventMask_Periodic  = EV_PERIODIC,
    EventMask_Signal    = EV_SIGNAL,
    EventMask_Child     = EV_CHILD,
    EventMask_Stat      = EV_STAT,
    EventMask_Idle      = EV_IDLE,
    EventMask_Check     = EV_CHECK,
    EventMask_Prepare   = EV_PREPARE,
    EventMask_Fork      = EV_FORK,
    EventMask_Async     = EV_ASYNC,
    EventMask_Embed     = EV_EMBED,
    EventMask_Error     = EV_ERROR
};

template <typename CppWatcherType, typename CWatcherType>
class EventWatcherBase;

// An EventDispatcher object represent an event dispatcher.
class EventDispatcher {
    TOFT_DECLARE_UNCOPYABLE(EventDispatcher);
    // Allow EventWatcherBase access m_loop
    template <typename CppWatcherType, typename CWatcherType>
    friend class EventWatcherBase;
public:
    EventDispatcher();
    ~EventDispatcher();
    void Run() { ev_run(m_loop); }
    void RunOnce() { ev_run(m_loop, EVRUN_ONCE); }
    void Break() { ev_break(m_loop); }
private:
    struct ev_loop* m_loop;
};

// Used as base template class of concrete EventWatcher class.
// CppWatcherType is the concrete EventWatcher
// CWatcherType is the c ev_* struct in libev.
template <typename CppWatcherType, typename CWatcherType>
class EventWatcherBase : private CWatcherType {
    TOFT_DECLARE_UNCOPYABLE(EventWatcherBase);
protected:
    typedef std::function<void(int event_mask)> CallbackType;
    EventWatcherBase(EventDispatcher* dispatcher, const CallbackType& callback)
        : m_dispatcher(dispatcher), m_callback(callback) {
        ev_init(this, Thunk);
    }
    ~EventWatcherBase() { static_cast<CppWatcherType*>(this)->Stop(); }

public:
    bool IsActive() const { return ev_is_active(this) != 0; }
    bool IsPending() const { return ev_is_pending(this) != 0; }
    EventDispatcher* Dispatcher() const {
        return m_dispatcher;
    }

protected:
    struct ev_loop* loop() {
        return m_dispatcher->m_loop;
    }
    CWatcherType* c_watcher() {
        return this;
    }
    const CWatcherType* c_watcher() const {
        return this;
    }

private:
    // Forward C callback to C++ callback.
    static void Thunk(struct ev_loop* loop, CWatcherType* w, int event_mask) {
        static_cast<EventWatcherBase*>(w)->m_callback(event_mask);
    }

protected:
    EventDispatcher* m_dispatcher;
    CallbackType m_callback;
};

// Watch realtime clock timeout event.
class TimerEventWatcher : public EventWatcherBase<TimerEventWatcher, ev_timer> {
public:
    TimerEventWatcher(EventDispatcher* dispatcher,
                      const CallbackType& callback,
                      int64_t after_ms, int64_t repeat_ms = 0)
        : EventWatcherBase(dispatcher, callback) {
        ev_timer_set(c_watcher(), after_ms / 1000.0, repeat_ms / 1000.0);
    }
    void Start() {
        ev_timer_start(loop(), c_watcher());
    }

    void Stop() {
        ev_timer_stop(loop(), c_watcher());
    }
};

// Watch signal event.
class SignalEventWatcher : public EventWatcherBase<SignalEventWatcher, ev_signal> {
public:
    SignalEventWatcher(EventDispatcher* dispatcher,
                       const CallbackType& callback,
                       int signo)
        : EventWatcherBase(dispatcher, callback) {
        ev_signal_set(c_watcher(), signo);
    }
    void Start() {
        ev_signal_start(loop(), c_watcher());
    }

    void Stop() {
        ev_signal_stop(loop(), c_watcher());
    }
};

// Watch fd io readable/writeable event.
class IoEventWatcher : public EventWatcherBase<IoEventWatcher, ev_io> {
public:
    IoEventWatcher(EventDispatcher* dispatcher, const CallbackType& callback)
        : EventWatcherBase(dispatcher, callback) {
    }
    IoEventWatcher(EventDispatcher* dispatcher, const CallbackType& callback,
                   int fd, int event_mask)
        : EventWatcherBase(dispatcher, callback) {
        ev_io_set(c_watcher(), fd, event_mask);
    }
    void Start() {
        ev_io_start(loop(), c_watcher());
    }

    void Stop() {
        ev_io_stop(loop(), c_watcher());
    }

    void Set(int fd, int events) {
        ev_io_set(c_watcher(), c_watcher()->fd, events);
    }

    void Set(int events) {
        struct ev_io* w = c_watcher();
        if (events == w->events)
            return;
        bool active = IsActive();
        if (active)
            Stop();
        ev_io_set(w, w->fd, events);
        if (active)
            Start();
    }
};

} // namespace toft

#endif // TOFT_SYSTEM_EVENT_DISPATCHER_EVENT_DISPATCHER_H
