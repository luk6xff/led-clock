
/**
 * @brief Some app configuration stuff
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2020-09-01
 */


// Define the version number, also used for webserver as Last-Modified header and to
// check version for update.  The format must be exactly as specified by the HTTP standard!
#define APP_VERSION     "0.0.1"

// LedClock App can be updated (OTA) to the latest version from a remote server.
#define APP_UPDATEHOST  "github.com"                    // Host for software updates
#define APP_BINFILE     "/firmware.bin"      // Binary file name for update software