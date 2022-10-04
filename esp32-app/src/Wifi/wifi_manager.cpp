/**
 * @brief WifiManager definition
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2022-03-30
 */

#include "wifi_manager.h"
#include <ESPmDNS.h>
#include "App/app_config.h"
#include "esp_wifi.h"

//------------------------------------------------------------------------------
#define MODULE_NAME "[WIFM]"

//------------------------------------------------------------------------------
WifiManager::WifiManager(const char *hostname)
    : k_apHostname(hostname)
    , m_webServer(k_apHostname)
{

}

//------------------------------------------------------------------------------
bool WifiManager::start()
{
    int attempt = 0;
    bool success = false;
    const int wifiConnectAttempts = 10;

    // Reset Wifi and set to station
    reset();
    WiFi.mode(WIFI_STA);

    while (attempt < wifiConnectAttempts && !success)
    {
        attempt++;
        utils::inf("\nWifi connection attempt num: %d", attempt);
        success = wifiConnect(AppCfg.getCurrent().wifi.ssid, AppCfg.getCurrent().wifi.pass);
    }

    if (success)
    {
        utils::inf("Wifi WebServer/Portal starting...");
        startWebPortal();
    }
    else
    {
        utils::inf("\nWifi connection could not be established");
        utils::inf("WifiMode AP (Captive Portal) starting...");
        startCaptivePortal();
    }

    return success;
}

//------------------------------------------------------------------------------
void WifiManager::process()
{
    if (isAPModeActive())
    {
        m_dnsServer.processNextRequest();
    }
}

//------------------------------------------------------------------------------
bool WifiManager::isAPModeActive()
{
    return WiFi.getMode() == WIFI_MODE_AP;
}

//------------------------------------------------------------------------------
bool WifiManager::isStationModeActive()
{
    return WiFi.getMode() == WIFI_MODE_STA;
}

//------------------------------------------------------------------------------
bool WifiManager::isConnected()
{
  return WiFi.status() == WL_CONNECTED;
}

//------------------------------------------------------------------------------
bool WifiManager::wifiConnect(const char *ssid, const char *password)
{
    utils::inf("%s Waiting for WiFi to connect", MODULE_NAME);

    bool connected = false;
    int retry = 0;
    const int wifiConnectRetries = 10;
    const int wifiConnectInterval = 500; //[ms]
    WiFi.begin(ssid, password[0] == '\0' ? NULL : password);

    while (retry < wifiConnectRetries && !connected)
    {
        utils::inf(".");
        connected = isConnected();
        if (connected)
        {
            utils::inf("%s Connected to:%s, IPaddress:%s", MODULE_NAME,
                WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
            WiFi.mode(WIFI_STA);
        }

        // Sleep a while
        delay(wifiConnectInterval);
        retry++;
    }

    return connected;
}

//------------------------------------------------------------------------------
bool WifiManager::startCaptivePortal()
{
    bool ret;

    utils::inf("Starting Access Point");

    // Reset Wifi and set to AP mode
    reset();
    WiFi.mode(WIFI_AP);

    String apHostname(k_apHostname);
    String apPass(AppCfg.getCurrent().wifi.ap_pass);
    if (apHostname == "")
    {
        apHostname = "emp-gateway";
    }
    if (apPass == "" || apPass.length() < 8 || apPass.length() > 63)
    {
        utils::inf("AccessPoint set password is INVALID or <8 or >63 chars, default `admin123` set instead");
        apPass = "admin123";
    }

    ret = WiFi.softAP(apHostname.c_str(), apPass.c_str());
    if (!ret)
    {
        utils::err("Error occured during setting Wifi.softAP!");
        return false;
    }

    utils::inf("Setting softAP Hostname: %s", apHostname.c_str());
    ret = WiFi.softAPsetHostname(apHostname.c_str());
    if (!ret)
    {
        utils::err("Hostname: AP set failed!");
    }
    else
    {
        utils::inf("Hostname: AP: %s", WiFi.softAPgetHostname());
    }

    IPAddress localIp(192,168,4,1);
    IPAddress gateway(192,168,4,1);
    IPAddress subnetMask(255,255,255,0);
    ret = WiFi.softAPConfig(localIp, gateway, subnetMask);
    if (!ret)
    {
        utils::err("WiFi.softAPConfig set failed!");
        return false;
    }

    delay(500);  // Need to wait to get IP
    utils::inf("AP Name: %s", k_apHostname);
    utils::inf("AP IP address: %s", WiFi.softAPIP().toString().c_str());

    m_dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    ret = m_dnsServer.start(k_dnsPort, "*", localIp);
    if (!ret)
    {
        utils::err("dnsServer.start failed!");
        return false;
    }

    // Start web server
    return m_webServer.startWebServer();
}

//------------------------------------------------------------------------------
bool WifiManager::startWebPortal()
{
    // Stop DNS server if running
    m_dnsServer.stop();
    // Start web server
    return m_webServer.startWebServer();
}

//------------------------------------------------------------------------------
void WifiManager::reset()
{
    WiFi.mode(WIFI_OFF);
    WiFi.softAPdisconnect(true);
    WiFi.disconnect(true);
    delay(1000);
}

//------------------------------------------------------------------------------
