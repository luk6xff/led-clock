#include <unity.h>
#include <stdio.h>
#include "../lib/ConfigManager/src/ConfigManager.h"



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void config_man_config_parameter(void)
{
    uint32_t enable;
    uint32_t updateInterval; // [s] in seconds    C
    ConfigParameter<uint32_t> enable("test_key", 10);
    ConfigParameter<uint32_t> updateInterval("test_key", &updateInterval);

    ConfigParameterGroup cpg("dev-cfg-weather");

    cpg.addParameter(enable);

    printf("\n\n");
    TEST_ASSERT_EQUAL(1, 0);
}



//------------------------------------------------------------------------------
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(config_man_config_parameter);
    UNITY_END();
    return 0;
}