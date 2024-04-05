/**
 * Copyright (c) 2024 COMWISE. All rights reserved.
 */
#ifndef __COMWISE__ROBOT_VIEWER__SINGLETON__H__
#define __COMWISE__ROBOT_VIEWER__SINGLETON__H__

#include <atomic>
#include <memory>
#include <mutex>

//! singleton
template <class T>
class singleton
{
  public:
    static T *instance() {
        // with atomic mutex DoubleCheckNull
        T *tmp = instance_.load(std::memory_order::memory_order_acquire);
        if (!tmp) {
            std::lock_guard<std::recursive_mutex> guard(lock_);
            tmp = instance_.load(std::memory_order::memory_order_relaxed);
            if (!tmp) {
                tmp = new T;
                associated_ = 0;
                instance_.store(tmp, std::memory_order::memory_order_release);
            }
        }
        return tmp;
    }

    static void associate(T *ptr) {
        // with atomic mutex DoubleCheckNull
        T *tmp = instance_.load(std::memory_order::memory_order_acquire);
        if (!tmp) {
            std::lock_guard<std::recursive_mutex> guard(lock_);
            tmp = instance_.load(std::memory_order::memory_order_relaxed);
            if (!tmp) {
                tmp = ptr;
                associated_ = 1;
                instance_.store(tmp, std::memory_order::memory_order_release);
            }
        }
    }

    static void release() {
        T *tmp = instance_.load(std::memory_order::memory_order_acquire);
        if (tmp && !associated_) {
            std::lock_guard<std::recursive_mutex> guard(lock_);
            tmp = instance_.load(std::memory_order::memory_order_relaxed);
            if (tmp && !associated_) {
                delete tmp;
                instance_.store(nullptr, std::memory_order::memory_order_release);
            }
        }
    }

  protected:
    singleton() { }
    ~singleton() { }

    singleton(const singleton &) = delete;
    singleton(singleton &&) = delete;
    singleton &operator=(const singleton &) = delete;
    singleton &operator=(singleton &&) = delete;

  private:
    static std::atomic<T *> instance_;
    static std::recursive_mutex lock_;
    static int associated_;
};

template <class T>
std::atomic<T *> singleton<T>::instance_ {nullptr};

template <class T>
std::recursive_mutex singleton<T>::lock_;

template <class T>
int singleton<T>::associated_ {0};

#endif  // __COMWISE__ROBOT_VIEWER__SINGLETON__H__
