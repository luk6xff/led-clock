#pragma once

#include <Arduino.h>

template<typename T>
struct InfoValue
{
    InfoValue(const char *name, T value, const char *unit="")
        : m_name(name)
        , m_value(String(value))
        , m_unit(unit)
    {
    }

    String toServerJson()
    {
        String json = "{\"name\":\""+ m_name + "\",\"value\":\"" + m_value + "\",\"unit\":\"" + m_unit + "\"}";
        return json;
    }

    String toValueString()
    {
        String val = m_name + ": " + m_value + " " + m_unit;
        return val;
    }

public:
    const String &name()
    {
        return m_name;
    }

    const String &val()
    {
        return m_value;
    }

    const String &unit()
    {
        return m_unit;
    }

private:
    String m_name;
    String m_value;
    String m_unit;
};



class DeviceInfo
{

public:
    explicit DeviceInfo();

    String latest();
    String createDevInfoTable();

private:
    InfoValue<String> buildAuthor;
    InfoValue<String> buildDate;
    InfoValue<String> buildTime;
    InfoValue<uint32_t> chipId;
    InfoValue<String> fullVersion;
    InfoValue<uint32_t> cpuFreqMhz;
    InfoValue<uint32_t> flashChipRealSize;
    InfoValue<uint32_t> appSize;
    InfoValue<uint32_t> freeHeap;
};

