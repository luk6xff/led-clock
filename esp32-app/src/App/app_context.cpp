#include "app_context.h"
#include "Rtos/logger.h"
#include <type_traits>
#include "day_info_runner.h"

#define MODULE_NAME "[ACTX]"

//------------------------------------------------------------------------------
AppContext::AppContext()
    : m_i18n(I18N_ENGLISH)
    , m_appStatus(AppStatusType::NO_ERROR)
{
    m_displayMsgQ = xQueueCreate(APP_DISPLAY_MSG_QUEUE_SIZE, sizeof(AppDisplayMsg));
    m_displayCmdQ = xQueueCreate(APP_DISPLAY_MSG_QUEUE_SIZE, sizeof(AppDisplayCmd));
    m_timeDataQ = xQueueCreate(APP_TIME_MSG_QUEUE_SIZE, sizeof(DateTime));
}

//------------------------------------------------------------------------------
AppContext& AppContext::instance()
{
    static AppContext appCtx;
    return appCtx;
}

//------------------------------------------------------------------------------
bool AppContext::putDisplayMsg(const char *msg, size_t msgLen, uint8_t msgRepeatNum)
{
    rtos::LockGuard<rtos::Mutex> lock(m_displayMsgMtx);
    bool ret = false;

    if (!m_displayMsgQ)
    {
        logger::err("%s m_displayMsgQ has not been created!.", MODULE_NAME);
        return ret;
    }

    if (msgLen > APP_DISPLAY_MSG_BUF_SIZE)
    {
        msgLen = APP_DISPLAY_MSG_BUF_SIZE;
    }

    // Add +1 for use in for loop
    msgRepeatNum++;

    for (size_t i = 0; i < msgRepeatNum && i < APP_DISPLAY_MSG_QUEUE_SIZE; i++)
    {
        const size_t msgDataSize = msgLen + 1;
        char *msgData = (char*)malloc(msgDataSize);
        if (msgData)
        {
            memset(msgData, 0, msgDataSize);
            memcpy(msgData, msg, msgDataSize);
            AppDisplayMsg dispMsg = { msgData, msgDataSize };
            ret = xQueueSendToBack(m_displayMsgQ, &dispMsg, 0) == pdPASS;
            if (!ret)
            {
                logger::err("%s dispMsg queue is full!, available space %d.", MODULE_NAME, uxQueueSpacesAvailable(m_displayMsgQ));
                free((void*)msgData);
                break;
            }
            else
            {
                ret = true;
                logger::inf("%s dispMsg: %s sent succesfully!", MODULE_NAME, msgData);
            }
        }
    }

    return ret;
}

//------------------------------------------------------------------------------
const QueueHandle_t& AppContext::getDisplayMsgQHandle()
{
    return m_displayMsgQ;
}

//------------------------------------------------------------------------------
void AppContext::clearDisplayMsgQueue()
{
    if (!getDisplayMsgQHandle())
    {
        return;
    }
    AppDisplayMsg dispMsg;
    while (xQueueReceive(getDisplayMsgQHandle(), &dispMsg, 0) == pdPASS)
    {
        free(dispMsg.msg);
    }
    xQueueReset(getDisplayMsgQHandle());
}

//------------------------------------------------------------------------------
bool AppContext::putDisplayCmd(const AppDisplayCmd cmd)
{
    bool ret = false;

    if (!m_displayCmdQ)
    {
        logger::err("%s m_displayCmdQ has not been created!.", MODULE_NAME);
        return ret;
    }

    ret = xQueueSendToBack(m_displayCmdQ, &cmd, 0) == pdPASS;
    if (!ret)
    {
        logger::err("%s dispCmd queue is full!", MODULE_NAME);
    }
    else
    {
        ret = true;
        logger::inf("%s dispCmd: %d sent succesfully!", MODULE_NAME, cmd);
    }

    return ret;
}

//------------------------------------------------------------------------------
const QueueHandle_t& AppContext::getDisplayCmdQHandle()
{
    return m_displayCmdQ;
}

//------------------------------------------------------------------------------
bool AppContext::putDateTimeMsg(const DateTime& dt)
{
    rtos::LockGuard<rtos::Mutex> lock(m_timeDataMtx);
    if (!m_timeDataQ)
    {
        logger::err("%s m_timeDataQ has not been created!.", MODULE_NAME);
        return false;
    }
    if (!const_cast<DateTime&>(dt).isValid())
    {
        logger::err("%s timeData:%s is invalid, sending skipped", MODULE_NAME,
                    const_cast<DateTime&>(dt).timestamp().c_str());
        return false;
    }
    logger::err("%s timeData: %s.", MODULE_NAME, const_cast<DateTime&>(dt).timestamp().c_str());
    return xQueueSendToBack(m_timeDataQ, &dt, 0) == pdPASS;
}

//------------------------------------------------------------------------------
const QueueHandle_t& AppContext::getTimeQHandle()
{
    return m_timeDataQ;
}

//------------------------------------------------------------------------------
void AppContext::setAppDt(const DateTime& dt)
{
    rtos::LockGuard<rtos::Mutex> lock(m_appDtMtx);
    m_appDt = dt;
    // Check day info
    DayInfoRunner dInfRun(dt);
    dInfRun.process();
}

//------------------------------------------------------------------------------
const DateTime& AppContext::getAppDt()
{
    return m_appDt;
}

//------------------------------------------------------------------------------
void AppContext::setAppLang(i18n_lang lang)
{
    m_i18n.loadTranslation(lang);
}

//------------------------------------------------------------------------------
const char* AppContext::getAppTranslated(i18n_msg_id id)
{
    return m_i18n.translate(id).c_str();
}

//------------------------------------------------------------------------------
void AppContext::setAppStatus(AppStatusValue val)
{
    m_appStatus |= val;
}

//------------------------------------------------------------------------------
void AppContext::clearAppStatus(AppStatusValue val)
{
    m_appStatus &= ~(val);
}

//------------------------------------------------------------------------------
AppStatusValue AppContext::appStatus() const
{
    return m_appStatus;
}

//------------------------------------------------------------------------------