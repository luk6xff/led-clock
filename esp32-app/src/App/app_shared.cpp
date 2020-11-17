#include "app_shared.h"
#include "utils.h"

#define MODULE_NAME "[APSH]"

//------------------------------------------------------------------------------
AppShared::AppShared()
{
    m_displayMsgQ = xQueueCreate(4, sizeof(AppDisplayMsg));
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
    if (!m_displayMsgQ)
    {
        utils::err("%s m_ntpTimeQ has not been created!.", MODULE_NAME);
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
    AppDisplayMsg dispMsg = { p, msgLen};
    return xQueueSendToBack(m_displayMsgQ, &dispMsg, 0) == pdPASS;
}

//------------------------------------------------------------------------------
const QueueHandle_t& AppShared::getDisplayQHandle()
{
    return m_displayMsgQ;
}

//------------------------------------------------------------------------------
