
#include <bits/stdc++.h>

/// ArduinoJson-v6.17.2.h, taken from:  https://github.com/bblanchon/ArduinoJson/releases/tag/v6.17.2
#include "ArduinoJson-v6.17.2.h"


//------------------------------------------------------------------------------
static void test1()
{
    std::cout << "<<< TEST 1 >>>" << std::endl;
    StaticJsonDocument<300> doc;
    char json[] = "{\"sensor\":\"gps\",\"time\":1234567890,\"data\":[{\"lo\":48.756080, \"la\":2.302038}]}";
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, json);

    // Test if parsing succeeds.
    if (error)
    {
        std::cerr << "deserializeJson() failed: " << error.c_str() << std::endl;
        return;
    }

    // Extract values
    const char* sensor = doc["sensor"];
    long time = doc["time"];
    JsonArray arr = doc["data"].as<JsonArray>();
    double latitude = arr[0]["lo"];
    double longitude = doc["data"][0]["lo"];

    // Print values.
    std::cout << sensor << std::endl;
    std::cout << time << std::endl;
    std::cout << latitude << std::endl;
    std::cout << longitude << std::endl;

    std::cout << std::endl;
}

//------------------------------------------------------------------------------
static void test2()
{
    std::cout << "<<< TEST 2 >>>" << std::endl;
    StaticJsonDocument<300> doc;
    char json[] = "[{\"lo\":48.756080, \"la\":2.302038}]";
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, json);

    // Test if parsing succeeds.
    if (error)
    {
        std::cerr << "deserializeJson() failed: " << error.c_str() << std::endl;
        return;
    }

    // Extract values
    JsonArray arr = doc.as<JsonArray>();
    double latitude = arr[0]["lo"];
    double longitude = doc[0]["la"];

    // Print values.
    std::cout << latitude << std::endl;
    std::cout << longitude << std::endl;

    std::cout << std::endl;
}

//------------------------------------------------------------------------------
static void test3()
{
    std::cout << "<<< TEST 3 >>>" << std::endl;
    StaticJsonDocument<300> doc;
    char json[] = "[{\"lo\":48.756080, \"la\":2.302038}, {\"lo\":12.7, \"la\":34.30}]";
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, json);

    // Test if parsing succeeds.
    if (error)
    {
        std::cerr << "deserializeJson() failed: " << error.c_str() << std::endl;
        return;
    }

    // Extract values and print values.
    JsonArray arr = doc.as<JsonArray>();
    for (const auto& v : arr)
    {
        double latitude = v["lo"];
        double longitude = v["la"];
        std::cout << latitude << std::endl;
        std::cout << longitude << std::endl;
    }
    std::cout << std::endl;
}

//------------------------------------------------------------------------------
static void test4()
{
    std::cout << "<<< TEST 4 >>>" << std::endl;
    StaticJsonDocument<300> doc;
    char json[] = "{\"dev-cfg-wifi\" : [{\"lo\":48.756080, \"la\":2.302038}, {\"lo\":12.7, \"la\":34.30}]}";
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, json);

    // Test if parsing succeeds.
    if (error)
    {
        std::cerr << "deserializeJson() failed: " << error.c_str() << std::endl;
        return;
    }

    // Extract values and print values.
    JsonArray arr = doc["dev-cfg-wifi"].as<JsonArray>();
    for (const auto& v : arr)
    {
        double latitude = v["lo"];
        double longitude = v["la"];
        std::cout << latitude << std::endl;
        std::cout << longitude << std::endl;
    }

    JsonVariant var = doc.as<JsonVariant>();
    JsonObject jsonObj = var.as<JsonObject>();
    const char* key = "dev-cfg-wifi";
    if (jsonObj.containsKey(key))
    {
        std::cout << "It contains the key: "<< key << std::endl;
    }
    else
    {
        std::cout << "No key: " << "found" << std::endl;
    }
    std::cout << std::endl;
}

//------------------------------------------------------------------------------
static void test5()
{
    std::cout << "<<< TEST 5 >>>" << std::endl;

    struct WifiSettings
    {
        #define WIFI_CFG_KEY            "dev-cfg-wifi"
        #define WIFI_CFG_VAL_SSID       "wifi-ssid"
        #define WIFI_CFG_VAL_PASS       "wifi-pass"
        #define WIFI_CFG_VAL_AP_PASS    "wifi-ap-pass"

        #define WIFI_SETTINGS_LEN 32

        WifiSettings()
        {
            const char *admin = "admin";
            memset(this, 0, sizeof(*this));
            memcpy(ssid, admin, WIFI_SETTINGS_LEN);
            memcpy(pass, admin, WIFI_SETTINGS_LEN);
            memcpy(ap_pass, admin, WIFI_SETTINGS_LEN);
        }

        std::string toStr()
        {
            return "ssid :" + std::string(std::string(ssid) + " pass:" + std::string(pass) + " ap-pass:" + std::string(ap_pass));
        }

        std::string toJson()
        {
            StaticJsonDocument<256> doc;
            std::string json;
            JsonArray arr = doc.createNestedArray(WIFI_CFG_KEY);
            JsonObject obj = arr.createNestedObject();
            obj[WIFI_CFG_VAL_SSID] = ssid;
            obj = arr.createNestedObject();
            obj[WIFI_CFG_VAL_PASS] = pass;
            obj = arr.createNestedObject();
            obj[WIFI_CFG_VAL_AP_PASS] = ap_pass;
            serializeJson(doc, json);
            return json;
        }

        void fromJson(const JsonObject& json)
        {
            JsonArray arr = json[WIFI_CFG_KEY].as<JsonArray>();
            for (const auto& v : arr)
            {
                if (v[WIFI_CFG_VAL_SSID])
                {
                    memset(ssid, 0, WIFI_SETTINGS_LEN);
                    memcpy(ssid, v[WIFI_CFG_VAL_SSID].as<const char*>(), WIFI_SETTINGS_LEN);
                }
                else if (v[WIFI_CFG_VAL_PASS])
                {
                    memset(pass, 0, WIFI_SETTINGS_LEN);
                    memcpy(pass, v[WIFI_CFG_VAL_PASS].as<const char*>(), WIFI_SETTINGS_LEN);
                }
                else if (v[WIFI_CFG_VAL_AP_PASS])
                {
                    memset(ap_pass, 0, WIFI_SETTINGS_LEN);
                    memcpy(ap_pass, v[WIFI_CFG_VAL_AP_PASS].as<const char*>(), WIFI_SETTINGS_LEN);
                }
            }
        }

        char ssid[WIFI_SETTINGS_LEN];
        char pass[WIFI_SETTINGS_LEN];
        char ap_pass[WIFI_SETTINGS_LEN];
    };

    WifiSettings cfg;

    std::cout << "toJson(): "<< cfg.toJson() << std::endl;

    StaticJsonDocument<300> doc;


    DeserializationError error = deserializeJson(doc, cfg.toJson().c_str());
    //char json[] = "{\"dev-cfg-wifi\":[{\"wifi-ssid\":\"INTEHNExx\"},{\"wifi-pass\":\"Faza19xxxx\"},{\"wifi-ap-pass\":\"admin123\"}]}";
    //DeserializationError error = deserializeJson(doc, json);

    if (error)
    {
        std::cerr << "deserializeJson() failed: " << error.c_str() << std::endl;
        return;
    }

    std::cout << "BEFORE fromJson(): "<< cfg.toStr() << std::endl;
    cfg.fromJson(doc.as<JsonObject>());
    std::cout << "AFTER fromJson(): "<< cfg.toStr() << std::endl;


    JsonVariant var = doc.as<JsonVariant>();
    JsonObject jsonObj = var.as<JsonObject>();
    const char* key = "dev-cfg-wifi";
    if (jsonObj.containsKey(key))
    {
        std::cout << "It contains the key: "<< key << std::endl;
    }
    else
    {
        std::cout << "No key: " << "found" << std::endl;
    }
    std::cout << std::endl;
}


//------------------------------------------------------------------------------
int main()
{
    test1();
    test2();
    test3();
    test4();
    test5();
    return 0;
}
