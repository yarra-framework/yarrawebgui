#ifndef YW_CONFIGURATION_H
#define YW_CONFIGURATION_H

#include <vector>
#include <Wt/WString>

using namespace Wt;
using namespace std;

class ywUser
{
public:
    WString name;
    WString password;
    int     level;
};


class ywConfiguration
{
public:
    ywConfiguration();
    ~ywConfiguration();

    void loadConfiguration();
    bool isValid();

    WString port;
    WString yarraPath;
    WString serverName;
    WString serverType;

    vector<ywUser*> users;

    bool configurationValid;

    ywUser* validateUser(WString name, WString password);

};


inline bool ywConfiguration::isValid()
{
    return configurationValid;
}


#endif // YW_CONFIGURATION_H

