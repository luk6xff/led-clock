
/**
 * @brief Class describing a complete application
 *
 * @author Lukasz Uszko - luk6xff
 * @date   2020-09-01
 */


class App
{

public:
    explicit App();

    static App& instance();

    void setup();

private:

    static constexpr int const DEBUG = 1;

};