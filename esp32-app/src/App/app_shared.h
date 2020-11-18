
#pragma once

/**
 * @brief App shared object
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2020-11-18
 */

#include "rtos_common.h"
#include "Clock/system_time.h" // For DateTime structure

#define AppSh  AppShared::instance()

#define APP_DISPLAY_MSG_BUF_SIZE 512

typedef struct
{
    char *msg;
    uint32_t msgLen;
} AppDisplayMsg;


class AppShared
{

public:
    AppShared();
    static AppShared& instance();

    bool putDisplayMsg(const char *msg, size_t msgLen);
    const QueueHandle_t& getDisplayQHandle();

    bool putDateTimeMsg(const DateTime& dt);
    const QueueHandle_t& getTimeQHandle();

private:
    // Global display message queue
    QueueHandle_t m_displayMsgQ;
    // Global display message queue buffer
    char displayMsgBufer[APP_DISPLAY_MSG_BUF_SIZE];
    // Display message mutex
    rtos::Mutex m_displayMsgMtx;

    // Global time queue
    QueueHandle_t m_timeDataQ;
    // Display data mutex
    rtos::Mutex m_timeDataMtx;
};