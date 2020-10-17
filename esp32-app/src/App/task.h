
#pragma once
/**
 * @brief Simple wrapper for FreeRtos task
 *
 * @author Lukasz Uszko - luk6xff
 * @date   15.10.2020
 */

#include <freertos/FreeRTOS.h>
#include <Arduino.h>

class Task
{

public:
    Task(const String& name, size_t stackSize = 4096, UBaseType_t priority = 12);
    virtual ~Task();

    void start();
    void suspend();
    void resume();

    void restart();

    TaskHandle_t& getTaskHandle();

    const char* getTaskName() const;

protected:
    void kill();
    virtual void run() = 0;

private:
    TaskHandle_t m_handle;
    String m_name;
    size_t m_stackSize;
    UBaseType_t m_priority;

    /**
     * @brief Adapter function that allows you to write a class
     *  specific run() function that interfaces with FreeRTOS.
     *  Look at the implementation of the constructors and this
     *  code to see how the interface between C and C++ is performed.
     */
    static void taskFuncAdapter(void *pvParameters);
};
