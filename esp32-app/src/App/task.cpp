#include "task.h"




//------------------------------------------------------------------------------
Task::Task(const String& name, size_t stackSize, UBaseType_t priority)
    : m_handle(nullptr)
    , m_name(name)
    , m_stackSize(stackSize)
    , m_priority(priority)
{
    start();
}

//------------------------------------------------------------------------------
Task::~Task()
{
    kill();
}


//------------------------------------------------------------------------------
void Task::suspend()
{
    vTaskSuspend(m_handle);
}


//------------------------------------------------------------------------------
void Task::resume()
{
    vTaskResume(m_handle);
}


//------------------------------------------------------------------------------
void Task::restart()
{
    kill();
    vTaskDelay(1);
    start();
}

//------------------------------------------------------------------------------
TaskHandle_t& Task::getTaskHandle()
{
    return m_handle;
}

//------------------------------------------------------------------------------
const char* Task::getTaskName() const
{
    return pcTaskGetTaskName(m_handle);
}


//------------------------------------------------------------------------------
void Task::kill()
{
    vTaskDelete(m_handle);
    m_handle = nullptr;
}

//------------------------------------------------------------------------------
void Task::start()
{
    xTaskCreate(taskFuncAdapter, m_name.c_str(), m_stackSize, this, m_priority, &m_handle);
}

//------------------------------------------------------------------------------
void Task::taskFuncAdapter(void *pvParameters)
{
    Task *task = static_cast<Task *>(pvParameters);
    task->run();
    task->kill();
}

//------------------------------------------------------------------------------
