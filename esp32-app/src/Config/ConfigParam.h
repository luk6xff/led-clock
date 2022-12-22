#pragma once

#include "ConfigTypes.h"
#include <map>
#include <ArduinoJson.h>
#include <cstring>

using ParamsKey = uint32_t;
using ParamsMap = std::map<uint32_t, const char*>;

template<typename ConfigDataType>
class ConfigParam : public ConfigParamBase
{

public:

    ConfigParam(const char* key, IConfig& cfgHndl)
    : ConfigParamBase(key)
    , m_cfgHndl(cfgHndl)
    {
        setCfgParamsMap();
    }

    virtual bool setConfigFromJson(const JsonObject& json) = 0;
    virtual String toStr() override = 0;

    bool setConfigFromRaw(const void* data)
    {
        if (data)
        {
            std::memcpy(&m_cfgData, data, cfgDataSize());
            return true;
        }
        return false;
    }

    void* getConfigAsRaw()
    {
        return (void*)&m_cfgData;
    }

    void getConfigAsStr(String& configPayload)
    {
        // Extract config data from application
        configPayload = packToJson(m_cfgData);
    }

    const ConfigDataType& cfgData()
    {
        return m_cfgData;
    }

    size_t cfgDataSize() override
    {
        return sizeof(m_cfgData);
    }

protected:

    virtual void setCfgParamsMap() {};

    virtual bool unpackFromJson(ConfigDataType& cfgData, const JsonObject& json) = 0;

    virtual String packToJson(const ConfigDataType& data) = 0;

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
    ConfigDataType m_cfgData;
    IConfig& m_cfgHndl;
    ParamsMap m_cfgParamsMap;
};
