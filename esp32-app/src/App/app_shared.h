
#pragma once

/**
 * @brief App shared object
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2020-11-18
 */

#include "rtos_common.h"
#include "Clock/system_time.h" // For DateTime structure
#include "i18n.h"

#define AppSh  AppShared::instance()
#define tr(msg_id)     AppSh.getAppTranslated(msg_id)

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

    /**
     * @brief Contains current system time, used (updated) inly in Clock task
     */
    void setAppDt(const DateTime& dt);
    const DateTime& getAppDt();

    void setAppLang(i18n_lang lang);
    const char * getAppTranslated(i18n_msg_id id);

private:

    // I18N map
    I18N m_i18n;

    // Global display message queue
    QueueHandle_t m_displayMsgQ;
    // Global display message queue buffer
    char displayMsgBufer[APP_DISPLAY_MSG_BUF_SIZE];
    // Display message mutex
    rtos::Mutex m_displayMsgMtx;

    // Global time queue
    QueueHandle_t m_timeDataQ;
    // Global time data mutex
    rtos::Mutex m_timeDataMtx;

    // App system time
    DateTime m_appDt;
    // App system time mutex
    rtos::Mutex m_appDtMtx;
};