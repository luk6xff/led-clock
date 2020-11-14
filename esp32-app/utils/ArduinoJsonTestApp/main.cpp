
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

}


//------------------------------------------------------------------------------
int main()
{
    test1();
    test2();
    test3();
    test4();
    return 0;
}
