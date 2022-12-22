#pragma once

#include <map>
#include <ArduinoJson.h>

class ConfigParamBase
{
public:

    ConfigParamBase(const char* key)
    : m_key(key)
    {
    }

    const char* key()
    {
        return m_key;
    }

protected:
    const char* m_key;
};

template<typename ConfigDataType, typename ConfigHndlType>
class ConfigParam : public ConfigParamBase
{

public:

    using ParamsKey = uint32_t;
    using ParamsMap = std::map<uint32_t, const char*>;

    ConfigParam(const char* key, ConfigHndlType& cfgHndl),
        : ConfigParamBase(key)
        , m_cfgHndl(cfgHndl)
    {
        setCfgParamsMap();
    }

    virtual bool setConfig(const JsonObject& json) = 0;

    virtual void getConfig(String& configPayload) = 0;

    virtual String toStr() = 0;


protected:

    virtual void setCfgParamsMap() = 0;

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
    ConfigHndlType& m_cfgHndl;
    ParamsMap m_cfgParamsMap;
};
