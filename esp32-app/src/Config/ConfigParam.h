#pragma once

#include <map>
#include <ArduinoJson.h>
#include <cstring>

using ParamsKey = uint32_t;
using ParamsMap = std::map<uint32_t, const char*>;

class ConfigParamBase
{
public:

    explicit ConfigParamBase(const char* key)
    : m_key(key)
    {
    }

    const char* key()
    {
        return m_key;
    }

    virtual String toStr() = 0;

protected:
    const char* m_key;
};

template<typename ConfigDataType, typename ConfigHndlType>
class ConfigParam : public ConfigParamBase
{

public:

    ConfigParam(const char* key, ConfigHndlType& cfgHndl)
        : ConfigParamBase(key)
        , m_cfgHndl(cfgHndl)
    {
        setCfgParamsMap();
    }

    virtual bool setConfigFromJson(const JsonObject& json) = 0;

    virtual void getConfigAsStr(String& configPayload) = 0;

    virtual String toStr() override = 0;


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
    ConfigHndlType& m_cfgHndl;
    ParamsMap m_cfgParamsMap;
};
