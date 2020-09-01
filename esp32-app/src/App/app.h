
/**
 * @brief Class describing a complete application
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2020-09-01
 */


#define dbg(msg) App::dbgPrint(msg)

class App
{

public:
    explicit App();

    static App& instance();

    void setup();

    /**
     * @brief Send a line of info to serial output.  Works like vsprintf(), but checks the DEBUG flag.
     *        Print only if DEBUG flag is true.  Always returns the formatted string.
     */
    static char* dbgPrint(const char* format, ... );

private:

    static constexpr int const DEBUG = 1;

};