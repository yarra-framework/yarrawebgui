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

    void initServerConfiguration();
    bool loadServerConfiguration();

    WString port;

    WString yarraPath;
    WString yarraLogPath;
    WString yarraModesPath;
    WString yarraQueuePath;
    WString yarraWorkPath;
    WString yarraFailPath;
    WString yarraStoragePath;
    WString yarraModulesPath;
    WString yarraModulesUserPath;
    WString matlabBinaryPath;
    WString yarraResumePath;
    bool    yarraEnableResume;

    WString serverName;

    vector<ywUser*> users;

    bool disableModuleInstallation;
    bool disableModeEditing;
    bool disableSupportForum;
    bool disableYarraNews;

    bool configurationValid;

    ywUser* validateUser(WString name, WString password);

};


inline bool ywConfiguration::isValid()
{
    return configurationValid;
}


#endif // YW_CONFIGURATION_H

