
/**
 * @brief Led-Clock app
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2020-10-18
 */


#define TESTS_OFF

#ifdef TESTS_ON
    #include "Tests/tests.h"
#else
    #include "App/app.h"
#endif


//------------------------------------------------------------------------------
void setup(void)
{
#ifdef TESTS_ON
    tests_setup();
#else
    App::instance().setup();
#endif
}

//------------------------------------------------------------------------------
void loop(void)
{
#ifdef TESTS_ON
    tests_run();
#else
    //App::instance().run();
    vTaskDelete(nullptr);
#endif
}

//------------------------------------------------------------------------------
