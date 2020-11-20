
#include "tests.h"

#include "../App/app.h"
#include "../App/utils.h"
#include "../hw_config.h"

//------------------------------------------------------------------------------
/**
 * @brief Enabled/Disabled TEST Defines
 */
// #define TEST_I2C_SCANNER
// #define TEST_RTOS_TASK
// #define TEST_DISPLAY
#define TEST_LIGHT_SENSOR
// #define TEST_RTC
// #define TEST_RADIO
// #define TEST_NTP
// #define TEST_CAPTIVE_PORTAL
// #define TEST_WEB_SERVER

//------------------------------------------------------------------------------
#if defined(TEST_RTC) || defined(TEST_DISPLAY)
    #include "../Display/display.h"
    #include "../Display/light_sensor.h"
    #include "../Clock/system_time.h"
#endif

#ifdef TEST_LIGHT_SENSOR
    #include "../Display/light_sensor.h"
#endif

#ifdef TEST_RADIO
    #include "../Radio/radio.h"
#endif

#ifdef TEST_NTP
    #include "../Clock/ntp.h"
    #include "WiFi.h"
#endif

#ifdef TEST_CAPTIVE_PORTAL
    #include <WiFiManager.h>
#endif

#ifdef TEST_WEB_SERVER
    #include <WiFi.h>
    #include <ESPmDNS.h>
    #include <ArduinoOTA.h>
    #include <WebServer.h>
#endif


//------------------------------------------------------------------------------
/**
 * @brief TEST Globals
 */
//#define TEST_RTOS_TASK
#ifdef TEST_RTOS_TASK
    #define BLINK_GPIO (gpio_num_t)CONFIG_BLINK_GPIO
    #ifndef LED_BUILTIN
        #define LED_BUILTIN 4
    #endif

    /**
     * @brief FreeRtos demo blink task
     */
    void blink_task(void *pvParameter)
    {
        gpio_pad_select_gpio(BLINK_GPIO);
        /* Set the GPIO as a push/pull output */
        gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
        while(1)
        {
            /* Blink off (output low) */
            gpio_set_level(BLINK_GPIO, 0);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            /* Blink on (output high) */
            gpio_set_level(BLINK_GPIO, 1);
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
    }
#endif

#ifdef TEST_DISPLAY
#endif

#ifdef TEST_LIGHT_SENSOR
#endif

#ifdef TEST_RTC
#endif

#ifdef TEST_RADIO
#endif

#ifdef TEST_NTP
#endif

#ifdef TEST_CAPTIVE_PORTAL
    WiFiManager wm;
#endif

#ifdef TEST_WEB_SERVER
    const char* ssid = "INTEHNXXX";
    const char* password = "Faza1994XX";

    WebServer server(80);

    const char* www_username = "admin";
    const char* www_password = "admin";
    // Allows you to set the realm of authentication Default:"Login Required"
    const char* www_realm = "Custom Auth Realm";
    // the Content of the HTML response in case of Unautherized Access Default:empty
    String authFailResponse = "Authentication Failed";
#endif


//------------------------------------------------------------------------------
void tests_setup()
{
    // Utils init
    utils::init();
#ifdef TEST_I2C_SCANNER
    utils::dbg("SCANNING I2C_BUS_0...");
    utils::util_i2c_scanner(0);
    utils::dbg("SCANNING I2C_BUS_1...");
    utils::util_i2c_scanner(1);
#endif

#ifdef TEST_RTOS_TASK
    xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
#endif

#ifdef TEST_DISPLAY
#endif

#ifdef TEST_LIGHT_SENSOR
#endif

#ifdef TEST_RTC
#endif

#ifdef TEST_RADIO
#endif

#ifdef TEST_NTP
#endif

#ifdef TEST_CAPTIVE_PORTAL
    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    // put your setup code here, to run once:
    Serial.begin(9600);

    //reset settings - wipe credentials for testing
    //wm.resetSettings();

    wm.setConfigPortalBlocking(false);

    //automatically connect using saved credentials if they exist
    //If connection fails it starts an access point with the specified name
    if (wm.autoConnect())
    {
        Serial.println("connected...yeey :)");
    }
    else
    {
        Serial.println("Configportal running");
    }
#endif

#ifdef TEST_WEB_SERVER
    Serial.begin(9600);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.println("WiFi Connect Failed! Rebooting...");
        delay(1000);
        ESP.restart();
    }
    ArduinoOTA.begin();

    server.on("/", []() {
        if (!server.authenticate(www_username, www_password))
            //Basic Auth Method with Custom realm and Failure Response
            //return server.requestAuthentication(BASIC_AUTH, www_realm, authFailResponse);
            //Digest Auth Method with realm="Login Required" and empty Failure Response
            //return server.requestAuthentication(DIGEST_AUTH);
            //Digest Auth Method with Custom realm and empty Failure Response
            //return server.requestAuthentication(DIGEST_AUTH, www_realm);
            //Digest Auth Method with Custom realm and Failure Response
        {
            return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
        }
        server.send(200, "text/plain", "Login OK");
    });
    server.begin();

    Serial.print("Open http://");
    Serial.print(WiFi.localIP());
    Serial.println("/ in your browser to see it working");

    if (MDNS.begin("ubiad-ledclock"))
    {
        Serial.println("MDNS responder started");
    }
#endif
}


//------------------------------------------------------------------------------
void tests_run()
{

////////////////////////////////////////////////////////////////////////////////
///<! SETUP
////////////////////////////////////////////////////////////////////////////////
#ifdef TEST_DISPLAY
    Display::MAX72xxConfig cfg =
    {
        DISPLAY_MAX72XX_HW_TYPE,
        DISPLAY_MAX72XX_MODULES_NUM,
        DISPLAY_DIN_PIN,
        DISPLAY_CLK_PIN,
        DISPLAY_CS_PIN,
    };
    Display disp(cfg);

    static bool	timeDots = true;
    static uint32_t timeLast = 0;
    static uint32_t timeSecCnt = 0;
    timeLast   = millis();
    timeSecCnt =  millis();
    Display::DateTimePrintMode timeDispMode = Display::THM;
#endif

#ifdef TEST_LIGHT_SENSOR
    LightSensor light;
#endif

#if defined(TEST_RTC) || defined(TEST_DISPLAY)
    // Timezones
    // Central European Time (Frankfurt, Paris, Warsaw)
    // SystemTimeSettings timeConfig = {
    //     {"CEST", Last, Sun, Mar, 2, 120}, // Central European Summer Time
    //     {"CET ", Last, Sun, Oct, 3, 60},  // Central European Standard Time
    // };
    SystemTimeSettings timeConfig;
    SystemTime time(timeConfig);
#endif

#ifdef TEST_RADIO
    RadioSensorSettings radioConfig = {
        .crit_vbatt_level = 3000,
        .update_data_interval = 30,
    };
    Radio radio(radioConfig);
#endif

#ifdef TEST_NTP
    WiFi.begin("INTEHNXXX", "Faza1994XX");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        utils::dbg(".");
    }
    NtpSettings ntpConfig(0, 10000, "time.google.com");//"pl.pool.ntp.org");
    Ntp ntp(ntpConfig);
#endif


#ifdef TEST_CAPTIVE_PORTAL
    wm.process();
#endif


#ifdef TEST_WEB_SERVER

#endif



////////////////////////////////////////////////////////////////////////////////
///<! LOOP
////////////////////////////////////////////////////////////////////////////////
    while(1)
    {

#ifdef TEST_DISPLAY
        disp.update();
        disp.processAutoIntensityLevelControl();
        DateTime dt = time.getTime();

        if (millis() - timeLast >= 10000) // 10[s]
        {
            disp.printTime(dt, Display::DWYMD);
            timeLast = millis();
            if (timeDispMode == Display::THM)
            {
                timeDispMode = Display::THMS;
            }
            else
            {
                timeDispMode = Display::THM;
            }

        }
        else
        {
            disp.printTime(dt, timeDispMode, timeDots);
        }

        if (millis() - timeSecCnt >= 1000) // 1[s]
        {
            timeDots = timeDots ^ 1;
            timeSecCnt = millis();
        }
#endif // TEST_DISPLAY


#ifdef TEST_LIGHT_SENSOR
        utils::dbg("BH1750 Light: %3.2f [lx]", light.getIlluminance());
#endif // TEST_LIGHT_SENSOR


#ifdef TEST_RTC
        utils::dbg("SystemTime temperature: %3.2f", time.getTemperature());
        utils::dbg("SystemTime time: %s", SystemTime::timeToStr(time.getTime()));
        utils::dbg("SystemTime date: %s", SystemTime::dateToStr(time.getTime()));
#endif // TEST_RTC


#ifdef TEST_RADIO
        // Empty, all is done in radio.cpp
#endif  // TEST_RADIO


#ifdef TEST_NTP
        if (ntp.updateTime())
        {
            DateTime dt(ntp.getCurrentTime());
            utils::dbg("NTP UTC:%s", dt.timestamp().c_str());
            time.setUtcTime(dt);
        }
#endif

#ifdef TEST_CAPTIVE_PORTAL

#endif

#ifdef TEST_WEB_SERVER
    ArduinoOTA.handle();
    server.handleClient();
#endif
    }
}

//------------------------------------------------------------------------------
