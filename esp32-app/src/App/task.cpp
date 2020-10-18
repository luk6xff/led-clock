#include "task.h"




//------------------------------------------------------------------------------
Task::Task(const String& name, size_t stackSize, UBaseType_t priority, BaseType_t coreId)
    : m_handle(nullptr)
    , m_name(name)
    , m_stackSize(stackSize)
    , m_priority(priority)
    , m_coreId(coreId)
{
    //start();
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
    if (m_handle)
    {
        vTaskDelete(m_handle);
    }
    m_handle = nullptr;
}

//------------------------------------------------------------------------------
void Task::start()
{
    //xTaskCreate(&Task::taskFuncAdapter, m_name.c_str(), m_stackSize, this, m_priority, &m_handle);
    xTaskCreatePinnedToCore(&Task::taskFuncAdapter, m_name.c_str(),
                            m_stackSize, this, m_priority, &m_handle, m_coreId);
}

//------------------------------------------------------------------------------
void Task::taskFuncAdapter(void *pvParameters)
{
    Task *task = static_cast<Task *>(pvParameters);
    task->run();
    task->kill();
}

//------------------------------------------------------------------------------
