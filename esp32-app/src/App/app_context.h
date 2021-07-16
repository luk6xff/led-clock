
#pragma once

/**
 * @brief App shared object
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2020-11-18
 */

#include "rtos_utils.h"
#include "Clock/system_time.h" // For DateTime structure
#include "i18n.h"
#include "app_status.h"

#define AppCtx      AppContext::instance()
#define tr(msg_id)  AppCtx.getAppTranslated(msg_id)

#define APP_DISPLAY_MSG_BUF_SIZE    512
#define APP_DISPLAY_MSG_QUEUE_SIZE  4
#define APP_TIME_MSG_QUEUE_SIZE     1


// App display Msg
typedef struct
{
    char *msg;
    uint32_t msgLen;
} AppDisplayMsg;


// App display Cmd
typedef enum
{
    APP_DISPLAY_CLEAR_CMD = 0,
    APP_DISPLAY_RESET_CMD = 1,
} AppDisplayCmd;



class AppContext
{

public:
    AppContext();
    static AppContext& instance();

    bool putDisplayMsg(const char *msg, size_t msgLen, uint8_t msgRepeatNum=0);
    const QueueHandle_t& getDisplayMsgQHandle();
    void clearDisplayMsgQueue();

    bool putDisplayCmd(const AppDisplayCmd cmd);
    const QueueHandle_t& getDisplayCmdQHandle();

    bool putDateTimeMsg(const DateTime& dt);
    const QueueHandle_t& getTimeQHandle();

    /**
     * @brief Contains current system time, used (updated) only in Clock task
     */
    void setAppDt(const DateTime& dt);
    const DateTime& getAppDt();

    void setAppLang(i18n_lang lang);
    const char *getAppTranslated(i18n_msg_id id);

    void setAppStatus(AppStatusValue val);
    void clearAppStatus(AppStatusValue val);
    AppStatusValue appStatus() const;

private:

    // I18N map
    I18N m_i18n;

    // Global display message queue
    QueueHandle_t m_displayMsgQ;

    // Global display cmd queue
    QueueHandle_t m_displayCmdQ;

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

    // Current app status
    AppStatusValue m_appStatus;
};