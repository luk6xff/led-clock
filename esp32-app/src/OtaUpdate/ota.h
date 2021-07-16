#pragma once

#include <ESPAsyncWebServer.h>


class OtaUpdate
{

public:
    OtaUpdate();

    void handleOtaRequest(AsyncWebServerRequest *request, const String& filename,
                            size_t index, uint8_t *data, size_t len, bool final);
private:
    void printOtaUpdateProgress(size_t prg, size_t sz);

private:
    size_t m_otaFileContentLen;
};
