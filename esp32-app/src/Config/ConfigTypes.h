#pragma once

#include <map>
#include <memory>
#include <cstdint>
#include <Arduino.h>

using ConfigParamKey = const char*;


class IConfig
{
public:
    virtual bool save(ConfigParamKey key, const void *cfg) = 0;
    virtual bool read(ConfigParamKey key, void *data) = 0;
};

struct IConfigData
{
};

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
    virtual size_t cfgDataSize() = 0;

protected:
    const char* m_key;
};

using ConfigParamMap = std::map<ConfigParamKey, ConfigParamBase&>;
