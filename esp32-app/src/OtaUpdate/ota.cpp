#include "ota.h"
#include <functional>
#include <esp_partition.h>
#include <Update.h>
#include "DebugUtils.h"
#include "App/app_context.h"

using namespace std::placeholders;

//------------------------------------------------------------------------------
OtaUpdate::OtaUpdate()
{
    Update.onProgress(std::bind(&OtaUpdate::printOtaUpdateProgress, this, _1, _2));
}

//------------------------------------------------------------------------------
void OtaUpdate::handleOtaRequest(AsyncWebServerRequest *request, const String& filename,
                                size_t index, uint8_t *data, size_t len, bool final)
{
    if (!index)
    {
        AppCtx.setAppStatus(AppStatusType::OTA_UPDATE_RUNNING);
        utils::inf("OTA Update starting...");
        m_otaFileContentLen = request->contentLength();
        // If filename includes spiffs, update the spiffs partition
        const int storageType = (filename.indexOf("spiffs") > -1) ? U_SPIFFS : U_FLASH;
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, storageType))
        {
            Update.printError(Serial);
        }
    }

    if (Update.write(data, len) != len)
    {
        Update.printError(Serial);
    }

    if (final)
    {
        AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "Please wait while the device reboots");
        response->addHeader("Refresh", "20");
        response->addHeader("Location", "/");
        request->send(response);
        if (!Update.end(true))
        {
            Update.printError(Serial);
            AppCtx.setAppStatus(AppStatusType::OTA_UPDATE_RUNNING);
        }
        else
        {
            utils::inf("OTA Update completed!");
            AppCtx.clearAppStatus(AppStatusType::OTA_UPDATE_RUNNING);
            ESP.restart();
        }
    }
}

//------------------------------------------------------------------------------
void OtaUpdate::printOtaUpdateProgress(size_t prg, size_t sz)
{
    utils::inf("Progress: %d%%\n", (prg*100) / m_otaFileContentLen);
}


//------------------------------------------------------------------------------
