#pragma once

#include <map>
#include <ArduinoJson.h>


template<typename WebConfigDataType>
class WebConfigParam
{

public:

    using ParamsKey = uint32_t;
    using ParamsMap = std::map<uint32_t, const char*>;

    WebConfigParam(const char* key)
    : m_key(key)
    {}

    virtual bool setConfig(const JsonObject& json) = 0;

    virtual void getConfig(String& configPayload) = 0;

protected:
    virtual void setCfgParamsMap() = 0;

    virtual bool unpackFromJson(WebConfigDataType& cfgData, const JsonObject& json) = 0;

    virtual String packToJson(const WebConfigDataType& data) = 0;

    const char* cfgParamKey(ParamsKey key)
    {
        if (m_cfgParamsMap.find(key) != m_cfgParamsMap.end())
        {
            return m_cfgParamsMap[key];
        }
        return nullptr;
    }

    static bool checkIfKeyExistsInJson(const char *key, const JsonObject& json)
    {
        if (!key && std::strlen(key) > 0)
        {
            return false;
        }

        if (json.containsKey(key))
        {
            return true;
        }
        return false;
    }

protected:
    const char* m_key;
    WebConfigDataType m_cfgData;
    ParamsMap m_cfgParamsMap;
};
