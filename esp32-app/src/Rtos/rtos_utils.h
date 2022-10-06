#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

namespace rtos
{

/**
 * @brief Simple RAII lock_guard implementation
 */
template<typename T>
class LockGuard
{

public:
    LockGuard(T& handle)
        : m_handle(handle)
    {
        m_handle.lock();
    }

    ~LockGuard()
    {
        m_handle.unlock();
    }

    LockGuard(const LockGuard& other) = delete;
    LockGuard(LockGuard&& other) = delete;
    LockGuard& operator=(const LockGuard& other) = delete;

private:
    T& m_handle;
};


/**
 * @brief FreeRtos mutex wrapper
 */
class Mutex final
{

public:
    Mutex();
    ~Mutex();

    void lock();
    void unlock();
    void reset();

private:
    SemaphoreHandle_t m_handle;
};


} // rtos


// Globally used rtos objects
/**
 * @brief Mutex to prevent mutual access to I2C peripheral
 */
extern rtos::Mutex g_i2c0Mutex;