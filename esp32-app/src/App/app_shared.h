
#pragma once

/**
 * @brief App shared object
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2020-11-18
 */

#include "rtos_common.h"

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

private:
    QueueHandle_t m_displayMsgQ;
    char displayMsgBufer[APP_DISPLAY_MSG_BUF_SIZE];
};