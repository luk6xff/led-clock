#include <unity.h>
#include <stdio.h>
#include "../lib/ConfigManager/src/ConfigManager.h"



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void config_man_config_parameter(void)
{
    uint32_t version;
    ConfigParameter<uint32_t> c("test_key", &version);
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