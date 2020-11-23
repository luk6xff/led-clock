
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
// #define TEST_LIGHT_SENSOR
// #define TEST_RTC
// #define TEST_RADIO
#define TEST_LORA //Use utils/LoraTestApp as receiver
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

#ifdef TEST_LORA
    #include "../../LORA/platform/arduino/lora-arduino.h"
    #include <esp_task_wdt.h>
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

#ifdef TEST_LORA
    #define WDT_TIMEOUT 10
    lora dev;
    lora_arduino arduino_dev;

    String outgoing;              // outgoing message
    byte msgCount = 0;            // count of outgoing messages
    byte localAddress = 0xFF;     // address of this device
    byte destination = 0xBB;      // destination to send to
    long lastSendTime = 0;        // last send time
    int interval = 2000;          // interval between sends

    void sendMessage(lora* const dev, String outgoing)
    {
        lora_beginPacket(dev, false);                   // start packet
        lora_write(dev, destination);              // add destination address
        lora_write(dev, localAddress);             // add sender address
        lora_write(dev, msgCount);                 // add message ID
        lora_write(dev, outgoing.length());        // add payload length
        //lora_print(dev, outgoing);                 // add payload
            lora_write_data(dev, (const uint8_t*)outgoing.c_str(), outgoing.length());
        lora_endPacket(dev, false);                     // finish packet and send it
        msgCount++;                           // increment message ID
    }

    void onReceive(void*ctx, int packetSize)
    {
        Serial.println(">>>Received from: 0x" + String(packetSize, HEX));
        if (packetSize == 0)
        {
            Serial.println("Packet size: %d" + String(packetSize));
            return;          // if there's no packet, return
        }

        lora *const dev = (lora*const) ctx;
        // read packet header bytes:
        int recipient = lora_read(dev);          // recipient address
        byte sender = lora_read(dev);            // sender address
        byte incomingMsgId = lora_read(dev);     // incoming msg ID
        byte incomingLength = lora_read(dev);    // incoming msg length

        String incoming = "";
        while (lora_available(dev))
        {
            incoming += (char)lora_read(dev);
        }

        if (incomingLength != incoming.length()) {   // check length for error
            //Serial.println("error: message length does not match length");
            return;                             // skip rest of function
        }

        // if the recipient isn't this device or broadcast,
        if (recipient != localAddress && recipient != 0xFF) {
            Serial.println("This message is not for me.");
            return;                             // skip rest of function
        }

        // if message is for this device, or broadcast, print details:
        Serial.println("Received from: 0x" + String(sender, HEX));
        Serial.println("Sent to: 0x" + String(recipient, HEX));
        Serial.println("Message ID: " + String(incomingMsgId));
        Serial.println("Message length: " + String(incomingLength));
        Serial.println("Message: " + incoming);
        Serial.println("RSSI: " + String(lora_packetRssi(dev)));
        //Serial.println("Snr: " + String(lora_packetSnr(dev)));
        Serial.println();
    }
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

#ifdef TEST_LORA
    dev.frequency = 433E6;
    dev.on_receive = NULL;
    dev.on_tx_done = NULL;
    arduino_dev.spi = new SPIClass(HSPI);
    arduino_dev.spi_settings = SPISettings();
    arduino_dev.mosi =  RADIO_MOSI_PIN;
    arduino_dev.miso =  RADIO_MISO_PIN;
    arduino_dev.sck =   RADIO_SCK_PIN;
    arduino_dev.nss =   RADIO_NSS_PIN;
    arduino_dev.reset = RADIO_RST_PIN;
    arduino_dev.dio0 =  RADIO_DI0_PIN;

    Serial.begin(9600);                   // initialize serial
    while (!Serial);

    Serial.println("LoRa Duplex");


    if (!lora_arduino_init(&dev, &arduino_dev)) {             // initialize ratio at 915 MHz
        while(1)
        {
            Serial.println("LoRa init failed. Check your connections.");               // if failed, do nothing
            delay(1000);
        }

    }
    int i = 0;
    while(i++ < 15)
    {
        delay(1000);
        Serial.println("...");
    }

    // enableLoopWDT();
    // disableCore0WDT();
    // disableCore1WDT();
    esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
    esp_task_wdt_add(NULL); //add current thread to WDT watch
    lora_onReceive(&dev, onReceive);
    lora_receive(&dev, 0);

    lora_dumpRegisters(&dev);
    Serial.println("LoRa init succeeded.");
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

#ifdef TEST_LORA
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

#ifdef TEST_LORA
    if (millis() - lastSendTime > interval)
    {
    String message = "HeLoRa from ESP32-LORA";   // send a message
    sendMessage(&dev, message);
    Serial.println("Sending " + message);
    lastSendTime = millis();            // timestamp the message
    interval = random(2000) + 1000;    // 2-3 seconds
    lora_receive(&dev, 0);
    }
    esp_task_wdt_reset();
#endif


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
