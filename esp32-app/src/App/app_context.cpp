#include "app_context.h"
#include "utils.h"

#define MODULE_NAME "[ACTX]"

//------------------------------------------------------------------------------
AppContext::AppContext()
    : m_i18n(I18N_ENGLISH)
    , m_otaUpdateStatus(false)
{
    m_displayMsgQ = xQueueCreate(3, sizeof(AppDisplayMsg));
    m_timeDataQ = xQueueCreate(1, sizeof(DateTime));
}

//------------------------------------------------------------------------------
AppContext& AppContext::instance()
{
    static AppContext appCtx;
    return appCtx;
}

//------------------------------------------------------------------------------
bool AppContext::putDisplayMsg(const char *msg, size_t msgLen)
{
    rtos::LockGuard<rtos::Mutex> lock(m_displayMsgMtx);
    bool ret = false;

    if (!m_displayMsgQ)
    {
        utils::err("%s m_displayMsgQ has not been created!.", MODULE_NAME);
        return ret;
    }
    if (msgLen > APP_DISPLAY_MSG_BUF_SIZE)
    {
        msgLen = APP_DISPLAY_MSG_BUF_SIZE;
    }

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
            utils::err("%s dispMsg queue is full!, available space %d.", MODULE_NAME, uxQueueSpacesAvailable(m_displayMsgQ));
            free((void*)msgData);
        }
        else
        {
            ret = true;
            utils::err("%s dispMsg: %s sent succesfully!", MODULE_NAME, msgData);
        }
    }

    return ret;
}

//------------------------------------------------------------------------------
const QueueHandle_t& AppContext::getDisplayQHandle()
{
    return m_displayMsgQ;
}

//------------------------------------------------------------------------------
bool AppContext::putDateTimeMsg(const DateTime& dt)
{
    rtos::LockGuard<rtos::Mutex> lock(m_timeDataMtx);
    if (!m_timeDataQ)
    {
        utils::err("%s m_timeDataQ has not been created!.", MODULE_NAME);
        return false;
    }
    if (!const_cast<DateTime&>(dt).isValid())
    {
        utils::err("%s timeData:%s is invalid, sending skipped", MODULE_NAME,
                    const_cast<DateTime&>(dt).timestamp().c_str());
        return false;
    }
    utils::err("%s timeData: %s.", MODULE_NAME, const_cast<DateTime&>(dt).timestamp().c_str());
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
void AppContext::setAppOtaUpdateStatus(bool status)
{
    m_otaUpdateStatus = status;
}

//------------------------------------------------------------------------------
bool AppContext::getAppOtaUpdateStatus() const
{
    return m_otaUpdateStatus;
}

//------------------------------------------------------------------------------