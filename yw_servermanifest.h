#ifndef YW_SERVERMANIFEST_H
#define YW_SERVERMANIFEST_H

#include "yw_global.h"

#include <Wt/WString>

using namespace Wt;


class ywServerManifest
{
public:
    ywServerManifest(WString yarraPath);

    WString id;
    WString version;
    WString releaseDate;
    WString versionServerCore;
    WString versionWebGUI;

    WString minimumVersionForUpdate;
    WStringList filesToRemoveForUpdate;


    bool readManifest(WString filename="");
    WString renderInformation();

    bool requiresUpdate(WString latestVersionString);
    bool canUpdateVersion(WString installedVersionString);
    float versionStringToFloat(WString versionString);

    WString localYarraPath;

};

#endif // YW_SERVERMANIFEST_H
