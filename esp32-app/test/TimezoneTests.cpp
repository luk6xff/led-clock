#include <unity.h>
#include <stdio.h>

#include "../lib/Timezone/src/Timezone.h"



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void timezone_setup(void)
{
    printf("\n\n");
    TimeChangeRule CET{"CET", Last, Sun, Oct, 3, 60}; // Central European Standard Time
    TimeChangeRule CEST{"CEST", Last, Sun, Mar, 2, 120}; // Central European Summer Time
    Timezone tz(CET, CEST);
    
    TEST_ASSERT_EQUAL(1, 1);
}



//------------------------------------------------------------------------------
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(timezone_setup);
    UNITY_END();
    return 0;
}