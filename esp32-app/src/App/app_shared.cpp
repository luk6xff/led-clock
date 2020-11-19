#include "app_shared.h"
#include "utils.h"

#define MODULE_NAME "[APSH]"

//------------------------------------------------------------------------------
AppShared::AppShared()
    : m_i18n(I18N_POLISH)
{
    m_displayMsgQ = xQueueCreate(4, sizeof(AppDisplayMsg));
    m_timeDataQ = xQueueCreate(1, sizeof(DateTime));
}

//------------------------------------------------------------------------------
AppShared& AppShared::instance()
{
    static AppShared appSh;
    return appSh;
}

//------------------------------------------------------------------------------
bool AppShared::putDisplayMsg(const char *msg, size_t msgLen)
{
    rtos::LockGuard<rtos::Mutex> lock(m_displayMsgMtx);
    if (!m_displayMsgQ)
    {
        utils::err("%s m_displayMsgQ has not been created!.", MODULE_NAME);
        return false;
    }
    if (msgLen > APP_DISPLAY_MSG_BUF_SIZE)
    {
        msgLen = APP_DISPLAY_MSG_BUF_SIZE;
    }
    memset(displayMsgBufer, 0, APP_DISPLAY_MSG_BUF_SIZE);
    memcpy(displayMsgBufer, msg, msgLen);
    utils::err("%s dispMsg: %s.", MODULE_NAME, displayMsgBufer);
    char *p = (char*)&displayMsgBufer[0];
    AppDisplayMsg dispMsg = { p, msgLen };
    return xQueueSendToBack(m_displayMsgQ, &dispMsg, 0) == pdPASS;
}

//------------------------------------------------------------------------------
const QueueHandle_t& AppShared::getDisplayQHandle()
{
    return m_displayMsgQ;
}

//------------------------------------------------------------------------------
bool AppShared::putDateTimeMsg(const DateTime& dt)
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
const QueueHandle_t& AppShared::getTimeQHandle()
{
    return m_timeDataQ;
}

//------------------------------------------------------------------------------
void AppShared::setAppDt(const DateTime& dt)
{
    rtos::LockGuard<rtos::Mutex> lock(m_appDtMtx);
    m_appDt = dt;
}

//------------------------------------------------------------------------------
const DateTime& AppShared::getAppDt()
{
    return m_appDt;
}

//------------------------------------------------------------------------------
void AppShared::setAppLang(i18n_lang lang)
{
    m_i18n.loadTranslation(lang);
}

//------------------------------------------------------------------------------
const char* AppShared::getAppTranslated(i18n_msg_id id)
{
    return m_i18n.translate(id).c_str();
}

//------------------------------------------------------------------------------
