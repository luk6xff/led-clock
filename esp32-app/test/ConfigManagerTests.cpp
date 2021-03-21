#include <unity.h>
#include <stdio.h>
#include "../lib/ConfigManager/src/ConfigManager.h"



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void config_man_config_parameter(void)
{
    // Create parameters
    ConfigParameter<uint32_t> enable("enable_key", 10);
    ConfigParameter<uint32_t> updateInterval("updateInterval_key", 30);
    // Create parameters group
    ConfigParameterGroup cpg("dev-cfg-weather");
    // Add new parameter
    cpg.addParameter(&enable);
    cpg.createParameter("city_key", 12);

    printf("\n\n");
    TEST_ASSERT_EQUAL(1, 1);
}



//------------------------------------------------------------------------------
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(config_man_config_parameter);
    UNITY_END();
    return 0;
}