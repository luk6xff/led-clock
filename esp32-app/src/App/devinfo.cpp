#include "devinfo.h"

DeviceInfo::DeviceInfo()
    : buildAuthor("Build author", "luk6xff")
    , buildDate("Build date", F(__DATE__))
    , buildTime("Build time", F(__TIME__))
    , chipId("Chip ID", ESP.getChipRevision())
    , fullVersion("SDKversion", ESP.getSdkVersion())
    , cpuFreqMhz("CPU Freq", ESP.getCpuFreqMHz(), "MHz")
    , flashChipRealSize("Chip ID", ESP.getFlashChipSize())
    , appSize("Application size", ESP.getSketchSize(), "Bytes")
    , freeHeap("Free Heap", ESP.getFreeHeap(), "Bytes", "glyphicon-indent-left")
{

}

//------------------------------------------------------------------------------
String DeviceInfo::latest()
{
    String res = "<<<DeviceInfo>>>\r\n";
    res += buildAuthor.toValueString() + "\r\n"
            + buildDate.toValueString() + "\r\n"
            + buildTime.toValueString() + "\r\n"
            + chipId.toValueString() + "\r\n"
            + fullVersion.toValueString() + "\r\n"
            + cpuFreqMhz.toValueString() + "\r\n"
            + flashChipRealSize.toValueString() + "\r\n"
            + appSize.toValueString() + "\r\n"
            + freeHeap.toValueString();
    res += "\r\n";
    return res;
}

//------------------------------------------------------------------------------
String DeviceInfo::createDevInfoTable()
{
    String json = "[";
    json += buildAuthor.toServerJson() + ","
            + buildDate.toServerJson() + ","
            + buildTime.toServerJson() + ","
            + chipId.toServerJson() + ","
            + fullVersion.toServerJson() + ","
            + cpuFreqMhz.toServerJson() + ","
            + flashChipRealSize.toServerJson() + ","
            + appSize.toServerJson() + ","
            + freeHeap.toServerJson();
    json += "]";
    return json;
}

//------------------------------------------------------------------------------